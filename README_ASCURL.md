# libcurl in AngelScript (aka ascurl)

* Take advantage of `libcurl.dll` or `libcurl.so` via AngelScript

## constants

```
const int ASCURL_METHOD_GET = 0;
const int ASCURL_METHOD_POST = 1;
const int ASCURL_METHOD_PUT = 2;
```

## Create a new HTTP Request

```
//int g_EngineFuncs.CreateHTTPRequest(const string& in url, bool async, int method, int conn_timeout_ms, int timeout_ms)

int request_id = g_EngineFuncs.CreateHTTPRequest("http://curl.se", 
				true,//false=sync, true=async
				ASCURL_METHOD_PUT,
				1000,//connect timeout in ms
				5000 //transfer timeout in ms
			);
```

## Set post field of http request (mostly POST)

```
//bool g_EngineFuncs.SetHTTPRequestPostField(int request_id, const string& in post_fields )

g_EngineFuncs.SetHTTPRequestPostField(request_id, "xyz=114514");//post_fields must be urlencoded before passed to SetHTTPRequestPostField 
```

```
//bool g_EngineFuncs.SetHTTPRequestPostFieldEx(int request_id, const string& in post_fields, int sizeof_post_fields )

g_EngineFuncs.SetHTTPRequestPostFieldEx(request_id, "xyz=114514");//post_fields must be urlencoded before passed to SetHTTPRequestPostField , sizeof_post_fields is used as size of post_fields. post_fields.length() is ignored
```

## Append request header to http request

```
//bool g_EngineFuncs.AppendHTTPRequestHeader(int request_id, const string& in header )

g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Cache-control: no-cache");
g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Content-Type: text/plain");
```

## Append form data to http request (mostly POST)

```
//bool AppendHTTPRequestFormString(int request_id, const string& in form, const string& in content )

g_EngineFuncs.AppendHTTPRequestFormString(request_id, "filename", "test.jpg");
```

```
//bool AppendHTTPRequestFormBlob(int request_id, const string& in form, const BLOB& in blob )

g_EngineFuncs.AppendHTTPRequestFormBlob(request_id, "filedata", pBlob);
```

## Upload using PUT method

```
//bool SetHTTPRequestUploadBlob(int request_id, const BLOB& in blob )

g_EngineFuncs.SetHTTPRequestUploadBlob(request_id, pBlob);
```

## Set asynchronous callback for http request

```
//void HTTPResponseCallback(int request_id)
//bool SetHTTPRequestCallback(int request_id, HTTPResponseCallback @callback )

g_EngineFuncs.SetHTTPRequestCallback(request_id, function(int reqid){
	int response_code = 0;
	string response_header;
	string response_body;
	g_EngineFuncs.GetHTTPResponse(reqid, response_code, response_header, response_body);

	if(response_code >= 200)
	{
		g_Game.AlertMessage(at_aiconsole,  "%1", response_header );

		g_EngineFuncs.DestroyHTTPRequest(reqid);
	}
});
```

## Send http request to server

```
//bool g_EngineFuncs.SendHTTPRequest(int request_id)

g_EngineFuncs.SendHTTPRequest(request_id);
```

## Get response header and body of http requset

```

//bool g_EngineFuncs.GetHTTPResponse(int request_id, int& out out_response, string& out out_header, string& out out_body)

int response_code = 0;
string response_header;
string response_body;
g_EngineFuncs.GetHTTPResponse(reqid, response_code, response_header, response_body);


```

## Destroy a http request

```
//bool g_EngineFuncs.DestroyHTTPRequest(int request_id)

g_EngineFuncs.DestroyHTTPRequest(request_id);
```

## Some util functions for hashing and encryption

```
//bool g_EngineFuncs.hmac_sha1(const string& in password, const string& in message, string& out outhash)//outhash is non-readable binary string
//bool g_EngineFuncs.hmac_md5(const string& in password, const string& in message, string& out outhash)//outhash is non-readable binary string
//bool g_EngineFuncs.md5(const string& in data, string& out outhash)//outhash is non-readable binary string
//bool g_EngineFuncs.base64_encode(const string& in hash, string& out outstr)//outstr is human-readable string
```

