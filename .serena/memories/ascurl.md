# ascurl 模块源码分析

## 概述
- ascurl 是 Metamod 插件，提供基于 libcurl 的 HTTP 功能给 AngelScript（CEngineFuncs 扩展），支持同步/异步请求、表单、上传、回调、响应读取，以及 HMAC/MD5/base64 工具函数。入口在 `ascurl/meta_api.cpp`，由 Meta_Attach 初始化并注册脚本 API。

## 职责
- 在 Metamod 生命周期内初始化/销毁 curl 环境与请求对象（`ascurl/ascurl.cpp`、`ascurl/dllapi.cpp`）。
- 将 HTTP API 与加密/编码工具暴露给 AngelScript（`ascurl/server_hook.cpp`）。
- 每帧驱动异步请求并触发 AS 回调（`ascurl/dllapi.cpp`、`ascurl/ascurl.cpp`）。

## 架构
- 插件入口（`ascurl/meta_api.cpp`）
  - `Meta_Query/Meta_Attach/Meta_Detach` 处理 Metamod 接口。
  - `Meta_Attach` 加载 `asext` 插件并导入 API，调用 `ASCURL_Init` 与 `RegisterAngelScriptMethods`。
- 请求管理（`ascurl/ascurl.cpp`）
  - 全局 `m_requests` map + 自增 `m_request_index`。
  - `CBaseHTTPRequest` 持有 curl easy handle、headers、form、流式缓冲、AS 回调。
  - `CAsyncHTTPRequest` 使用 curl multi；`CSyncHTTPRequest` 直接 `curl_easy_perform`。
- 帧驱动（`ascurl/dllapi.cpp`）
  - `NewStartFrame` 调用 `ASCURL_Frame` 驱动 async。
  - `NewGameShutdown` 调用 `ASCURL_Shutdown` 清理。
- 脚本绑定（`ascurl/server_hook.cpp`）
  - `CASEngineFuncs__*` 包装函数在 AS 与 C++ 间做类型转换。
  - `RegisterAngelScriptMethods` 注册 CEngineFuncs 方法、回调类型、脚本宏、目录访问权限。

## 核心实现与工作流
- 初始化：
  1) `Meta_Attach` 加载 `asext` 并导入 API；
  2) `ASCURL_Init` 通过 `gpMetaUtilFuncs->pfnGetModuleHandle(LIBCURL_DLL_NAME)` 找到 libcurl，动态解析 `curl_*` 指针，创建 `curl_multi`。
- 发送请求：
  1) AS 调用 `CreateHTTPRequest` -> 构造 `CAsyncHTTPRequest` 或 `CSyncHTTPRequest`（设置 URL/Method/超时/SSL/回调写入流）；
  2) 可设置 post fields/headers/form/upload/callback；
  3) `SendHTTPRequest`：为 headers/form 生成 curl_slist/curl_form，async 添加到 multi；sync 直接 `curl_easy_perform`。
- 异步轮询：
  - `ASCURL_Frame` 每帧执行 `curl_multi_perform` + `curl_multi_info_read`；
  - 取 `CURLINFO_PRIVATE` 找到 request，调用 `CallCallback`；
  - 若请求在 AS 回调中被销毁则标记，回调结束后统一 `DestroyHTTPRequest`。
- 获取响应：
  - `GetHTTPResponse` 读取 `curl_easy_getinfo` 的 http code，并从 stringstream 取 header/body。
- 关闭：
  - `ASCURL_Shutdown` 删除所有请求并清理 multi handle。

## 依赖
- libcurl 动态链接（`ascurl/signatures.h`）：Windows `libcurl.dll`，Linux `libcurl.so.4`。
- AngelScript 扩展 `asext`（`asext_api.h`）提供 AS 绑定与回调支持。
- 加密：
  - Windows: BCrypt (`bcrypt.lib`)。
  - Linux: OpenSSL `libcrypto.so.1.1` 动态加载。
- Metamod SDK：`meta_api.h`, `dllapi.h`, `engine_api.cpp` 等。

## 注意事项
- `ASCURL_Init` 仅 `GetModuleHandle` 查找 libcurl，未主动 `LoadLibrary`，依赖 libcurl 已被加载。
- 默认关闭 SSL 校验（`CURLOPT_SSL_VERIFYPEER/VERIFYHOST = false`）。
- `ASCURL_DestroyHTTPRequest` 在 AS 回调中仅标记，回调结束后再释放，避免回调期间释放对象。
- `GetResponse` 对 stringstream 仅 `clear()` 状态，未清空 buffer；重复读取可能返回相同内容。
- `read_stream_callback` 固定返回 `size*nmemb`，未使用实际读取字节数，异常短读场景需注意。