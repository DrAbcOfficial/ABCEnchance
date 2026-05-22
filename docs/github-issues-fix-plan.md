# GitHub Issues Fix Plan

## Issue #122: "Taking damage draw white square on the screen"
[Link](https://github.com/DrAbcOfficial/ABCEnchance/issues/122) | Opened May 22, 2026

**Symptom:** A white square appears on screen when the player takes damage.

**Root Cause Analysis:**
The damage tiles HUD (`dmgtiles.cpp`) draws direction indicators for incoming damage. The white square is likely:
1. A missing/invalid texture being used for the damage indicator.
2. A texture that failed to load (sprite not found, wrong path), causing `surface()->DrawSetTexture(-1)` which results in a white rectangle.
3. A shader issue causing white output.

**Files to Investigate:**
- `src/hud/vgui/dmgtiles.cpp` — Damage tiles rendering
- `src/hud/vgui/dmgtiles.h` — Texture handles
- `src/core/resource/spriteresource.cpp` — Sprite loading

**Fix Plan:**
1. Check `CDmgTilesPanel::Paint()` for texture validity before drawing.
2. Verify sprite paths in the initialization code.
3. Add null/valid checks for all draw textures.
4. Check `ApplySettings()` for damaged indicator sprite paths.
5. Add bounds check to prevent drawing with invalid textures.

---

## Issue #120: "HP HUD Shows 0 After Revive at 1 HP"
[Link](https://github.com/DrAbcOfficial/ABCEnchance/issues/120) | Opened Mar 19, 2026

**Symptom:** After a player dies at 1 HP and is revived (revive restores half HP = 0.5), the HUD shows 0 HP temporarily, treating player as dead.

**Root Cause Analysis:**
The HP value is received from the server as an integer. When the server calculates half of 1 = 0.5, it truncates to 0. The client-side health display treats 0 HP as dead, causing the HUD to disable until the server corrects the value.

**Files to Investigate:**
- `src/hud/vgui/health.cpp` — Health panel rendering
- `src/core/resource/playerresource.cpp` — Player info (health tracking)
- `src/hud/Viewport.cpp` — HUD enable/disable logic based on health

**Fix Plan:**
1. In the health panel, clamp displayed HP to minimum 1 if the player exists and is alive.
2. Alternatively, in `PlayerInfo::Update()` or the health event handler, clamp health to at least 1 if the player state is alive (but not dead/spectating).
3. Check `IsAlive()` or similar state tracking — the issue mentions the player briefly appears dead, meaning the alive/dead state may also be incorrectly set.

---

## Issue #119: "HUD Weapon Panel — Empty Slot Selects Previous Weapon"
[Link](https://github.com/DrAbcOfficial/ABCEnchance/issues/119) | Opened Mar 15, 2026

**Symptom:** When selecting a weapon in an empty slot and clicking mouse1, the weapon choose HUD selects the last weapon from another slot instead of closing the selection (as vanilla does).

**Root Cause Analysis:**
The weapon selection logic in `weaponchoose.cpp` doesn't check if the selected slot is empty before processing the selection. Vanilla behavior is to close the weapon HUD when the selected slot has no weapon.

**Files to Investigate:**
- `src/hud/vgui/weaponchoose.cpp` — Weapon selection panel
- `src/hud/vgui/weaponchoose.h` — Selection logic
- `src/hud/Viewport.cpp` — Input handling, weapon selection events

**Fix Plan:**
1. Find the mouse click handler or keyboard selection confirmation logic.
2. Before confirming weapon selection, check if `m_pHandledWeapon` (or equivalent) is valid for the selected slot.
3. If the slot is empty, close the weapon panel (or do nothing) instead of selecting the previous weapon.

---

## Issue #118: "Comma and dot cause main menu closing"
[Link](https://github.com/DrAbcOfficial/ABCEnchance/issues/118) | Opened Mar 15, 2026

**Symptom:** When a non-English language is selected, pressing comma (,) or dot (.) causes the main menu to close.

**Root Cause Analysis:**
This is likely a locale/keyboard input issue. In some locales, comma and dot keys are used for different purposes. The game may interpret these key presses as "close menu" or "escape" commands when the locale changes key bindings.

**Files to Investigate:**
- `src/hud/Viewport.cpp` — Input/key handling
- `src/core/events/command.cpp` — Command processing for keys
- `src/hud/vgui/textmenu.cpp` — Text menu keyboard handling
- `VGUI2/GameUI/BasePanel.cpp` — Game UI keyboard input

**Fix Plan:**
1. Check `VoidInput()` or key binding lookup logic — it may be mapping comma/dot incorrectly in non-English locales.
2. Verify `gEngfuncs.pfnKey_LookupBinding()` or similar key-binding functions aren't affected by locale.
3. Ensure keyboard input handling uses scan codes rather than virtual key codes where possible to avoid locale sensitivity.
4. Check if `isalpha()` or similar locale-dependent functions are used in keyboard input processing.

---

## Issue #117: "Buy menu text overlaps"
[Link](https://github.com/DrAbcOfficial/ABCEnchance/issues/117) | Opened Feb 19, 2026

**Symptom:** Text in the buy menu overlaps/overflows its container.

**Root Cause Analysis:**
The buy menu text (likely in the weapon selection or item purchase UI) has incorrect text layout. This could be due to:
1. Font size too large for the allocated space.
2. Missing text clipping or ellipsis.
3. The text is using a fixed layout that doesn't account for label length.
4. The text panel doesn't have proper word wrap or text fitting.

**Files to Investigate:**
- `src/hud/vgui/weaponchoose.cpp` — Buy menu / weapon selection
- `src/hud/vgui/weaponchoose.h` — Layout constants
- `src/hud/vgui/textmenu.cpp` — Text menu rendering
- `VGUI2/GameUI/BasePanel.cpp` — Game UI base panel
- Resource files defining the layout (RES files)

**Fix Plan:**
1. Check text label widths in the buy menu layout — they may need to be wider or use dynamic sizing.
2. Enable text clipping or word wrap on buy menu text labels.
3. Verify font sizes set on buy menu labels.
4. If using Label control, set `SetWrap(true)` or `SetTextEllipsis(true)`.
5. Check RES files in `Build/svencoop/abcenchance/res/` for incorrect label dimensions.

---

## Execution Order (as specified)
Fix issues in order: #122 → #120 → #119 → #118 → #117

---

## Priority Summary

| Issue | Severity | Complexity | Files |
|-------|----------|------------|-------|
| #122 | HIGH | Low | dmgtiles.cpp |
| #120 | HIGH | Low | health.cpp, playerresource.cpp |
| #119 | MEDIUM | Medium | weaponchoose.cpp |
| #118 | MEDIUM | Medium | Viewport.cpp, command.cpp |
| #117 | MEDIUM | Low-Medium | weaponchoose.cpp, textmenu.cpp |
