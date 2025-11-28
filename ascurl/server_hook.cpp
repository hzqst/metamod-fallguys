#include <extdll.h>

#include <dllapi.h>
#include <meta_api.h>

#include <cl_entity.h>
#include <entity_state.h>

#include "enginedef.h"
#include "serverdef.h"
#include "ascurl.h"

int SC_SERVER_DECL CASEngineFuncs__CreateHTTPRequest(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, const CString *str, bool async, int method, int conn_timeout_ms, int timeout_ms)
{
	return ASCURL_CreateHTTPRequest(str->c_str(), async, method, conn_timeout_ms, timeout_ms);
}

bool SC_SERVER_DECL CASEngineFuncs__SetHTTPRequestPostField(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int request_id, const CString *str)
{
	return ASCURL_SetHTTPRequestPostField(request_id, str->c_str());
}

bool SC_SERVER_DECL CASEngineFuncs__SetHTTPRequestPostFieldEx(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int request_id, const CString *str, size_t size_of_postfield)
{
	return ASCURL_SetHTTPRequestPostFieldEx(request_id, str->c_str(), size_of_postfield);
}

bool SC_SERVER_DECL CASEngineFuncs__AppendHTTPRequestHeader(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int request_id, const CString *str)
{
	return ASCURL_AppendHTTPRequestHeader(request_id, str->c_str());
}

bool SC_SERVER_DECL CASEngineFuncs__AppendHTTPRequestFormString(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int request_id, const CString *str, const CString *content)
{
	return ASCURL_AppendHTTPRequestFormString(request_id, str->c_str(), content->c_str());
}

bool SC_SERVER_DECL CASEngineFuncs__AppendHTTPRequestFormBlob(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int request_id, const CString *str, const CASBLOB *blob)
{
	return ASCURL_AppendHTTPRequestFormBlob(request_id, str->c_str(), blob->data(), blob->size());
}

bool SC_SERVER_DECL CASEngineFuncs__SetHTTPRequestUploadBlob(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int request_id, const CASBLOB *blob)
{
	return ASCURL_SetHTTPRequestUploadBlob(request_id, blob->data(), blob->size());
}

bool SC_SERVER_DECL CASEngineFuncs__SetHTTPRequestCallback(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int request_id, aslScriptFunction *aslfn)
{
	return ASCURL_SetHTTPRequestCallback(request_id, aslfn);
}

bool SC_SERVER_DECL CASEngineFuncs__SendHTTPRequest(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int request_id)
{
	return ASCURL_SendHTTPRequest(request_id);
}

bool SC_SERVER_DECL CASEngineFuncs__GetHTTPResponse(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int request_id, int *out_response, CString *out_header, CString *out_body)
{
	int response = 0;
	std::string header, body;
	if (ASCURL_GetHTTPResponse(request_id, response, header, body))
	{
		*out_response = response;
		out_header->assign(header.c_str(), header.size());
		out_body->assign(body.c_str(), body.size());

		return true;
	}

	return false;
}

bool SC_SERVER_DECL CASEngineFuncs__DestroyHTTPRequest(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int request_id)
{
	return ASCURL_DestroyHTTPRequest(request_id);
}

bool SC_SERVER_DECL CASEngineFuncs__hmac_sha1(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, CString *password, CString *message, CString *outhash)
{
	std::string pwd(password->c_str(), password->length());
	std::string msg(message->c_str(), message->length());
	std::string out;

	if (ASCURL_hmac_sha1(pwd, msg, out))
	{
		outhash->assign(out.c_str(), out.size());
		return true;
	}

	return false;
}

bool SC_SERVER_DECL CASEngineFuncs__hmac_md5(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, CString *password, CString *message, CString *outhash)
{
	std::string pwd(password->c_str(), password->length());
	std::string msg(message->c_str(), message->length());
	std::string out;

	if (ASCURL_hmac_md5(pwd, msg, out))
	{
		outhash->assign(out.c_str(), out.size());
		return true;
	}

	return false;
}

bool SC_SERVER_DECL CASEngineFuncs__md5(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, CString *data, CString *outhash)
{
	std::string out;

	if (ASCURL_md5(data->c_str(), data->length(), out))
	{
		outhash->assign(out.c_str(), out.size());
		return true;
	}

	return false;
}

bool SC_SERVER_DECL CASEngineFuncs__base64_encode(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, CString *hash, CString *out)
{
	std::string outstr;
	ASCURL_base64_encode(hash->c_str(), hash->length(), outstr);
	out->assign(outstr.c_str(), outstr.size());

	return true;
}

