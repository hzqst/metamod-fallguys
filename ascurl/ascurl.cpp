#include <extdll.h>			// always

#include <meta_api.h>		// of course

#include <curl.h>		// of course

#undef read
#undef write

#include <sstream>		// of course
#include <map>		// of course
#include <vector>		// of course

#include "enginedef.h"
#include "serverdef.h"
#include "ascurl.h"

//Missing symbol for linux
mBOOL dlclose_handle_invalid;

#define LIBCURL_FUNCTION_DEFINE(name) static decltype(name) * g_pfn_##name

LIBCURL_FUNCTION_DEFINE(curl_easy_init);
LIBCURL_FUNCTION_DEFINE(curl_easy_getinfo);
LIBCURL_FUNCTION_DEFINE(curl_easy_setopt);
LIBCURL_FUNCTION_DEFINE(curl_easy_cleanup);
LIBCURL_FUNCTION_DEFINE(curl_easy_perform);
LIBCURL_FUNCTION_DEFINE(curl_slist_append);
LIBCURL_FUNCTION_DEFINE(curl_slist_free_all);
LIBCURL_FUNCTION_DEFINE(curl_formadd);
LIBCURL_FUNCTION_DEFINE(curl_formfree);
LIBCURL_FUNCTION_DEFINE(curl_multi_init);
LIBCURL_FUNCTION_DEFINE(curl_multi_perform);
LIBCURL_FUNCTION_DEFINE(curl_multi_info_read);
LIBCURL_FUNCTION_DEFINE(curl_multi_add_handle);
LIBCURL_FUNCTION_DEFINE(curl_multi_remove_handle);
LIBCURL_FUNCTION_DEFINE(curl_multi_cleanup);

static CURLM *g_multi_handle = NULL;
static int g_running_handles = 0;

static size_t read_stream_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	auto stream = (std::stringstream *)userdata;

	stream->read(ptr, size * nmemb);

	return size * nmemb;
}

static size_t write_stream_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	auto stream = (std::stringstream *)userdata;

	stream->write(ptr, size * nmemb);

	return size * nmemb;
}

