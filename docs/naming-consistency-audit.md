# Naming Consistency Audit Plan

## Overview
This document catalogs naming inconsistencies, typos, and conventions issues in `src/` (excluding thirdparty/, SourceSDK/).

---

## 1. CRITICAL: Macro References Non-Existent Class

### 1.1 `src/core/library/mymathlib.h:37`
**Bug:** `#define AngleIVectors CCMathlib::AngleVectorsTranspose` — class name `CCMathlib` does not exist. The correct class is `CMathlib`.
**Fix:** Change to `#define AngleIVectors CMathlib::AngleVectorsTranspose`

---

## 2. HIGH: Typos That Affect Correctness

### 2.1 `src/core/module/extraprecache.cpp` — Multiple typos
| Line | Typo | Correct |
|------|------|---------|
| 7 | `g_ExtraPreacheModel` | `g_ExtraPrecacheModel` |
| 8 | `g_iExtraPrecaheSize` | `g_iExtraPrecacheSize` |
| 11 | `EXTRPRECACHE_INDEX_BASE` | `EXTRAPRECACHE_INDEX_BASE` |
| 17,18,30-33,38-41 | Various uses of above | Fix all occurrences |

### 2.2 `src/core/library/soundengine.h:355` — `m_pFModChennel`
**Bug:** Typo `Chennel` should be `Channel`. Used ~75 times in `soundengine.cpp`.
**Fix:** Rename to `m_pFModChannel` across both files.

### 2.3 `src/core/library/httpclient.h:102` — `m_iStatue` and related
| Line | Typo | Correct |
|------|------|---------|
| 102 | `m_iStatue` (variable) | `m_iStatus` or `m_iState` |
| 19 | `DESTORYED` (enum) | `DESTROYED` |
| 57,91 | `m_pOnDestory` | `m_pOnDestroy` |
| 96 | `m_aryReciveData` | `m_aryReceiveData` |
| 116 | `pDestory` (param) | `pDestroy` |
**Fix:** Fix all occurrences in both .h and .cpp files (~15+ locations).

### 2.4 `src/utility/util.h:19,54` — Parameter typos
**Line 19:** `PlaySoundByName(const char* x, float volum)` — `volum` -> `volume`
**Line 54:** `float* pucnangle1, float* pucnangle2` — `pucnangle` -> `punchangle`
**Fix:** Fix in both .h declaration and .cpp definition.

---

## 3. MEDIUM: Naming Inconsistencies

### 3.1 File Naming Convention Inconsistency
**Description:** `src/core/module/` files are all lowercase/snake_case, while `src/core/library/` mixes PascalCase (`Task.h`, `NetworkMessageReader.h`) with lowercase (`httpclient.h`, `mymathlib.h`).
**Fix:** Rename files to follow consistent convention. Recommended: all lowercase for `src/core/`.

### 3.2 Header Guard Inconsistency (4 different styles)
| Style | Files |
|-------|-------|
| `#pragma once` only | Most files |
| `#pragma once` + `#ifndef` | `voice_status.h`, `voice_banmgr.h`, `gl_common.h`, `crosshair.h` |
| `#ifndef __NAME__` | `playerresource.h`, `teamresource.h`, `spriteresource.h`, `weaponresource.h` |
| `#ifndef NAME` (no underscores) | `health.h`, `ammobar.h`, etc. |
| Duplicate `#pragma once` | `command.h:1,3` |
**Fix:** Standardize to `#pragma once` only. Remove redundant guards. Fix duplicate in `command.h`.

### 3.3 Hungarian Prefix Inconsistencies
| File | Line | Issue |
|------|------|-------|
| `hud/Viewport.h` | 168 | `m_bitsHideHUDDisplay` — should be `m_iHideHUDDisplay` |
| `hud/Viewport.h` | 171 | `m_bitsWeaponBits` — should be `m_iWeaponBits` |
| `hud/vgui/ammobar.h` | 39 | `m_Ammo1Icon` — should be `m_pAmmo1Icon` |
| `utility/util.cpp` | 9 | `m_szCurrentLanguage` — global with `m_` prefix, should be `g_szCurrentLanguage` |
**Fix:** Rename to follow `m_[type prefix][Name]` convention consistently.

