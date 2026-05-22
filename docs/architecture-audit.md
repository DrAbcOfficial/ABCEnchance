# Architecture Audit Plan

## Overview
This document catalogs architecture and design issues found in `src/` (excluding thirdparty/, SourceSDK/). Issues are grouped by category with severity, location, and fix plan.

---

## 1. Missing Virtual Destructors (CRITICAL)

### 1.1 `src/Particleman/pman_triangleffect.h:32` — `CCoreTriangleEffect`
**Severity:** CRITICAL
**Description:** Base class with 15 pure virtual functions has no virtual destructor. Deleting derived objects through `CCoreTriangleEffect*` is undefined behavior.
**Fix:** Add `virtual ~CCoreTriangleEffect() = default;`

### 1.2 `src/Particleman/IParticleMan.h:66` — `CBaseParticle` inherits from `CCoreTriangleEffect`
**Severity:** HIGH
**Description:** Depends on #1.1 fix. If base gets virtual dtor, all derived classes will work correctly.
**Fix:** Will be resolved by fixing #1.1.

### 1.3 `src/hud/vgui/basestackpanel.h:11` — `CBaseSprStackItem`
**Severity:** HIGH
**Description:** Has virtual function `CheckExpire()` but no virtual destructor. Subclasses (`CWeaponStackItem`, `CItemStackItem`, `CAmmoStackItem`) are managed via base class pointers.
**Fix:** Add `virtual ~CBaseSprStackItem() = default;`

### 1.4 `src/hud/vgui/basestackpanel.h:29` — `CBaseStackPanel`
**Severity:** MEDIUM
**Description:** Has pure virtual `GetName()` without declared virtual destructor. Relies on VGUI base class.
**Fix:** Add `virtual ~CBaseStackPanel() = default;`

---

## 2. Raw Owning Pointers (HIGH)

### 2.1 `src/core/library/httpclient.h:99,118` — `CHttpClient`
**Description:** Uses `std::vector<CHttpClientItem*>` and raw `IUtilHTTPRequest*` as owning pointers.
**Fix:** Replace with `std::vector<std::unique_ptr<CHttpClientItem>>`. Requires updating all `push_back(new ...)` to `push_back(std::make_unique<...>())` and removing manual `delete` calls.

### 2.2 `src/core/library/Task.h:79-80` — `CTaskManager`
**Description:** Two `std::list<ITaskItem*>` owning raw pointers.
**Fix:** Replace with `std::list<std::unique_ptr<ITaskItem>>`.

### 2.3 `src/core/library/liblrc.h:21-22` — `CLrcCollection`
**Description:** `std::vector<CLrcLine*>` and `std::vector<lrcattribute_t*>` owning raw pointers.
**Fix:** Replace with `std::vector<std::unique_ptr<...>>`.

### 2.4 `src/hud/vgui/basestackpanel.h:56` — `CBaseStackPanel`
**Description:** `std::list<CBaseSprStackItem*>`. VGUI panels are reference-counted.
**Fix:** Use `vgui::PHandle` or `vgui::DHANDLE` for safe lifetime management.

---

## 3. Single Responsibility Violations (MEDIUM)

### 3.1 `src/hud/Viewport.h:51` — `CViewport` God Object
**Description:** 668-line class managing 25 HUD panels, entity management, intermission, third-person, titles, pop numbers, spectator state, and server info. The `Start()` method is 319 lines.
**Fix Plan:**
1. Extract panel registration into `CViewport::InitPanels()`
2. Extract event callback registration into `CViewport::RegisterCallbacks()`
3. Create per-panel initialization helper methods
4. Delegate entity/title management to dedicated classes

### 3.2 `src/core/resource/weaponresource.h:152` — `WeaponsResource`
**Description:** Handles weapon registry, player inventory, selection logic, sprite loading, slot management — all in one class.
**Fix Plan:** Split into `WeaponRegistry`, `PlayerInventory`, and `WeaponSelectionLogic` classes.

