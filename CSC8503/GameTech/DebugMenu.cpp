#include "DebugMenu.h"
#ifdef _WIN64
#include "CourseworkGame.h"
#include "imgui_internal.h"
#include <string>

#include <windows.h>
#include <Psapi.h>

using namespace NCL;
using namespace CSC8503;

void DebugMenu::Content()
{
	if (game->debug_mode) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + 800), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(360, 280), ImGuiCond_Always);

		if (!ImGui::Begin("Debug Menu", NULL, window_flags))
		{
			// Early out if the window is collapsed, as an optimization.
			ImGui::End();
			return;
		}

		ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
		game->GetUI()->dFont->Scale = 0.7;

		ImGui::Text("F5: Free Camera Mode");
		ImGui::Separator();

		ImGui::Text("F6: Show NavMesh");
		ImGui::Separator();

		ShowFPS();
		ImGui::Separator();

		ShowMemory();
		ImGui::Separator();

		ImGui::End();
	}
}

void DebugMenu::ShowFPS() {
	std::string FPS = "FPS: " + std::to_string(1 / Window::GetTimer()->GetTimeDeltaSeconds());
	ImGui::BulletText(FPS.c_str());
}

void DebugMenu::ShowMemory() {
	PROCESS_MEMORY_COUNTERS_EX pmi = { 0 };
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmi, sizeof(pmi));
	std::string Memory = "MemoryFootprint: " + std::to_string(pmi.WorkingSetSize/1000000) + " MB";
	ImGui::BulletText(Memory.c_str());
}
#endif _WIN64