## Synchronous HTTP Client Example

```
int request_id = g_EngineFuncs.CreateHTTPRequest("http://www.baidu.com/", 
	false,
	ASCURL_METHOD_GET,
	1000,//connect timeout in ms
	5000 //transfer timeout in ms
);

g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Cache-control: no-cache");
g_EngineFuncs.SendHTTPRequest(request_id);

int response_code = 0;
string response_header;
string response_body;
while(g_EngineFuncs.GetHTTPResponse(request_id, response_code, response_header, response_body))
{
	//if response_code == 100, keeps looping
	if(response_code >= 200)
		break;
}
g_EngineFuncs.DestroyHTTPRequest(request_id);
```

## Asynchronous HTTP Client Example

```
int request_id = g_EngineFuncs.CreateHTTPRequest("http://www.baidu.com/", 
	true,
	ASCURL_METHOD_GET,
	1000,//connect timeout in ms
	5000 //transfer timeout in ms
);

g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Cache-control: no-cache");
g_EngineFuncs.SetHTTPRequestCallback(request_id, function(int reqid){
	int response_code = 0;
	string response_header;
	string response_body;
	g_EngineFuncs.GetHTTPResponse(reqid, response_code, response_header, response_body);

	if(response_code >= 200)
	{
		g_Game.AlertMessage(at_aiconsole,  "%1", response_header );
		g_Game.AlertMessage(at_aiconsole,  "%1", response_body );

		g_EngineFuncs.DestroyHTTPRequest(reqid);
	}
});
g_EngineFuncs.SendHTTPRequest(request_id);
```

## Synchronous AliyunOSS File Uploader Example

