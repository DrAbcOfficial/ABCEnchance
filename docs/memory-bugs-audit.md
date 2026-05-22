# Memory Bugs & Crash Risk Audit Plan

## Overview
This document catalogs memory bugs, potential crashes, and code quality issues found in `src/` (excluding thirdparty/, SourceSDK/).

---

## 1. CRITICAL: Memory Leaks

### 1.1 `src/hud/Viewport.cpp:94-96` — Viewport destructor uses `~IViewportPanel()` instead of `delete`
**Bug:** The destructor explicitly calls `panel->~IViewportPanel()` without deallocating memory. All panels are allocated with `new` at lines 389-417. This is both a massive leak and technically UB (destructor called on uninitialized VGUI subobject).
```cpp
for (auto panel : m_Panels) {
    panel->~IViewportPanel();  // BUG: should be delete panel
}
```
**Fix:** Change to `delete panel;`

### 1.2 `src/hud/Viewport.cpp:550-553` — `AddPopNumber()` leaks `CPopNumberPanel`
**Bug:** `new CPopNumberPanel(...)` is never deleted. VGUI panels with parent set use auto-delete when parent is deleted, but this relies on VGUI behavior that may vary.
**Fix:** Store pop number panels or ensure VGUI auto-delete works. Consider adding to `m_Panels` vector.

### 1.3 `src/core/library/win32helper.cpp:91—104` — `unsigned char* bits` leak
**Bug:** `new unsigned char[width * height * 4]` allocated at line 91, used to extract pixel data, then `hBmp` and `hdc` are freed but `bits` is never deleted.
**Fix:** Add `delete[] bits;` before return. Or use `std::vector<unsigned char>`.

### 1.4 `src/core/library/win32helper.cpp:72-74` — GDI bitmap handle leaks
**Bug:** `GetIconInfo()` returns `ICONINFO` with `hbmColor` and `hbmMask` GDI handles. Per MSDN, these must be freed with `DeleteObject()`. They are leaked.
**Fix:** Add `if (iconInfo.hbmColor) DeleteObject(iconInfo.hbmColor); if (iconInfo.hbmMask) DeleteObject(iconInfo.hbmMask);`

---

## 2. HIGH: Null Pointer Dereferences

### 2.1 `src/core/resource/playerresource.cpp:84` — No null check on `GetLocalPlayer()`
**Bug:** `IsThisPlayer()` dereferences `gEngfuncs.GetLocalPlayer()->index` without checking for null.
**Fix:** Add null check:
```cpp
bool PlayerInfo::IsThisPlayer() const {
    auto* lp = gEngfuncs.GetLocalPlayer();
    return lp ? (m_iIndex == lp->index) : false;
}
```

### 2.2 `src/core/resource/playerresource.cpp:108` — `Update()` null deref risk
**Bug:** `gEngfuncs.GetLocalPlayer()->index` accessed without null check (though `info` is checked earlier at line 95).
**Fix:** Add null check guard.

### 2.3 `src/hud/vgui/scoreboard.cpp:509` — `DeathMsg()` null deref
**Bug:** `gPlayerRes.GetLocalPlayerInfo()->m_iIndex` — `GetLocalPlayerInfo()` can return nullptr when local player is null.
**Fix:** Add null check:
```cpp
auto* lpInfo = gPlayerRes.GetLocalPlayerInfo();
if (!lpInfo) return;
if (victim == lpInfo->m_iIndex) ...
```

### 2.4 `src/hud/vgui/neteasemusic.cpp:807-809` — No null check after `FreeImage_LoadFromMemory`
**Bug:** If `FreeImage_LoadFromMemory` returns nullptr (invalid image data), `FreeImage_GetBits(nullptr)` crashes.
**Fix:** Add null check:
```cpp
FIBITMAP* bitmap = FreeImage_LoadFromMemory(format, mem);
if (!bitmap) return nullptr;
byte* pixels = (byte*)FreeImage_GetBits(bitmap);
```

---

## 3. HIGH: Uninitialized Variables

### 3.1 `src/hud/vgui/scoreboard.cpp:330` — `m_iCurrentImage` never initialized
**Bug:** `m_iCurrentImage` (type `size_t`) has no initializer. Used in `Paint()` before `OnTick()` runs. Accessing indeterminate index causes UB.
**Fix:** Initialize to `0`: `size_t m_iCurrentImage = 0;`

### 3.2 `src/hud/vgui/scoreboard.cpp:323` — `m_bDrawFriend` never initialized
**Bug:** `bool m_bDrawFriend;` — no initializer. Set in `SetFriend()` but may be read before that in `Paint()`.
**Fix:** Initialize: `bool m_bDrawFriend = false;`

---

## 4. HIGH: Buffer Overflow Risks

### 4.1 `src/core/module/voice_status.cpp:144-167` — `sprintf/strcat` on fixed buffers
**Bug:** `sprintf(numStr, " %lx", banMask)` could write up to 513 chars into `numStr[512]`. Then `strcat(str, numStr)` could overflow `str[2048]`.
**Fix:** Use `snprintf` with buffer sizes or `Q_snprintf`.

