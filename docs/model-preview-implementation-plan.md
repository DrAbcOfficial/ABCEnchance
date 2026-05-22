# Model Preview Implementation Plan

## Overview
The `ModelViewPanel` class at `vgui_controls/ModelViewPanel.cpp` has full infrastructure but the core rendering is stubbed out. This plan describes how to implement it using the MetaRenderer API.

---

## Current State

### What Works:
- **FBO Creation** (`ApplySettings()`, lines 264-318): Correctly creates FBO with color + depth-stencil textures.
- **Model Loading** (`ChangeModel()`): Uses `gEngineStudio.Mod_ForName()` — correct.
- **Entity Setup**: Allocates `cl_entity_s` and sets model, origin, angles, sequence, skin, etc.
- **Transform Methods**: `SetModelPos`, `SetModelRotate`, `SetFOV`, etc. all implemented.

### What's Broken:
- **`Paint()`** (lines 238-261): Entire rendering logic is commented out.
- **Light methods** (lines 219-233): `SetAmbientLight`, `SetShadeLight`, `SetLightColor`, `SetLightOrigin` are empty stubs.
- **Bug**: `SetModelRotate()` writes yaw to `origin[YAW]` instead of `angles[YAW]` (line 68).

---

## Implementation Plan

### Step 1: Fix `SetModelRotate()` Bug
**File:** `vgui_controls/ModelViewPanel.cpp:68`
**Fix:** Change `m_pModelEntity->origin[YAW] = yaw;` to `m_pModelEntity->angles[YAW] = yaw;`

### Step 2: Understand the Renderer API
The MetaRenderer API (`IMetaRenderer` in `thirdparty/MetaHookSv/include/Interface/IMetaRenderer.h`) provides:

| Method | Purpose |
|--------|---------|
| `BindFrameBuffer(FBO_Container_t*)` | Bind FBO for rendering |
| `SetCurrentSceneFBO(FBO_Container_t*)` | Set scene FBO |
| `GetCurrentRenderingFBO()` | Get active rendering FBO |
| `SetViewport(x, y, w, h)` | Set GL viewport |
| `SetCurrentEntity(cl_entity_t*)` | Set entity to render |
| `DrawCurrentEntity(bool transparent)` | Draw studio model |
| `ClearFBO(bool color, bool depth, bool stencil)` | Clear FBO |
| `PushRefDef()`, `PopRefDef()` | Push/pop refdef state |
| `SetRefDefViewOrigin(vec3_t)`, `SetRefDefViewAngles(vec3_t)` | Set view position |
| `SetupPerspective(fov, aspect, near, far)` | Perspective projection |
| `SetupFrustumProjectionMatrix()` | Frustum projection |
| `PushWorldMatrix()`, `PopWorldMatrix()` | Matrix stack |
| `LoadIdentityForWorldMatrix()`, `TranslateWorldMatrix()`, `RotateWorldMatrix()` | Transform operations |

### Step 3: Implement `Paint()` Using GaussianBlurPanel as Reference
The `GaussianBlurPanel.cpp` demonstrates the working FBO rendering pattern. Key steps:

