#pragma once

#include <string>

void RegisterAngelScriptMethods();
void RegisterAngelScriptHooks();

const int ASCURL_METHOD_GET = 0;
const int ASCURL_METHOD_POST = 1;
const int ASCURL_METHOD_PUT = 2;

bool ASCURL_Init();
void ASCURL_Frame();
void ASCURL_Shutdown();
int ASCURL_CreateHTTPRequest(const char *url, bool async, int method, int conn_timeout_ms, int timeout_ms);
bool ASCURL_SetHTTPRequestPostField(int request_id, const char *postfield);
bool ASCURL_SetHTTPRequestPostFieldEx(int request_id, const char *postfield, size_t size_of_postfield);
bool ASCURL_AppendHTTPRequestHeader(int request_id, const char *header);
bool ASCURL_AppendHTTPRequestFormString(int request_id, const char *form, const char *content);
bool ASCURL_AppendHTTPRequestFormBlob(int request_id, const char *form, const void *blob, size_t size_of_blob);
bool ASCURL_SetHTTPRequestUploadBlob(int request_id, const void *blob, size_t size_of_blob);
bool ASCURL_SetHTTPRequestCallback(int request_id, aslScriptFunction *aslfn);
bool ASCURL_SendHTTPRequest(int request_id);
bool ASCURL_GetHTTPResponse(int request_id, int &response_code, std::string& out_header, std::string& out_body);
bool ASCURL_DestroyHTTPRequest(int request_id);
bool ASCURL_hmac_sha1(const std::string& pwd, const std::string& msg, std::string& out);
bool ASCURL_hmac_md5(const std::string& pwd, const std::string& msg, std::string& out);
bool ASCURL_md5(const char* Data, int DataByte, std::string &out);
void ASCURL_base64_encode(const char* Data, int DataByte, std::string &strEncode);