```
int get_weekday(int day, int month, int year) {
	int a, y, m, R;
	a = ( 14 - month ) / 12;
	y = year - a;
	m = month + 12 * a - 2;
	R = 7000 + ( day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12 );
	return R % 7;
}

string UnixTimeToGMT(time_t t)
{
	const int MILLISECONDS_IN_SECOND = 1000;
	const int MILLISECONDS_IN_MINUTE = 60000;
	const int MILLISECONDS_IN_HALF_HOUR = 1800000;
	const int MILLISECONDS_IN_HOUR = 3600000;
	const int MILLISECONDS_IN_DAY = 86400000;
	const int SECONDS_IN_MINUTE = 60;
	const int SECONDS_IN_HALF_HOUR = 1800;
	const int SECONDS_IN_HOUR = 3600;
	const int SECONDS_IN_DAY = 86400;
	const int SECONDS_IN_YEAR = 31536000;
	const int SECONDS_IN_LEAP_YEAR = 31622400;
	const int AVERAGE_SECONDS_IN_YEAR = 31557600;
	const int SECONDS_IN_4_YEAR = 126230400;
	const int MINUTES_IN_HOUR = 60;
	const int MINUTES_IN_DAY = 1440;
	const int MINUTES_IN_WEEK = 10080;
	const int MINUTES_IN_MONTH = 40320;
	const int HOURS_IN_DAY = 24;
	const int MONTHS_IN_YEAR = 12;
	const int DAYS_IN_WEEK = 7;
	const int DAYS_IN_LEAP_YEAR = 366;
	const int DAYS_IN_YEAR = 365;
	const int DAYS_IN_4_YEAR = 1461;
	const int FIRST_YEAR_UNIX = 1970;
	const int MAX_DAY_MONTH = 31;
	const int OADATE_UNIX_EPOCH = 25569;
	const int _TBIAS_DAYS = 25567;
	const int _TBIAS_YEAR = 1900;

	time_t _secs = t;
	int _mon, _year;
	int _days;
	int i;

	_days = _TBIAS_DAYS;

	_days += _secs / SECONDS_IN_DAY; _secs = _secs % SECONDS_IN_DAY;
	int hour = _secs / SECONDS_IN_HOUR; _secs %= SECONDS_IN_HOUR;
	int minute = _secs / SECONDS_IN_MINUTE;
	int second = _secs % SECONDS_IN_MINUTE;
	int month = 0;
	int day = 0;
	array<int>	lmos = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
	array<int>	mos = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

	_year = _days / DAYS_IN_YEAR;
	int mos_i;
	mos_i = ( ((_year & 3) != 0) || (_year == 0) ) ? mos[0] : lmos[0];
	i = ((_year - 1) / 4) + mos_i + DAYS_IN_YEAR*_year;
	while (_days < i){
		-_year;
		mos_i = ( ((_year & 3) != 0) || (_year == 0) ) ? mos[0] : lmos[0];
		i = ((_year - 1) / 4) + mos_i + DAYS_IN_YEAR*_year;
	}
	_days -= i;
	int year = _year + _TBIAS_YEAR;

	if( ((_year & 3) != 0) || (_year == 0) ) {
		// mos
		for(_mon = MONTHS_IN_YEAR; _days < mos[--_mon];);
		month = _mon + 1;
		day = _days - mos[_mon] + 1;
	} else {
		for(_mon = MONTHS_IN_YEAR; _days < lmos[--_mon];);
		month = _mon + 1;
		day = _days - lmos[_mon] + 1;
	}

	//0~11

	int week = get_weekday(day, month, year);

	array<string> weeknames = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	array<string> monthnames = { "", "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

	string fullhour = (hour < 10) ? ("0"+hour) : hour;
	string fullmin = (minute < 10) ? ("0"+minute) : minute;
	string fullsec = (second < 10) ? ("0"+second) : second;
	return weeknames[week] + ", " + day + " " + monthnames[month] + " " + year + " " + fullhour + ":" + fullmin + ":" + fullsec + " GMT";
}

string szSoundCacheFile = "maps/soundcache/"+g_Engine.mapname+".txt";
size_t nFileSize = 0;
File@ pFile = g_FileSystem.OpenFile(szSoundCacheFile, OpenFile::READ);
if (pFile !is null && pFile.IsOpen())
{
	nFileSize = pFile.GetSize();

	BLOB @pBlob = pFile.ReadBlob();

	string AccessKeyId = "************";
	string AccessKeySecret = "************************";

	string Host = "****bucket****.oss-cn-shanghai.aliyuncs.com";
	string CanonicalizedResource = "/****bucket****/"+szSoundCacheFile;
	string szFileMD5 = "";
	string szFileType = "text/plain";

	string GMTDate = UnixTimeToGMT(UnixTimestamp());
	string CanonicalizedOSSHeaders = "";
	string Signature;
	string SignatureBase64;
	g_EngineFuncs.hmac_sha1(AccessKeySecret,
				"PUT" + "\n"
				+ szFileMD5 + "\n" 
				+ szFileType + "\n" 
				+ GMTDate + "\n" 
				+ CanonicalizedOSSHeaders
				+ CanonicalizedResource, Signature);
	g_EngineFuncs.base64_encode(Signature, SignatureBase64);
	string Authorization = "OSS " + AccessKeyId + ":" + SignatureBase64;

	int request_id = g_EngineFuncs.CreateHTTPRequest("http://"+Host+"/"+szSoundCacheFile, 
		false,//false=sync, true=async
		ASCURL_METHOD_PUT,
		1000,//connect timeout in ms
		5000 //transfer timeout in ms
	);

	g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Host: " + Host);
	g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Cache-control: no-cache");
	g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Content-Type: " + szFileType);
	g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Content-Length: " + nFileSize);
	g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Date: " + GMTDate);
	g_EngineFuncs.AppendHTTPRequestHeader(request_id, "Authorization: " + Authorization);
	g_EngineFuncs.SetHTTPRequestUploadBlob(request_id, pBlob);
	g_EngineFuncs.SendHTTPRequest(request_id);

	int response_code = 0;
	string response_header;
	string response_body;
	while(g_EngineFuncs.GetHTTPResponse(request_id, response_code, response_header, response_body))
	{
		if(response_code >= 200)
			break;
	}
	g_EngineFuncs.DestroyHTTPRequest(request_id);

	pFile.Close();
}
```