class CBaseHTTPRequest
{
public:
	CBaseHTTPRequest(int index, const char *url, int method, int conn_timeout_ms, int timeout_ms)
	{
		m_mark_as_dead = false;
		m_in_as_context = false;
		m_callback = NULL;
		m_form_post = NULL;
		m_header_list = NULL;
		m_request_index = index;
		m_easy_handle = g_pfn_curl_easy_init();
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_PRIVATE, this);
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_URL, url);

		if(method == ASCURL_METHOD_POST)
			g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_POST, 1);
		else if (method == ASCURL_METHOD_PUT)
			g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_PUT, 2);

		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_SSL_VERIFYPEER, false);
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_SSL_VERIFYHOST, false);

		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_HEADERFUNCTION, write_stream_callback);
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_WRITEFUNCTION, write_stream_callback);
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_HEADERDATA, &m_headerstream);
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_WRITEDATA, &m_writestream);
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_CONNECTTIMEOUT_MS, conn_timeout_ms);
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_TIMEOUT_MS, timeout_ms);
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_NOSIGNAL, 1);
	}
	virtual ~CBaseHTTPRequest()
	{
		if (m_form_post)
		{
			g_pfn_curl_formfree(m_form_post);
		}

		if (m_header_list)
		{
			g_pfn_curl_slist_free_all(m_header_list);
		}

		if (m_easy_handle)
		{
			g_pfn_curl_easy_cleanup(m_easy_handle);
		}

		if (m_callback)
		{
			ASEXT_DereferenceCASFunction(m_callback);
		}
	}
	virtual bool IsAsync() const = 0;
	virtual void GetResponse(int &response, std::string &header, std::string &body) = 0;
	virtual int GetRequestIndex()
	{
		return m_request_index;
	}
	virtual void Destroy()
	{
		delete this;
	}
	virtual void SetPostField(const char *postfield)
	{
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_COPYPOSTFIELDS, postfield);
	}
	virtual void SetPostField(const char *postfield, size_t size_of_postfield)
	{
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_POSTFIELDSIZE, size_of_postfield);
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_COPYPOSTFIELDS, postfield);
	}
	virtual void AppendHeader(const char *headerfield)
	{
		m_headers.emplace_back(headerfield);
	}
	virtual void SetCallback(aslScriptFunction *aslfn)
	{
		if (!m_callback)
		{
			m_callback = ASEXT_CreateCASFunction(aslfn, ASEXT_GetServerManager()->curModule, 1);
		}
	}
	virtual void CallCallback()
	{
		if (m_callback && ASEXT_CallCASBaseCallable && (*ASEXT_CallCASBaseCallable))
		{
			m_in_as_context = true;

			(*ASEXT_CallCASBaseCallable)(m_callback, 0, m_request_index);

			m_in_as_context = false;
		}
	}
	virtual void UploadBlob(const void *blob, size_t size_of_blob)
	{
		m_readstream.write((const char *)blob, size_of_blob);

		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_UPLOAD, 1L);
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_READFUNCTION, read_stream_callback);
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_READDATA, &m_readstream);
		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_INFILESIZE_LARGE, (curl_off_t)size_of_blob);
	}
	virtual void AppendFormString(const char *formname, const char *content)
	{
		g_pfn_curl_formadd(
			&m_form_post,
			&m_form_last,
			CURLFORM_COPYNAME, formname,
			CURLFORM_COPYCONTENTS, content,
			CURLFORM_END);
	}
	virtual void AppendFormBlob(const char *formname, const void *blob, size_t size_of_blob)
	{
		g_pfn_curl_formadd(
			&m_form_post,
			&m_form_last,
			CURLFORM_COPYNAME, formname,
			CURLFORM_COPYCONTENTS, blob,
			CURLFORM_CONTENTLEN, size_of_blob,
			CURLFORM_END);
	}
	virtual void AppendFormStream(const char *formname, const void *blob, size_t size_of_blob)
	{
		m_readstream.write((const char *)blob, size_of_blob);

		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_READFUNCTION, read_stream_callback);

		g_pfn_curl_formadd(
			&m_form_post,
			&m_form_last,
			CURLFORM_COPYNAME, formname,
			CURLFORM_STREAM, &m_readstream,
			CURLFORM_CONTENTLEN, size_of_blob,
			CURLFORM_END);
	}
	virtual void SendRequest()
	{
		if (m_form_post)
		{
			g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_HTTPPOST, m_form_post);
		}

		for (const auto &h : m_headers)
		{
			m_header_list = g_pfn_curl_slist_append(m_header_list, h.c_str());
		}

		g_pfn_curl_easy_setopt(m_easy_handle, CURLOPT_HTTPHEADER, m_header_list);
	}
	virtual void MarkAsDead()
	{
		m_mark_as_dead = true;
	}
	virtual bool IsMarkedAsDead() const
	{
		return m_mark_as_dead;
	}
	virtual bool IsInAngelScriptContext() const
	{
		return m_in_as_context;
	}

	CURL *m_easy_handle;
	struct curl_httppost *m_form_post;
	struct curl_httppost *m_form_last;
	struct curl_slist *m_header_list;
	std::vector<std::string> m_headers;
	std::stringstream m_headerstream;
	std::stringstream m_writestream;
	std::stringstream m_readstream;
	CASFunction *m_callback;
	int m_request_index;
	bool m_mark_as_dead;
	bool m_in_as_context;
};

class CAsyncHTTPRequest : public CBaseHTTPRequest
{
public:
	CAsyncHTTPRequest(int index, const char *url, int method, int conn_timeout_ms, int timeout_ms) : CBaseHTTPRequest(index, url, method, conn_timeout_ms, timeout_ms)
	{
		
	}

	~CAsyncHTTPRequest()
	{
		if (m_easy_handle)
		{
			g_pfn_curl_multi_remove_handle(g_multi_handle, m_easy_handle);
		}
	}

	bool IsAsync() const
	{
		return true;
	}

	void SendRequest()
	{
		CBaseHTTPRequest::SendRequest();

		g_running_handles++;
		g_pfn_curl_multi_add_handle(g_multi_handle, m_easy_handle);
	}

