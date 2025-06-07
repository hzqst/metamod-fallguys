# CMake 构建说明

本项目现在支持使用 CMake 在 Windows 和 Linux 平台上构建。

## 前置要求

### Windows
- Visual Studio 2017 或更高版本
- CMake 3.10 或更高版本
- 第三方库（capstone、bullet3 等）需要预先编译并放置在 `thirdparty/` 目录下

### Linux
- GCC 支持 32 位编译
- CMake 3.10 或更高版本
- 第三方库需要预先编译并放置在 `thirdparty/install/` 目录下

## 构建步骤

### Windows

1. 使用提供的批处理文件：
```bash
# Debug 构建
build.bat Debug

# Release 构建
build.bat Release
```

2. 或手动使用 CMake：
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 15 2017" -A Win32
cmake --build . --config Release
```

### Linux

1. 使用提供的脚本：
```bash
# 添加执行权限
chmod +x build.sh

# Debug 构建
./build.sh Debug

# Release 构建
./build.sh Release
```

2. 或手动使用 CMake：
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## 输出文件

编译后的文件将输出到 `output/<平台>/<配置>/` 目录下：
- Windows: `output/Win32/Release/` 或 `output/Win32/Debug/`
- Linux: `output/i686/Release/` 或 `output/i686/Debug/`

## 项目依赖关系

- `fallguys`、`ascurl` 和 `asqcvar` 依赖于 `asext`
- `metamod` 是独立的
- `asusermsg` 是独立的

## 第三方库配置

确保以下第三方库正确放置：

### Windows
- Capstone: `thirdparty/capstone/`
- Bullet3: `thirdparty/bullet3/`
- FMOD: `thirdparty/fmod/`
- cURL: `thirdparty/curl/`
- OpenSSL: `thirdparty/openssl/`

### Linux
- 所有库都应该在 `thirdparty/install/<库名>/linux-i386/` 下
- 包含 `include/` 和 `lib/` 子目录

## 注意事项

1. Linux 构建默认为 32 位（-m32）
2. Windows 使用静态运行时库（/MT 或 /MTd）
3. 所有项目输出都没有前缀（例如输出 `metamod.dll` 而不是 `libmetamod.dll`）
4. metamod 项目在 Windows 下需要 `metamod.def` 文件
5. Linux 下某些项目可能需要链接器脚本（如 `i386pe.merge`） 