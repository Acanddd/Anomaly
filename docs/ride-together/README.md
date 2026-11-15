# Ride Together Core / 载具同乘配套核心

This branch preserves the modified Anomaly Core shipped with **Ride Together Full 1.1**. It is an **unofficial derivative**, not an AnomalyNTE release. The Ride Together plugin and game resources are distributed separately.

本分支保存 **载具同乘 Full 1.1** 随包使用的修改版 Anomaly Core；这是非官方衍生版本，不是 AnomalyNTE 官方发行版。载具同乘插件与游戏资源另行分发。

## Baseline and published binary / 基线与发布文件

- Upstream / 上游：[AnomalyNTE/Anomaly](https://github.com/AnomalyNTE/Anomaly), tag `v3.0.0`, commit `bdc6fa1267ddf03be4e3423d3b3fe04a4725d478`.
- Branch / 分支：`ride-together/3.0`.
- Release source tag / 发布源码标签：`ridetogether-core-1.1`.
- Published `Anomaly.Core.dll` SHA256: `b80b215f3b5ad10c8c19cec708f44096e7dc37876fda2cb08ecd9b8d11c8da6e`.
- Machine-readable provenance / 对应关系：[core-1.1.json](core-1.1.json).

The DLL in the Full 1.1 clean bundle matches the original build artifact byte for byte. All 971 `Anomaly-host` source files in the accompanying source ZIP match that build's source directory. This branch imports that snapshot, with Git line-ending normalization and the fork documentation identified here. Local test fixtures, logs, generated build trees and game assets are excluded. Fresh-build byte-for-byte reproducibility is not claimed: MSVC/linker metadata, paths and generated Git provenance can differ.

Full 1.1 全量包内 DLL 与原构建产物哈希一致；配套源码包的 971 个 `Anomaly-host` 文件与原构建源码逐字节一致。本分支导入该快照，仅进行 Git 行尾规范化并添加本文等分支说明，不包含本地测试夹具、日志、生成目录或游戏素材。重新构建可能因编译路径、链接器元数据及 Git 来源信息而产生不同哈希，不宣称逐字节可复现。

The original `anomaly_core` build target was rerun successfully during publication verification; its output retained the published SHA256.

本次公开前重新运行了原构建目录的 `anomaly_core` 目标，构建通过且产物仍保持上述发布哈希。

## Changes / 改动

- Scope-owned Game cleanup service for plugin shutdown, with bounded completion and quarantine handling.
- Independent plugin-window presentation, focus/input handling and phone-entry reconciliation.
- Append-only Core/object service extensions for guarded memory exchange and object-address resolution.
- Panel styling, selection and layout APIs, custom window chrome, and matching SDK ABI snapshot updates.
- Executable-section symbol-resolution support and TLS-free UI resource initialization for manual mapping.

- 插件作用域内的 Game 清理服务，包含有界完成与隔离处理。
- 独立插件窗口、焦点／输入及手机入口协调。
- Core／对象服务尾部扩展，用于受控内存交换与对象地址解析。
- 面板样式、选项与布局接口、自定义窗口外观及对应 SDK ABI 快照。
- 可执行节符号解析支持，以及手动映射所需的无静态 TLS 界面资源初始化。

Existing ABI prefixes are retained, but this is **not a blanket compatibility guarantee** for every third-party plugin or game build. Ride Together Full 1.1 requires its paired Core; the stock official Core does not provide all required extensions. Aurora automatic installation is not validated by this source publication.

保留既有 ABI 前缀不等于保证全部第三方插件或游戏版本兼容。载具同乘 Full 1.1 需要配套 Core，官方原版 Core 不包含全部所需扩展。本次公开源码不代表已完成 Aurora 自动安装适配。

## Build / 构建

Requirements: Windows x64, Visual Studio 2022 C++ Build Tools with Windows SDK, CMake supporting preset schema 6, and PowerShell 7. See the upstream [build guide](../developer-guide/building.md).

前置条件：Windows x64、Visual Studio 2022 C++ Build Tools 与 Windows SDK、支持 schema 6 预设的 CMake、PowerShell 7。其余依赖由已有 CMake 配置获取。

```powershell
cmake --preset windows-vs2022 -DANOMALY_BUILD_BUILTIN_PLUGINS=OFF
cmake --build --preset windows-relwithdebinfo --target anomaly_core --parallel
```

Output / 产物：`.build/windows-vs2022/bin/RelWithDebInfo/Anomaly/Anomaly.Core.dll`.

The original development build also enabled ignored `.local` fixture targets and a link map. They do not supply Core implementation sources and are not required for the production `anomaly_core` target. Do not copy an arbitrary rebuilt DLL over an installed package without checking the plugin's pairing contract.

原开发构建另启用了 `.local` 测试目标和链接映射；它们不提供 Core 实现源码，生产 `anomaly_core` 目标不依赖这些夹具。自行重建的 DLL 不应未经配对检查就覆盖玩家安装。

## License and maintenance / 许可与维护

The upstream [LICENSE](../../LICENSE), [NOTICE](../../NOTICE) and third-party notices remain in place. Anomaly and these modifications are distributed under the repository's AGPL-3.0 terms. This branch is a source reference for the paired release, not an official endorsement or a promise of perpetual API compatibility. Future work should prefer upstream services when equivalent support becomes available.

保留上游许可证、版权与第三方声明；Anomaly 及本分支修改遵循仓库 AGPL-3.0 许可。本分支用于明确保存配套版本的源码，不代表官方背书或永久接口兼容承诺；官方提供等效能力后，优先回归官方接口。
