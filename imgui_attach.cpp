#include <metahook.h>
#include <Psapi.h>
#include "imgui_attach.h"

#include "scoreboard.h"


SDL_Window* gSDLWindow;

// To draw imgui on top of Half-Life, we take a detour from certain engine's function into HL_ImGUI_Draw function
void HL_ImGUI_Init() {
	ImGui_ImplSdl_Init(gSDLWindow);
	
	SDL_AddEventWatch(HL_ImGUI_ProcessEvent, NULL);

	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowRounding = 0.0f;
	style->ScrollbarRounding = 0.0f;

	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.86f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.60f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.39f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.16f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.63f, 0.63f, 0.63f, 0.60f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.63f, 0.63f, 0.63f, 0.71f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.51f, 0.51f, 0.51f, 0.60f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.53f, 0.51f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.53f, 0.53f, 0.53f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);

	style->WindowPadding = ImVec2(8.0f, 4.0f);
}

void HL_ImGUI_Deinit() {
	if (!gSDLWindow) {
		return;
	}

	SDL_DelEventWatch(HL_ImGUI_ProcessEvent, NULL);
}

extern cvar_t* printmodelindexes;
extern cvar_t* print_aim_entity;
extern cvar_t* print_player_info;
void HL_ImGUI_Draw() {

	ImGui_ImplSdl_NewFrame(gSDLWindow);

	m_HudCustomScoreboard.Draw(gEngfuncs.GetClientTime());

	ImGui::Render();

	SDL_GL_SwapWindow(gSDLWindow);
}

int HL_ImGUI_ProcessEvent(void* data, SDL_Event* event) {
	return ImGui_ImplSdl_ProcessEvent(event);
}