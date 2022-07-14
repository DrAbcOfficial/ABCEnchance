#pragma once
#include "SDL.h"
#include "SDL_opengl.h"
#include "imgui\imgui.h"
#include "imgui_impl_sdl.h"

void HL_ImGUI_Init();
void HL_ImGUI_Deinit();
void HL_ImGUI_Draw();
int HL_ImGUI_ProcessEvent(void* data, SDL_Event* event);

extern SDL_Window* gSDLWindow;