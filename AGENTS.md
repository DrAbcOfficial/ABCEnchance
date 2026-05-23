# AGENTS.md

## Build

```powershell
# First time: clone submodules (required before any build)
git submodule update --init --recursive

# Build (interactive, picks config)
.\build-ABCEnchance.ps1

# Or direct MSBuild (Win32 only — there is no x64 target)
msbuild /m /p:Configuration=Release /t:ABCEnchance
msbuild /m /p:Configuration=Release_AVX2 /t:ABCEnchance
msbuild /m /p:Configuration=Debug /t:ABCEnchance
```

- **Only builds for Win32 (x86).** The solution has AnyCPU entries but they map to Win32; there is no x64 configuration.
- `Release` copies output DLL to `Build/ABCEnchance.dll`. `Release_AVX2` copies to `Build/ABCEnchance_AVX2.dll`. `Debug` does NOT copy.
- Build depends on `thirdparty/MetaHookSv/` (git submodule). Missing submodules = build failure.
- Language standards: C++20, C17. Character set: MultiByte (source files use GBK encoding).

## Architecture

- **Entry point**: `src/plugins.cpp:105` — implements `IPluginsV4` (MetaHookSV plugin interface).
- **Headers live in `src/Header/`**, sources in `src/core/`, `src/hud/`, `src/utility/`, etc. This split is non-standard — when adding includes, use `#include "core/module/config.h"` style paths (not relative from Header dir).
- **Renders only on SvEngine**: `src/plugins.cpp:55-59` — calls `SYS_ERROR()` if `g_iEngineType != ENGINE_SVENGINE`. Any other engine type is a fatal error.
- **VGUI2** (`VGUI2/`, `vgui_controls/`) is a custom implementation bundled in-tree, not from MetaHookSV.
- **SourceSDK** (`SourceSDK/`) contains local copies of Valve Source SDK tier0/tier1/vstdlib — not externally linked.
- **Shader files** live in `Build/svencoop/abcenchance/shader/` (not in source tree).
- **Submodules**: `thirdparty/MetaHookSv`, `thirdparty/eventpp`, `thirdparty/AIGCJson`.

## Key preprocessor defines

`GLEW_STATIC`, `VGUI_USE_SURFACE2`, `VGUI_USE_SCHEME2`, `VGUI_USE_INPUT2`, `NO_MALLOC_OVERRIDE`, `NO_TIER0_MINIDUMP`

## No test/lint infrastructure

There are no tests, no linter, no formatter, and no CI checks beyond build success. The only verification is that the DLL compiles.

## CI

- **livebuild.yml**: Builds Release + Release_AVX2 on every push/PR to `main`, uploads artifact.
- **msbuild.yml**: Same but also builds installer and creates GitHub Release on `v*` tags.

## Additional docs

See `CLAUDE.md` for detailed module descriptions, CVar reference, and Hook architecture.