### 3.3 `src/hud/vgui/scoreboard.h:22` — `CScorePanel`
**Description:** Handles scores, avatars, team sorting, 7 context menu actions, mute/friend icons, 5 size modes, 9 texture columns, donor icons, killer highlighting, and labels.
**Fix Plan:** Extract `ScoreLayout`, `AvatarManager`, and `ContextMenuHandler`.

---

## 4. Missing Const Correctness (MEDIUM)

### 4.1 `src/core/resource/teamresource.h:56` — `TeamInfo::GetName()`
**Fix:** Add `const` qualifier: `const char* GetName() const;`

### 4.2 `src/hud/vgui/vote.h:27` — `IsVoteEnable()`
**Fix:** Add `const`: `bool IsVoteEnable() const;`

### 4.3 `src/hud/Viewport.h:85-116` — Multiple query methods
**Description:** `IsScoreBoardVisible()`, `IsPlayerTileEnable()`, `IsVoteEnable()`, `IsTextMenuOpen()`, `IsInScore()` — none are `const`.
**Fix:** Add `const` to all.

### 4.4 `src/core/library/Task.h:74` — `CTaskManager::Has()`
**Fix:** Add `const`: `bool Has(...) const;`

### 4.5 `src/core/library/soundengine.h:235,350` — `Valid()` methods
**Fix:** Add `const` to `CFModSound::Valid()` and `CFModChannel::Valid()`.

---

## 5. Public Members Lacking Encapsulation (LOW)

### 5.1 `src/core/resource/weaponresource.h:34-101` — `WeaponData` and `Weapon`
**Description:** 55+ public data members, zero encapsulation.
**Fix:** Convert to private members with getters/setters where appropriate.

### 5.2 `src/core/resource/playerresource.h:47-86` — `PlayerInfo`
**Description:** 15 public members. Mixed public/private sections.
**Fix:** Move members to private with accessors.

### 5.3 `src/core/module/voice_status.h:63-86` — `CVoiceStatus`
**Description:** All member variables are public.
**Fix:** Make private, add const accessors.

---

## 6. Hardcoded Magic Numbers (LOW)

Many instances across the codebase. Key fixes:
- `efxenchance.cpp:128` — Max hits should be a named constant
- `scoreboard.cpp:1224-1232` — Screen height thresholds should be named constants
- `playerboard.cpp:87` — Draw distance (1024) should be a constant
- `itemhighlight.cpp:272` — Pickup range (86) should be a constant
- Multiple `for (size_t i = 0; i < 32; i++)` loops should use `SC_MAX_PLAYERS`

---

## 7. Long Functions (LOW)

### 7.1 `src/hud/Viewport.cpp:99` — `CViewport::Start()` (319 lines)
**Fix:** Split into `InitPanels()`, `InitCrosshair()`, `RegisterHudCallbacks()`, `RegisterEngineCallbacks()`.

### 7.2 `src/core/module/efxenchance.cpp:128` — `DoGaussFire()` (156 lines)
**Fix:** Extract `ProcessGaussTraces()`, `RenderGaussBeam()`, `CalculateGaussDamage()`.

### 7.3 `src/hud/vgui/crosshair.cpp:97` — `CCrosshairPanel::OnThink()` (117 lines)
**Fix:** Extract `CalculateSpread()`, `CheckCrosshairVisibility()`, `UpdateThirdPersonCrosshair()`.

---

## Priority Summary

| Priority | Issue | Impact |
|----------|-------|--------|
| P0 | #1.1 Virtual dtor on `CCoreTriangleEffect` | Undefined behavior crash risk |
| P0 | #1.3 Virtual dtor on `CBaseSprStackItem` | Memory corruption on delete |
| P1 | #2.1-2.3 Raw owning pointers | Memory leaks |
| P1 | #3.1 CViewport god object | Maintainability |
| P2 | #4.x Missing const | Code quality |
| P2 | #6.x Magic numbers | Readability |
| P3 | #5.x Public members | Encapsulation |
| P3 | #7.x Long functions | Maintainability |
