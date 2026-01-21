# 添加私有函数的完整工作流程

本文档记录了如何在插件中添加新的私有函数，以 server.dll/server.so 中导入的函数 RegisterSCScriptColor24 为例。

## 工作流程概览

1. 使用 IDA Pro 分析函数签名
2. 定义函数类型和调用约定
3. 添加函数签名（Windows/Linux）
4. 声明和定义全局函数指针
5. 在 Meta_Attach 中填充函数指针
6. 在业务代码中调用函数

---

## 步骤 1: 使用 IDA Pro 分析函数

### 1.1 查找函数
```bash
# 使用 ida-pro-mcp 查找函数
mcp__ida-pro-mcp__lookup_funcs("RegisterSCScriptColor24")
```

### 1.2 获取反编译代码
```bash
mcp__ida-pro-mcp__decompile("0x函数地址")
```

### 1.3 获取反汇编指令
```bash
mcp__ida-pro-mcp__disasm("0x函数地址", max_instructions=30)
```

### 1.4 获取字节码创建签名
```bash
mcp__ida-pro-mcp__get_bytes({"addr": "0x函数地址", "size": 50})
```

### 1.5 创建好的签名应当再去IDA里搜索一次，以确保该字节码搜索到的只有我们需要的目标函数，没有搜索到其他的预期之外的函数。

---

## 步骤 2: 定义函数类型和调用约定

### 2.1 确定调用约定

**常见调用约定**：
- `SC_SERVER_DECL` - fastcall (Windows) / cdecl (Linux)，用于类成员函数
- `SC_SERVER_CDECL` - cdecl (Windows) / cdecl (Linux)，用于普通 C 函数
- 无修饰符 - 默认 cdecl

### 2.2 如需添加新的调用约定宏 (可选)

编辑 `asext/include/asext_api.h`：

```cpp
#ifdef _WIN32
#define SC_SERVER_CDECL __cdecl
#else
#define SC_SERVER_CDECL
#endif
```

### 2.3 在 serverdef.h 中定义函数类型

编辑 `fallguys/serverdef.h`：

```cpp
// 添加注释说明函数签名（从 IDA Pro 获取）
//void __cdecl RegisterSCScriptColor24(CASDocumentation *a1)
typedef void(SC_SERVER_CDECL *fnRegisterSCScriptColor24)(CASDocumentation* pthis);
PRIVATE_FUNCTION_EXTERN(RegisterSCScriptColor24);
```

**注意**：
- typedef 格式：`返回类型(调用约定 *类型名)(参数列表)`
- `PRIVATE_FUNCTION_EXTERN` 会展开为：
  ```cpp
  extern fnRegisterSCScriptColor24 g_pfn_RegisterSCScriptColor24;
  extern fnRegisterSCScriptColor24 g_call_original_RegisterSCScriptColor24;
  ```

---

## 步骤 3: 添加函数签名

编辑 `fallguys/signatures.h`，为 Windows 和 Linux 添加签名。

### 3.1 Windows 签名

```cpp
#ifdef _WIN32

#define RegisterSCScriptColor24_Signature "\x83\xEC\x2C\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x44\x24\x28\x56\x8B\x74\x24\x30\x8B\xCE\x6A\x02\x6A\x03\x68"
```

**签名选择原则**：
- 选择函数开头 20-30 字节
- 包含函数序言（sub esp, push 寄存器等）
- 包含特征性的参数值或常量
- 使用 `\x2A` 作为通配符替换地址/偏移量

### 3.2 Linux 签名和符号

```cpp
#else

#define RegisterSCScriptColor24_Signature "\x55\x57\x56\x53\x83\xEC\x5C\xE8\x2A\x2A\x2A\x2A\x81\xC3\x2A\x2A\x2A\x2A\x8B\x74\x24\x70\xC7\x44\x24\x10\x02\x00\x00\x00\xC7\x44\x24\x0C\x03"
#define RegisterSCScriptColor24_Symbol "_Z23RegisterSCScriptColor24P16CASDocumentation"
```