	void GetResponse(int &response, std::string &header, std::string &body)
	{
		g_pfn_curl_easy_getinfo(m_easy_handle, CURLINFO_RESPONSE_CODE, &response);
		
		header = m_headerstream.str();
		body = m_writestream.str();

		m_headerstream.clear();
		m_writestream.clear();
	}
};

class CSyncHTTPRequest : public CBaseHTTPRequest
{
public:
	CSyncHTTPRequest(int index, const char *url, int method, int conn_timeout_ms, int timeout_ms) : CBaseHTTPRequest(index, url, method, conn_timeout_ms, timeout_ms)
	{
		
	}

	~CSyncHTTPRequest()
	{
		
	}

	bool IsAsync() const
	{
		return false;
	}

	void SendRequest()
	{
		CBaseHTTPRequest::SendRequest();

		g_pfn_curl_easy_perform(m_easy_handle);
	}

	void GetResponse(int &response, std::string &header, std::string &body)
	{
		g_pfn_curl_easy_getinfo(m_easy_handle, CURLINFO_RESPONSE_CODE, &response);

		header = m_headerstream.str();
		body = m_writestream.str();

		m_headerstream.clear();
		m_writestream.clear();
	}
};

static std::map<int, CBaseHTTPRequest *> m_requests;
static int m_request_index = 1;

