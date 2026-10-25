# AGENTS.md

Guidance for AI/automation collaborators working in this repository. Read `README.md` first; it is the only document distributed with the release.

## Hard rules

- Nothing committed (code, comments, docs, commit messages, tags) may contain local machine paths, personal directories, session logs or any credentials.
- Do not commit test scaffolding, acceptance scripts, evidence screenshots, plans or progress logs; those are session artifacts that do not belong in the release.
- Every module package keeps `svencoop/` at the archive root so the merged release extracts directly into the Sven Co-op folder; do not add a versioned top-level folder to a module zip.
- Submodule pins may only point at commits that are already public upstream; confirm the commit is pushed before bumping a pin.

## README

- The README is the single public document and is written for end users.
- Keep it short: features, installation and user-facing commands or settings
  only. Do not document architecture, module layout, ABI or interface
  contracts, design conventions or internal implementation, and never add
  parenthesised technical remarks.
- Do not describe changelogs, TODOs or FIXMEs.
- The only technical content is how to build the dist package, including its
  prerequisites.

## Build

Win32 x86 only. The root project builds the three modules through ExternalProject and exposes one `dist` target:

```bash
cmake -S . -B build/ci -G "Visual Studio 18 2026" -A Win32
cmake --build build/ci --config Release --parallel
cmake --build build/ci --config Release --target dist
```

`dist` writes `AbcCore-<version>.zip`, `AbcEssential-<version>.zip` and `AbcUi-<version>.zip` to `build/ci/dist`. The release workflow merges them into `ABCEnchance-<tag>.zip`; keep its required-file list in sync when a package gains or loses files.

## Conventions

- Commit messages use `type(scope): summary` (feat / fix / refactor / docs / build / chore) with motivation and verification in the body.
- Before handing off: no local paths or credentials; README matches actual behaviour; the merged package contains all three plugins.
