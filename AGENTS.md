# AGENTS.md

本文件用于指导在本仓库内采用渐进式披露的方式进行 Agent Coding

## Serena memories（保持上下文精简）

1. 优先使用 `list_memories` 查看当前项目已有的 memories（不要默认读全）。
2. 仅在需要时，用 `read_memory` 精确读取某个 memory 文件（按需加载）。
3. 如 memory 信息不足/过期，再回退读取仓库文件或用 ContextEngine/符号/搜索能力做定点定位，并使用 `write_memory`、`edit_memory` 或 `delete_memory` 维护记忆内容。

## 本仓库的高层信息（优先读对应 memories）

以下内容已在 onboarding 时整理到 Serena memories，本文件不再重复展开：

- 项目目的/背景：`project_overview`
- 技术栈：`tech_stack`
- 目录结构与模块划分：`project_structure`
- 常用开发命令：`suggested_commands`
- 代码风格与约定：`code_style_conventions`
- 开发规范与注意事项：`development_guidelines`
- 完成任务后的建议清单：`task_completion_checklist`
- 模块专题：`asext`

## 当 memories 不足时的“源文件”入口（按需查询和读取）

- 项目说明/插件文档：`README.md`、`README.txt`、`README_FALLGUYS.md`、`README_ASEXT.md`、`README_ASCURL.md`、`README_ASQCVAR.md`、`README_ASUSERMSG.md`、`README_CMAKE.md`
- 构建与配置入口：`CMakeLists.txt`、`Makefile`、`Config.mak`、`metamod.sln`、`scripts/`
- 主要模块源码：`metamod/`、`fallguys/`、`asext/`、`ascurl/`、`asqcvar/`、`asusermsg/`
- 工具与CI：`tools/`、`.github/workflows/`
- 大体量目录（避免全量读取）：`thirdparty/`、`build/`、`build-cmake/`、`intermediate/`、`output/`、`install/`、`Release/`、`Debug/`、`.vs/`

## 渐进式披露要点

- 先读 memories，再定位单文件/单符号；不要一次性读全仓库。
- 与符号/二进制相关的目录优先“按需定位”，避免全量扫描。

## Misc rules

- Always `activate_project` on agent startup.