**Linux 特点**：
- 需要同时提供签名和符号名
- 签名用于 Sven Co-op 5.16+（使用 SCServerDLL003）
- 符号用于 Sven Co-op 5.15（使用符号表）
- PIC (位置无关代码) 特征：`call __x86.get_pc_thunk`, `add ebx, offset`

---

## 步骤 4: 定义全局函数指针

编辑 `fallguys/server_hook.cpp`（或其他适当的 .cpp 文件）：

```cpp
PRIVATE_FUNCTION_DEFINE(CPlayerMove_PlayStepSound);
PRIVATE_FUNCTION_DEFINE(PM_PlaySoundFX_SERVER);
PRIVATE_FUNCTION_DEFINE(RegisterSCScriptColor24);  // 新增
```

**注意**：
- 必须在某个 `.cpp` 文件中定义，否则会出现链接错误
- 应当在已有的`PRIVATE_FUNCTION_DEFINE`定义下面追加
- `PRIVATE_FUNCTION_DEFINE` 宏会被自动展开为：
  ```cpp
  fnRegisterSCScriptColor24 g_pfn_RegisterSCScriptColor24;
  fnRegisterSCScriptColor24 g_call_original_RegisterSCScriptColor24;
  ```

---

## 步骤 5: 在 Meta_Attach 中填充函数指针

编辑 `fallguys/meta_api.cpp`，在 `Meta_Attach` 函数中添加填充代码。

### 5.1 Windows 分支

```cpp
#ifdef _WIN32

FILL_FROM_SIGNATURED_CALLER_FROM_END(server, PM_PlaySoundFX_SERVER, -1);
FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CPlayerMove_PlayStepSound, -1);

FILL_FROM_SIGNATURE(server, RegisterSCScriptColor24);  // 新增
```

### 5.2 Linux Sven Co-op 5.16 分支

```cpp
//Sven Co-op 5.16 rc1 and rc2 (10152 and 10182)
if (CreateInterface("SCServerDLL003", nullptr) != nullptr)
{
    LOG_MESSAGE(PLID, "SCServerDLL003 found! Using signatures for Sven Co-op 5.16");

    FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CPlayerMove_PlayStepSound, -1);
    FILL_FROM_SIGNATURED_CALLER_FROM_END(server, PM_PlaySoundFX_SERVER, -1);

    FILL_FROM_SIGNATURE(server, RegisterSCScriptColor24);  // 新增
}
```

### 5.3 Linux Sven Co-op 5.15 分支

```cpp
else
{
    LOG_MESSAGE(PLID, "SCServerDLL003 not found! Using symbols for Sven Co-op 5.15");

    FILL_FROM_SYMBOL(server, CPlayerMove_PlayStepSound);
    FILL_FROM_SYMBOL(server, PM_PlaySoundFX_SERVER);

    FILL_FROM_SYMBOL(server, RegisterSCScriptColor24);  // 新增
}
```

### 5.4 填充宏说明

**常用宏**：
- `FILL_FROM_SIGNATURE(模块, 函数名)` - 从签名扫描填充
- `FILL_FROM_SYMBOL(模块, 函数名)` - 从符号表填充
- `FILL_FROM_SIGNATURED_CALLER_FROM_END(模块, 函数名, 偏移)` - 从调用者签名填充

**模块选择**：
- `server` - server.dll/server.so
- `engine` - hw.dll/hw.so

---

## 步骤 6: 在业务代码中调用函数

### 6.1 使用前检查函数指针

```cpp
if (g_pfn_RegisterSCScriptColor24)
{
    g_pfn_RegisterSCScriptColor24(pASDoc);
}
```

**重要**：
- 始终检查函数指针是否非空
- 如果签名扫描失败，函数指针将为 NULL
- 不检查会导致崩溃

### 6.2 调用示例

