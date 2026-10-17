# ABCEnchance

Aggregation and release repository for the ABC plugin family. This branch is a
fresh aggregate history. The retired monolithic ABCEnchance plugin is preserved
on the `legacy` branch (last legacy commit `598f924`) and in the old tags.

## Modules

| Path | Upstream | Package artifact |
|---|---|---|
| `AbcCore/` | https://github.com/DrAbcOfficial/AbcCore | `AbcCore-<version>.zip` — `AbcCore.dll` + shared gamedata |
| `AbcEssential/` | https://github.com/DrAbcOfficial/AbcEssential | `AbcEssential-<version>.zip` — direct-extract game package |
| `AbcUi/` | https://github.com/DrAbcOfficial/AbcUi | `AbcUi-<version>.zip` — direct-extract game package |

`AbcCore` is the shared core DLL. `AbcEssential` is the non-UI utility
plugin. `AbcUi` owns the renderer bridge and the RmlUi interface. The aggregate
repository is the single owner of the AbcCore checkout: AbcUi and AbcEssential
consume it through `ABC_CORE_DIR` and no longer carry their own AbcCore
submodule.

## Install (release package)

Extract `ABCEnchance-<tag>.zip` directly into the Sven Co-op root. The archive
contains `svencoop/` at its root, so there is no extra version folder to copy:

```text
Sven Co-op/
├── svencoop/
│   ├── abc/
│   │   ├── abcessential/
│   │   ├── abcui/
│   │   ├── mdl/
│   │   └── spr/
│   └── metahook/
│       ├── dlls/AbcCore/AbcCore.dll
│       ├── gamedata/abc/
│       └── plugins/
│           ├── AbcEssential.dll
│           └── AbcUi.dll
└── svencoop.exe
```

MetahookSV, `VGUI2Extension.dll` and `Renderer.dll` are prerequisites and are
installed separately. `FreeImage.dll` and `Mile.FFmpeg.dll` are provided by
MetaHookSV/Renderer and are not part of this package. Add the plugins to
`svencoop/metahook/configs/plugins.lst` in this order:

```text
VGUI2Extension.dll
Renderer.dll
AbcEssential.dll
AbcUi.dll
```

The VC++ 2015-2022 x86 runtime is required: AbcCore.dll and both plugins use
the dynamic CRT so STL objects can cross the shared DLL boundary safely.

The individual `AbcUi-<version>.zip` and `AbcEssential-<version>.zip` assets use
the same root-level `svencoop/` layout. `AbcCore-<version>.zip` contains the
runtime `AbcCore.dll` for `metahook/dlls/AbcCore/` and the single shared
`metahook/gamedata/abc/` catalog. The merged `ABCEnchance-<tag>.zip` includes
all three; the individual plugin packages need the matching AbcCore runtime and
shared gamedata separately.

## Clone

```bash
git clone --recursive https://github.com/DrAbcOfficial/ABCEnchance.git
```

`AbcUi` pins a `Renderer` commit that currently lives in the
`DrAbcOfficial/Renderer` fork. If the recursive clone cannot find it, configure
the URL rewrite before fetching submodules:

```bash
git config --global url."https://github.com/DrAbcOfficial/Renderer".insteadOf "https://github.com/MetaHookSv/Renderer"
git submodule update --init --recursive
```

## Build

The root `CMakeLists.txt` is the aggregate build entry point. Each module keeps
its own CMake project and presets; the root uses `ExternalProject` to build them
in isolated binary directories, because `AbcUi` and `AbcEssential` each vendor a
pinned `AbcCore` and would otherwise define the same CMake target twice.

Win32 x86 only:

```bash
cmake -S . -B build/ci -G "Visual Studio 18 2026" -A Win32
cmake --build build/ci --config Release --parallel
cmake --build build/ci --config Release --target dist
```

The aggregate `dist` target writes `AbcCore-<version>.zip`,
`AbcEssential-<version>.zip` and `AbcUi-<version>.zip` to `build/ci/dist/`.
With Visual Studio 2022, use `-G "Visual Studio 17 2022"`.

You can still build a single module directly with its own presets:

```bash
cd AbcUi
cmake --preset win32-release
cmake --build --preset win32-release
```

## CI and releases

`.github/workflows/aggregate-release.yml` configures the root CMake project and
builds `Debug` and `Release` for all three modules. A `v*` tag publishes one
GitHub Release with exactly one asset:

- `ABCEnchance-<tag>.zip` (merged AbcUi + AbcEssential + AbcCore game package
  with `svencoop/` at the archive root)

Push and pull-request builds upload the three module packages plus the merged
`ABCEnchance-game.zip` as workflow artifacts without creating a GitHub Release.
The module zips stay available as CI artifacts for diagnostics; the tag release
renames only the merged package to `ABCEnchance-<tag>.zip` and attaches that
single file.

Each module retains its own CI in its upstream repository; this repository is
the single aggregate build and release entry point.

## License

The modules are distributed under the BSD 3-Clause License; each upstream
repository and its dependencies keep their own license files.