---

## 4. LOW: Typos (Non-Critical)

| File | Line | Typo | Correct |
|------|------|------|---------|
| `hud/vgui/effect.h` | 55 | `m_iOldWaterStatue` | `m_iOldWaterStatus` or `m_iOldWaterState` |
| `hud/vgui/health.h` | 56,61 | `m_pHealthImageBackround`, `m_pArmorImageBackround` | `...Background` |
| `hud/Viewport.h` | 77 | `LoacalPlayerAvilable()` | `LocalPlayerAvailable()` |
| `hud/vgui/neteasemusic.h` | 55 | `IsSuppressBackGroudMusic()` | `IsSuppressBackgroundMusic()` |
| `hud/vgui/popnum.h` | 30 | `Destory()` | `Destroy()` |
| `utility/util.h` | 10 | `ConsoleWriteline` | `ConsoleWriteLine` |
| `utility/util.h` | 135 | Comment: `//Defualt CVars` | `//Default CVars` |
| `core/library/mymathlib.h` | 58 | `VectorMultipiler` | `VectorMultiplier` |
| `core/library/soundengine.h` | 1 | Guard: `__FMOD_SOUNDEING_` | `__FMOD_SOUNDENGINE__` |
| `core/module/efxenchance.h/.cpp` | - | Filename: `enchance` | `enhance` (file rename) |
| `exportfuncs.cpp` | 405 | Comment: `destory vanillia` | `destroy vanilla` |

### 4.1 `src/core/library/soundengine.h:1` — Header guard typo
**Fix:** Change `__FMOD_SOUNDEING_` to `__FMOD_SOUNDENGINE__` (or just use `#pragma once`).

### 4.2 `src/core/module/efxenchance.h` / `.cpp` — Filename typo
**Description:** `enchance` should be `enhance`.
**Fix:** Rename files. Update all includes (check with grep first).

---

## 5. Include Path Inconsistencies

### 5.1 `src/utility/util.cpp:5`, `src/core/library/httpclient.cpp:4`
**Issue:** `#include <Controls.h>` uses angle brackets for a local VGUI file. Other files use `#include "vgui_controls/Controls.h"` with quotes.
**Fix:** Change to quoted includes.

### 5.2 `src/core/library/NetworkMessageReader.cpp:1`
**Issue:** `#include "metahook.h"` uses quotes for external MetaHook framework. Should use `#include <metahook.h>` like other files.
**Fix:** Change to angle brackets.

### 5.3 `src/exportfuncs.cpp:36`
**Issue:** `#include "glew.h"` uses quotes for GLEW (external library). Should be `#include <glew.h>`.
**Fix:** Change to angle brackets.

---

## 6. Namespace Inconsistencies

| Namespace | Convention | Issue |
|-----------|-----------|-------|
| `namespace mathlib` | lowercase | Fine |
| `namespace lrc` | lowercase | Fine |
| `namespace FModEngine` | PascalCase, odd `FMod` | Should be `fmod_engine` or `FMODEngine` |
| `namespace abcconfig` | lowercase | Fine |
| `namespace AutoFunc` | PascalCase | Inconsistent with other lowercase namespaces |
| `namespace HudEvent` | PascalCase, `Hud` not `HUD` | Should be `HUDEvent` or `hud_event` |
| `namespace vgui` | lowercase | Consistent with framework |

**Fix:** Standardize to lowercase snake_case for all custom namespaces.

---

## Priority Summary

| Priority | Issue | Count |
|----------|-------|-------|
| P0 | #1.1 Broken macro `AngleIVectors` | 1 |
| P1 | #2.1-2.4 Typos affecting correctness | ~100 lines |
| P2 | #3.2 Header guard inconsistency | ~50 files |
| P3 | #5.x Include path fixes | ~5 files |
| P4 | #4.x Non-critical typos | ~12 locations |