bool ASCURL_Init()
{
	auto libcurl = DLOPEN(LIBCURL_DLL_NAME);
	if (!libcurl)
	{
		LOG_ERROR(PLID, "failed to find libcurl dll");
		return false;
	}
#define CURL_DLSYM(name) g_pfn_##name = (decltype(g_pfn_##name))DLSYM(libcurl, #name);\
	if (!g_pfn_##name)\
	{\
		LOG_ERROR(PLID, "failed to find " #name);\
		return false;\
	}

	CURL_DLSYM(curl_easy_init);
	CURL_DLSYM(curl_easy_getinfo);
	CURL_DLSYM(curl_easy_setopt);
	CURL_DLSYM(curl_easy_cleanup);
	CURL_DLSYM(curl_easy_perform);
	CURL_DLSYM(curl_slist_append);
	CURL_DLSYM(curl_slist_free_all);
	CURL_DLSYM(curl_formadd);
	CURL_DLSYM(curl_formfree);
	CURL_DLSYM(curl_multi_init);
	CURL_DLSYM(curl_multi_perform);
	CURL_DLSYM(curl_multi_info_read);
	CURL_DLSYM(curl_multi_add_handle);
	CURL_DLSYM(curl_multi_remove_handle);
	CURL_DLSYM(curl_multi_cleanup);

	g_multi_handle = g_pfn_curl_multi_init();
	if (!g_multi_handle)
	{
		LOG_ERROR(PLID, "curl_multi_init failed !");
		return false;
	}

	return true;
}

void ASCURL_Shutdown()
{
	for (auto itor : m_requests)
	{
		delete itor.second;
	}
	m_requests.clear();

	if (g_multi_handle)
	{
		g_pfn_curl_multi_cleanup(g_multi_handle);
		g_multi_handle = NULL;
	}
}

void ASCURL_Frame()
{
	auto code = g_pfn_curl_multi_perform(g_multi_handle, &g_running_handles);

	if (code == CURLM_OK || code == CURLM_CALL_MULTI_PERFORM)
	{
		int msg_queued = 0;
		CURLMsg * msg = NULL;
		do
		{
			msg = g_pfn_curl_multi_info_read(g_multi_handle, &msg_queued);
			if (msg && (msg->msg == CURLMSG_DONE))
			{
				auto eh = msg->easy_handle;

				CBaseHTTPRequest *request = NULL;

				g_pfn_curl_easy_getinfo(eh, CURLINFO_PRIVATE, &request);

				if (request)
				{
					request->CallCallback();

					if(request->IsMarkedAsDead())
					{
						ASCURL_DestroyHTTPRequest(request->GetRequestIndex());
					}
				}
			}
		} while (msg);
	}
}

int ASCURL_CreateHTTPRequest(const char *url, bool async, int method, int conn_timeout_ms, int timeout_ms)
{
	int request_index = m_request_index;

	m_request_index++;

	CBaseHTTPRequest *request = NULL;
	
	if(async)
		request = new CAsyncHTTPRequest(request_index, url, method, conn_timeout_ms, timeout_ms);
	else
		request = new CSyncHTTPRequest(request_index, url, method, conn_timeout_ms, timeout_ms);

	m_requests[request_index] = request;

	return request_index;
}

bool ASCURL_SetHTTPRequestPostField(int request_id, const char *postfield)
{
	auto itor = m_requests.find(request_id);
	if (itor != m_requests.end())
	{
		itor->second->SetPostField(postfield);

		return true;
	}

	return false;
}

bool ASCURL_SetHTTPRequestPostFieldEx(int request_id, const char *postfield, size_t size_of_postfield)
{
	auto itor = m_requests.find(request_id);
	if (itor != m_requests.end())
	{
		itor->second->SetPostField(postfield, size_of_postfield);

		return true;
	}

	return false;
}

bool ASCURL_AppendHTTPRequestHeader(int request_id, const char *headerfield)
{
	auto itor = m_requests.find(request_id);
	if (itor != m_requests.end())
	{
		itor->second->AppendHeader(headerfield);

		return true;
	}

	return false;
}

bool ASCURL_SetHTTPRequestUploadBlob(int request_id, const void *blob, size_t size_of_blob)
{
	auto itor = m_requests.find(request_id);
	if (itor != m_requests.end())
	{
		itor->second->UploadBlob(blob, size_of_blob);

		return true;
	}

	return false;
}

bool ASCURL_SetHTTPRequestCallback(int request_id, aslScriptFunction *aslfn)
{
	auto itor = m_requests.find(request_id);
	if (itor != m_requests.end())
	{
		itor->second->SetCallback(aslfn);

		return true;
	}

	return false;
}

bool ASCURL_AppendHTTPRequestFormString(int request_id, const char *form, const char *content)
{
	auto itor = m_requests.find(request_id);
	if (itor != m_requests.end())
	{
		itor->second->AppendFormString(form, content);

		return true;
	}

	return false;
}

bool ASCURL_AppendHTTPRequestFormBlob(int request_id, const char *form, const void *blob, size_t size_of_blob)
{
	auto itor = m_requests.find(request_id);
	if (itor != m_requests.end())
	{
		itor->second->AppendFormBlob(form, blob, size_of_blob);

		return true;
	}

	return false;
}

bool ASCURL_SendHTTPRequest(int request_id)
{
	auto itor = m_requests.find(request_id);
	if (itor != m_requests.end())
	{
		itor->second->SendRequest();

		return true;
	}

	return false;
}

bool ASCURL_GetHTTPResponse(int request_id, int &response_code, std::string& out_header, std::string& out_body)
{
	auto itor = m_requests.find(request_id);
	if (itor != m_requests.end())
	{
		itor->second->GetResponse(response_code, out_header, out_body);

		return true;
	}

	return false;
}

bool ASCURL_DestroyHTTPRequest(int request_id)
{
	auto itor = m_requests.find(request_id);
	if (itor != m_requests.end())
	{
		auto req = itor->second;

		if (req->IsInAngelScriptContext())
		{
			req->MarkAsDead();
		}
		else
		{
			req->Destroy();
			m_requests.erase(itor);
		}

		return true;
	}

	return false;
}

void RegisterAngelScriptHooks()
{

}

#if _WIN32

#include <windows.h>
#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

bool ASCURL_hmac_sha1(const std::string& pwd, const std::string& msg, std::string& out)
{
	bool bSuccess = false;
	ULONG hashSize = 0, hashSizeSize = 0;
	auto hmacSecret = pwd.data();
	auto hmacSecretSize = pwd.size();
	auto data = msg.data();
	auto dataSize = msg.size();

	byte hashBuf[64];

	BCRYPT_ALG_HANDLE context;
	if (BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&context, BCRYPT_SHA1_ALGORITHM, NULL, BCRYPT_ALG_HANDLE_HMAC_FLAG)))
	{
		BCRYPT_HASH_HANDLE hash;
		if (BCRYPT_SUCCESS(BCryptGetProperty(context, BCRYPT_HASH_LENGTH, (PUCHAR)&hashSize, sizeof(hashSize), &hashSizeSize, 0)))
		{
			if (BCRYPT_SUCCESS(BCryptCreateHash(context, &hash, NULL, 0, (PUCHAR)hmacSecret, hmacSecretSize, 0)))
			{
				if (BCRYPT_SUCCESS(BCryptHashData(hash, (PUCHAR)data, dataSize, 0)))
				{
					if (BCRYPT_SUCCESS(BCryptFinishHash(hash, hashBuf, hashSize, 0)))
					{
						out.resize(hashSize);
						memcpy((void *)out.data(), hashBuf, hashSize);
						bSuccess = true;
					}
				}
				BCryptDestroyHash(hash);
			}
		}
		BCryptCloseAlgorithmProvider(context, 0);
	}
	return bSuccess;
}