在 `fallguys/server_hook.cpp` 的 `RegisterAngelScriptMethods` 中：

```cpp
ASEXT_RegisterDocInitCallback([](CASDocumentation *pASDoc) {

    /* Register color24 type from server.dll */
    if (g_pfn_RegisterSCScriptColor24)
    {
        g_pfn_RegisterSCScriptColor24(pASDoc);
    }

    /* 其他注册代码... */
});
```

---

## 常见问题和解决方案

### Q1: 编译错误 - 调用约定不支持

**错误信息**：
```
error: expected ')' before '*' token
typedef void(__cdecl* fnXXX)(...);
```

**解决方案**：
使用跨平台宏而不是直接使用 `__cdecl`、`__fastcall` 等：
- Windows 独有的调用约定 → 创建新宏（如 `SC_SERVER_CDECL`）
- 在 `asext/include/asext_api.h` 中定义宏（Windows 为实际约定，Linux 为空）

### Q2: 链接错误 - 未定义的引用

**错误信息**：
```
error LNK2019: unresolved external symbol "void (__cdecl* g_pfn_XXX)"
```

**解决方案**：
在某个 `.cpp` 文件中添加 `PRIVATE_FUNCTION_DEFINE(XXX);`

### Q3: 运行时崩溃 - 函数指针为 NULL

**原因**：
- 签名不匹配（游戏版本不同）
- 符号名称错误
- 函数在当前版本中不存在

**解决方案**：
- 始终检查函数指针：`if (g_pfn_XXX)`
- 检查日志中是否有 "XXX not found" 消息
- 使用 IDA Pro 验证签名

### Q4: 签名扫描失败

**调试步骤**：
1. 检查日志输出，确认模块加载成功
2. 验证签名字节是否正确（从 IDA Pro 重新获取）
3. 尝试缩短或延长签名
4. 对于 Linux，尝试使用符号名称

---

## 检查清单

添加新私有函数时，请确保完成以下步骤：

- [ ] 步骤 1: 使用 IDA Pro 分析函数（反编译、反汇编、获取字节码）
- [ ] 步骤 2: 在 `asext/include/asext_api.h` 添加调用约定宏（如需要）
- [ ] 步骤 3: 在 `fallguys/serverdef.h` 定义函数类型和 `PRIVATE_FUNCTION_EXTERN`
- [ ] 步骤 4: 在 `fallguys/signatures.h` 添加 Windows 签名
- [ ] 步骤 5: 在 `fallguys/signatures.h` 添加 Linux 签名和符号
- [ ] 步骤 6: 在 `fallguys/server_hook.cpp`（或其他 .cpp）添加 `PRIVATE_FUNCTION_DEFINE`
- [ ] 步骤 7: 在 `fallguys/meta_api.cpp` 的 Windows 分支添加 `FILL_FROM_SIGNATURE`
- [ ] 步骤 8: 在 `fallguys/meta_api.cpp` 的 Linux 5.16 分支添加 `FILL_FROM_SIGNATURE`
- [ ] 步骤 9: 在 `fallguys/meta_api.cpp` 的 Linux 5.15 分支添加 `FILL_FROM_SYMBOL`
- [ ] 步骤 10: 在业务代码中调用，包含空指针检查
- [ ] 步骤 11: 编译测试（Windows 和 Linux）
- [ ] 步骤 12: 运行测试，检查日志确认函数找到

---

## 相关文件

- `asext/include/asext_api.h` - 调用约定宏定义
- `fallguys/serverdef.h` - 函数类型定义和声明
- `fallguys/signatures.h` - 函数签名和符号
- `fallguys/server_hook.cpp` - 函数指针定义
- `fallguys/meta_api.cpp` - 函数指针填充
- `metamod/signatures_template.h` - 宏定义模板

---

## 示例代码参考

完整的 RegisterSCScriptColor24 实现可作为参考模板，包含了所有必要的步骤和最佳实践。