void RegisterAngelScriptMethods()
{
	ASEXT_RegisterScriptBuilderDefineCallback([](CScriptBuilder* pScriptBuilder) {

		ASEXT_CScriptBuilder_DefineWord(pScriptBuilder, "METAMOD_PLUGIN_ASCURL");

	});

	ASEXT_RegisterDirInitCallback([](CASDirectoryList *pASDir) {

		ASEXT_CreateDirectory(pASDir, "maps", ASFlag_Plugin, ASFileAccessControl_Read, true, 0);
		ASEXT_CreateDirectory(pASDir, "maps/soundcache", ASFlag_Plugin, ASFileAccessControl_Read | ASFileAccessControl_Write, true, 0);

		ASEXT_CreateDirectory(pASDir, "models", ASFlag_Plugin, ASFileAccessControl_Read, true, 0);
		ASEXT_CreateDirectory(pASDir, "models/player", ASFlag_Plugin, ASFileAccessControl_Read, true, 0);

		ASEXT_CreateDirectory(pASDir, "sound", ASFlag_Plugin, ASFileAccessControl_Read, true, 0);
		ASEXT_CreateDirectory(pASDir, "sprites", ASFlag_Plugin, ASFileAccessControl_Read, true, 0);

	});
	ASEXT_RegisterDocInitCallback([](CASDocumentation *pASDoc) {
		
		ASEXT_RegisterObjectMethod(pASDoc,
			"Create http request via libcurl, returns request_id of created request", "CEngineFuncs", "int CreateHTTPRequest(const string& in url, bool async, int method, int conn_timeout_ms, int timeout_ms)",
			(void *)CASEngineFuncs__CreateHTTPRequest, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Set post field of http request via libcurl", "CEngineFuncs", "bool SetHTTPRequestPostField(int request_id, const string& in post_fields )",
			(void *)CASEngineFuncs__SetHTTPRequestPostField, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Set post field of http request via libcurl", "CEngineFuncs", "bool SetHTTPRequestPostFieldEx(int request_id, const string& in post_fields, int sizeof_post_fields )",
			(void *)CASEngineFuncs__SetHTTPRequestPostFieldEx, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Append http header to request via libcurl", "CEngineFuncs", "bool AppendHTTPRequestHeader(int request_id, const string& in header )",
			(void *)CASEngineFuncs__AppendHTTPRequestHeader, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Append http header to request via libcurl", "CEngineFuncs", "bool AppendHTTPRequestFormString(int request_id, const string& in form, const string& in content )",
			(void *)CASEngineFuncs__AppendHTTPRequestFormString, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Append http header to request via libcurl", "CEngineFuncs", "bool AppendHTTPRequestFormBlob(int request_id, const string& in form, const BLOB& in blob )",
			(void *)CASEngineFuncs__AppendHTTPRequestFormBlob, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Set upload blob via libcurl", "CEngineFuncs", "bool SetHTTPRequestUploadBlob(int request_id, const BLOB& in blob )",
			(void *)CASEngineFuncs__SetHTTPRequestUploadBlob, 3);

		ASEXT_RegisterFuncDef(pASDoc, "callback for HTTP request via libcurl", "void HTTPResponseCallback(int request_id)");

		ASEXT_RegisterObjectMethod(pASDoc,
			"Set http request callback via libcurl", "CEngineFuncs", "bool SetHTTPRequestCallback(int request_id, HTTPResponseCallback @callback )",
			(void *)CASEngineFuncs__SetHTTPRequestCallback, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Send http request via libcurl", "CEngineFuncs", "bool SendHTTPRequest(int request_id)",
			(void *)CASEngineFuncs__SendHTTPRequest, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Get http response via libcurl", "CEngineFuncs", "bool GetHTTPResponse(int request_id, int& out out_response, string& out out_header, string& out out_body)",
			(void *)CASEngineFuncs__GetHTTPResponse, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Destroy http request that created with CreateHTTPRequest", "CEngineFuncs", "bool DestroyHTTPRequest(int request_id)",
			(void *)CASEngineFuncs__DestroyHTTPRequest, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Calculate hmac_sha1 for input string, return hash binary", "CEngineFuncs", "bool hmac_sha1(const string& in password, const string& in message, string& out outhash)",
			(void *)CASEngineFuncs__hmac_sha1, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Calculate hmac_md5 for input string, return hash binary", "CEngineFuncs", "bool hmac_md5(const string& in password, const string& in message, string& out outhash)",
			(void *)CASEngineFuncs__hmac_md5, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Calculate md5 for input string, return hash binary", "CEngineFuncs", "bool md5(const string& in data, string& out outhash)",
			(void *)CASEngineFuncs__md5, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Calculate base64 for input string (binary)", "CEngineFuncs", "bool base64_encode(const string& in hash, string& out outstr)",
			(void *)CASEngineFuncs__base64_encode, 3);

	});
}