bool ASCURL_hmac_md5(const std::string& pwd, const std::string& msg, std::string& out)
{
	bool bSuccess = false;
	ULONG hashSize = 0, hashSizeSize = 0;
	auto hmacSecret = pwd.data();
	auto hmacSecretSize = pwd.size();
	auto data = msg.data();
	auto dataSize = msg.size();

	byte hashBuf[64];

	BCRYPT_ALG_HANDLE context;
	if (BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&context, BCRYPT_MD5_ALGORITHM, NULL, BCRYPT_ALG_HANDLE_HMAC_FLAG)))
	{
		BCRYPT_HASH_HANDLE hash;
		if (BCRYPT_SUCCESS(BCryptGetProperty(context, BCRYPT_HASH_LENGTH, (PUCHAR)&hashSize, sizeof(hashSize), &hashSizeSize, 0)))
		{
			if (BCRYPT_SUCCESS(BCryptCreateHash(context, &hash, NULL, 0, (PUCHAR)hmacSecret, hmacSecretSize, 0)))
			{
				if (BCRYPT_SUCCESS(BCryptHashData(hash, (PUCHAR)data, dataSize, 0)))
				{
					if (BCRYPT_SUCCESS(BCryptFinishHash(hash, hashBuf, hashSize, 0)))
					{
						out.resize(hashSize);
						memcpy((void *)out.data(), hashBuf, hashSize);
						bSuccess = true;
					}
				}
				BCryptDestroyHash(hash);
			}
		}
		BCryptCloseAlgorithmProvider(context, 0);
	}
	return bSuccess;
}

bool ASCURL_md5(const char* Data, int DataByte, std::string &out)
{
	bool bSuccess = false;
	ULONG hashSize = 0, hashSizeSize = 0;
	auto data = Data;
	auto dataSize = DataByte;

	byte hashBuf[64];

	BCRYPT_ALG_HANDLE context;
	if (BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&context, BCRYPT_MD5_ALGORITHM, NULL, 0)))
	{
		BCRYPT_HASH_HANDLE hash;
		if (BCRYPT_SUCCESS(BCryptGetProperty(context, BCRYPT_HASH_LENGTH, (PUCHAR)&hashSize, sizeof(hashSize), &hashSizeSize, 0)))
		{
			if (BCRYPT_SUCCESS(BCryptCreateHash(context, &hash, NULL, 0, NULL, 0, 0)))
			{
				if (BCRYPT_SUCCESS(BCryptHashData(hash, (PUCHAR)data, dataSize, 0)))
				{
					if (BCRYPT_SUCCESS(BCryptFinishHash(hash, hashBuf, hashSize, 0)))
					{
						out.resize(hashSize);
						memcpy((void *)out.data(), hashBuf, hashSize);
						bSuccess = true;
					}
				}
			}
			BCryptDestroyHash(hash);
		}
		BCryptCloseAlgorithmProvider(context, 0);
	}
	return bSuccess;
}

#else

#include <openssl/hmac.h>