### 4.2 `src/hud/vgui/notice.cpp:26` — `sprintf` with variable-length string
**Bug:** `sprintf(szControl, VGUI2_ROOT_DIR "%s.res", szControlName)` on `char szControl[256]` could overflow.
**Fix:** Use `snprintf(szControl, sizeof(szControl), ...)`.

---

## 5. HIGH: Division by Zero Risks

### 5.1 `src/hud/vgui/deadmsg.cpp:111` — `h / m_pDeathMsgCount->value`
**Bug:** If CVar value is 0, division by zero.
**Fix:** Guard: `int count = max(1, m_pDeathMsgCount->value);`

### 5.2 `src/hud/vgui/motd.cpp:93` — `flTime / m_flKeepTime`
**Bug:** If `m_flKeepTime` is 0, division by zero.
**Fix:** Guard: `if (m_flKeepTime <= 0.0f) return;`

---

## 6. HIGH: Out-of-Bounds Access

### 6.1 `src/core/resource/playerresource.cpp:126` — Array index out of bounds
**Bug:** `g_aryNativeHUDPanelInfo[m_iIndex - 1]` — if `m_iIndex` is 0 (or 1 where the array is 0-indexed), this accesses element at -1.
**Fix:** Add bounds check: `if (m_iIndex > 0 && m_iIndex <= g_aryNativeHUDPanelInfo.size())`

### 6.2 `src/core/resource/spriteresource.cpp:46` — Unsigned bounds check bug
**Bug:** `return (index < 0) ? 0 : m_arySprites[index]->hspr;` — `index` is `size_t` (unsigned), so `index < 0` is always false. No upper bounds check.
**Fix:** Check `index >= m_arySprites.size()` instead.

---

## 7. MEDIUM: Race Conditions

### 7.1 `src/hud/vgui/neteasemusic.cpp:782-877` — Static variable in task lambda
**Bug:** `static music_obj obj;` inside lambda — shared mutable state. Multiple overlapping music requests will overwrite each other's data.
**Fix:** Remove `static`, allocate on heap via `std::make_unique<music_obj>()` and pass ownership through the task chain.

---

## 8. MEDIUM: Unsafe Casts

### 8.1 `src/hud/vgui/scoreboard.cpp:135` — Double const-stripping cast
**Bug:** `reinterpret_cast<BYTE*>(const_cast<char*>(data))` — strips `const` from input data. UB if `FreeImage_OpenMemory` writes to the buffer.
**Fix:** If the buffer is read-only, don't cast away const.

### 8.2 `src/core/library/liblrc.cpp:92` — `reinterpret_cast<const char*>` from `char8_t*`
**Bug:** `reinterpret_cast<const char*>(CurrentLyric.c_str())` — `std::u8string::c_str()` returns `const char8_t*`. `char8_t` and `char` may have different alignment on some compilers.
**Fix:** Use proper `std::u8string` type or convert explicitly.

---

## 9. MEDIUM: Broken Logic

### 9.1 `src/core/library/mymathlib.cpp:330-332` — Debug leftover in normal calculation
**Bug:** 
```cpp
if (fabs(v[1] - 0.000215956) < 0.0001)
    i = 1;
length = 0;
for (i; i < 3; i++)
```
This appears to be a debugging leftover that skips `v[0]` in length calculation when a specific condition is met.
**Fix:** Remove this code block. It causes incorrect normalization.

### 9.2 `src/hud/vgui/ammobar.cpp:109` — Missing buffer size in `Q_snprintf`
**Bug:** `Q_snprintf(buf, "%dx%d/%d", ...)` — `Q_snprintf` requires buffer size as first argument. Missing size means potential buffer overflow.
**Fix:** Add buffer size: `Q_snprintf(buf, sizeof(buf), "%dx%d/%d", ...)`

---

## 10. Resource Leaks

### 10.1 `src/core/library/win32helper.cpp:72-74` — GDI bitmap handle leaks
See #1.4 above.

### 10.2 `src/core/library/win32helper.cpp:96` — Leak on error path
**Bug:** If `DrawIconEx` fails, `hBmp` and `bits` leaked.
**Fix:** Use RAII (e.g., `std::unique_ptr` with custom deleter for GDI objects).

---

## Priority Summary

| Priority | Issue | Fix Complexity |
|----------|-------|----------------|
| P0 | #1.1 Viewport destructor memory leak | Simple — one line change |
| P0 | #1.2 PopNumberPanel leak | Medium — need to track panels |
| P0 | #1.3 bits leak in win32helper | Simple — add delete[] |
| P0 | #1.4 GDI handle leaks | Simple — add DeleteObject calls |
| P0 | #2.1-2.3 Null derefs | Simple — add null checks |
| P1 | #3.1-3.2 Uninitialized variables | Simple — add initializers |
| P1 | #4.1-4.2 Buffer overflows | Simple — replace sprintf with snprintf |
| P1 | #5.1-5.2 Division by zero | Simple — add guards |
| P1 | #6.1-6.2 Out-of-bounds | Simple — add bounds checks |
| P2 | #7.1 Race condition in neteasemusic | Medium |
| P2 | #9.1 Debug leftover in mymathlib | Simple — remove block |
| P2 | #9.2 Missing buffer size in Q_snprintf | Simple — add sizeof |