```cpp
void ModelViewPanel::Paint() {
    IMetaRenderer* pRenderer = MetaRenderer();
    if (!pRenderer) return;
    
    // 1. Save current state
    pRenderer->BeginDebugGroup("ModelViewPanel::Paint");
    auto* oldSceneFBO = pRenderer->GetCurrentSceneFBO();
    auto* oldRenderingFBO = pRenderer->GetCurrentRenderingFBO();
    int oldW = oldRenderingFBO ? oldRenderingFBO->iWidth : glwidth;
    int oldH = oldRenderingFBO ? oldRenderingFBO->iHeight : glheight;
    
    // 2. Bind model FBO and clear it
    pRenderer->BindFrameBuffer(&m_ModelFBO);
    pRenderer->SetCurrentSceneFBO(&m_ModelFBO);
    pRenderer->SetViewport(0, 0, m_ModelFBO.iWidth, m_ModelFBO.iHeight);
    pRenderer->ClearFBO(true, true, true);
    
    // 3. Setup view/projection for model preview
    pRenderer->PushRefDef();
    pRenderer->SetRefDefViewOrigin(m_pModelEntity->origin);
    pRenderer->SetRefDefViewAngles(m_pModelEntity->angles);
    pRenderer->SetupPerspective(m_flFov, 
        (float)m_ModelFBO.iWidth / (float)m_ModelFBO.iHeight, 
        0.1f, 8192.0f);
    pRenderer->SetupFrustumProjectionMatrix();
    
    // 4. Render the model
    pRenderer->SetCurrentEntity(m_pModelEntity);
    pRenderer->DrawCurrentEntity(false);  // opaque pass
    // If model has transparency:
    // pRenderer->DrawCurrentEntity(true);  // transparent pass
    
    // 5. Restore state
    pRenderer->PopRefDef();
    pRenderer->SetCurrentSceneFBO(oldSceneFBO);
    pRenderer->BindFrameBuffer(oldRenderingFBO);
    pRenderer->SetViewport(0, 0, oldW, oldH);
    
    // 6. Draw the FBO texture to the panel
    vgui::surface()->DrawSetTexture(m_ModelFBO.s_hBackBufferTex);
    vgui::surface()->DrawFilledRect(0, 0, m_ModelFBO.iWidth, m_ModelFBO.iHeight);
    
    pRenderer->EndDebugGroup();
}
```

### Step 4: Implement Lighting Methods
```cpp
void ModelViewPanel::SetAmbientLight(int light) {
    if (m_pModelEntity) {
        m_pModelEntity->curstate.rendermode = kRenderNormal;
    }
}

void ModelViewPanel::SetLightColor(int r, int g, int b) {
    // Store light color for rendering
    // The entity renderer uses engine lighting by default
    // Could use rendercolor for tint effect
}

void ModelViewPanel::SetShadeLight(int light) { /* stored for later */ }
void ModelViewPanel::SetLightOrigin(float x, float y, float z) { /* stored for later */ }
```

**Note:** The MetaRenderer's `DrawCurrentEntity` uses the engine's `R_DrawCurrentEntity`, which handles lighting internally. The light methods may need to be integrated with the entity's `curstate.rendercolor` and `curstate.renderamt`.

### Step 5: Handle Panel Resizing
The FBO needs to be resized when the panel size changes. Override `OnSize()` or check dimensions in `Paint()` and recreate the FBO if needed.

---

## Files to Modify

| File | Changes |
|------|---------|
| `vgui_controls/ModelViewPanel.cpp` | Implement `Paint()`, fix `SetModelRotate()`, implement light methods |
| `vgui_controls/ModelViewPanel.h` | Add FBO resize method, light state members |

---

## Testing

1. **Build**: Ensure Release and Debug configs compile.
2. **Verify ModelViewPanel renders** in the Options menu (Model viewer in advanced multiplayer settings).
3. **Test rotation slider**: Verify smooth rotation.
4. **Test model switching**: Change model and verify display updates.
5. **Test different models**: Player models, weapon models, etc.

---

## Fallback / Alternative Approach

If `DrawCurrentEntity` doesn't work correctly in this context (e.g., the engine expects certain global state), an alternative approach is:
1. Use `RenderScene()` instead (renders entire scene into the FBO).
2. This requires a more complex setup with a proper refdef and entity list.

However, the commented-out code suggests the author intended to use `DrawCurrentEntity`, which is the simpler approach and should work if properly set up.

---

## Priority: P0 (Feature-complete)

This implementation is critical because:
- The Consumer (`OptionAdvancedDlg.cpp`) actively calls `ModelViewPanel` methods expecting visible output.
- The model viewer is a user-facing feature in the game options.
- All infrastructure is already in place; only the rendering loop needs to be written.