bool ASCURL_hmac_sha1(const std::string& pwd, const std::string& msg, std::string& out)
{
	bool bSuccess = false;

	auto libcryptoHandle = DLOPEN("libcrypto.so.1.1");

	if (libcryptoHandle)
	{
		auto pfnEVP_sha1 = (decltype(EVP_sha1) *)DLSYM(libcryptoHandle, "EVP_sha1");
		auto pfnHMAC = (decltype(HMAC) *)DLSYM(libcryptoHandle, "HMAC");

		if (pfnEVP_sha1 && pfnHMAC)
		{
			auto key = pwd.data();
			auto keySize = pwd.size();

			auto data = msg.data();
			auto dataSize = msg.size();

			auto digest = pfnHMAC(pfnEVP_sha1(), key, keySize, (unsigned char*)data, dataSize, NULL, NULL);

			if (digest)
			{
				out.resize(20);
				memcpy((void *)out.data(), digest, 20);
			}

			bSuccess = true;
		}

		DLCLOSE(libcryptoHandle);
	}

	return bSuccess;
}

bool ASCURL_hmac_md5(const std::string& pwd, const std::string& msg, std::string& out)
{
	bool bSuccess = false;

	auto libcryptoHandle = DLOPEN("libcrypto.so.1.1");

	if (libcryptoHandle)
	{
		auto pfnEEVP_md5 = (decltype(EVP_md5) *)DLSYM(libcryptoHandle, "EVP_md5");
		auto pfnHMAC = (decltype(HMAC) *)DLSYM(libcryptoHandle, "HMAC");

		if (pfnEEVP_md5 && pfnHMAC)
		{
			auto key = pwd.data();
			auto keySize = pwd.size();

			auto data = msg.data();
			auto dataSize = msg.size();

			auto digest = pfnHMAC(pfnEEVP_md5(), key, keySize, (unsigned char*)data, dataSize, NULL, NULL);

			if (digest)
			{
				out.resize(16);
				memcpy((void *)out.data(), digest, 16);
			}

			bSuccess = true;
		}

		DLCLOSE(libcryptoHandle);
	}

	return bSuccess;
}

#include <openssl/md5.h>

bool ASCURL_md5(const char* Data, int DataByte, std::string &out)
{
	bool bSuccess = false;

	auto libcryptoHandle = DLOPEN("libcrypto.so.1.1");

	if (libcryptoHandle)
	{
		auto pfnMD5_Init = (decltype(MD5_Init) *)DLSYM(libcryptoHandle, "MD5_Init");
		auto pfnMD5_Update = (decltype(MD5_Update) *)DLSYM(libcryptoHandle, "MD5_Update");
		auto pfnMD5_Final = (decltype(MD5_Final) *)DLSYM(libcryptoHandle, "MD5_Final");

		if (pfnMD5_Init &&pfnMD5_Update && pfnMD5_Final)
		{
			unsigned char digest[16];
			MD5_CTX md5;
			pfnMD5_Init(&md5);
			pfnMD5_Update(&md5, Data, DataByte);
			pfnMD5_Final(digest, &md5);

			out.resize(16);
			memcpy((void *)out.data(), digest, 16);

			bSuccess = true;
		}
	}

	return bSuccess;
}

#endif
void ASCURL_base64_encode(const char* Data, int DataByte, std::string &strEncode)
{
	const char *EncodeTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned char Tmp[4] = { 0 };
	for (int i = 0; i < (int)(DataByte / 3); i++)
	{
		Tmp[1] = (unsigned char)*Data++;
		Tmp[2] = (unsigned char)*Data++;
		Tmp[3] = (unsigned char)*Data++;
		strEncode += EncodeTable[Tmp[1] >> 2];
		strEncode += EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
		strEncode += EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
		strEncode += EncodeTable[Tmp[3] & 0x3F];
	}
	int Mod = DataByte % 3;
	if (Mod == 1)
	{
		Tmp[1] = (unsigned char)*Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4)];
		strEncode += "==";
	}
	else if (Mod == 2)
	{
		Tmp[1] = (unsigned char)*Data++;
		Tmp[2] = (unsigned char)*Data++;
		strEncode += EncodeTable[(Tmp[1] & 0xFC) >> 2];
		strEncode += EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
		strEncode += EncodeTable[((Tmp[2] & 0x0F) << 2)];
		strEncode += '=';
	}
}