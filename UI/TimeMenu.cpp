#include "TimeMenu.h"
#include <string>

using namespace NCL;
using namespace CSC8503;

void TimeMenu::Content() {
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

    //Draw background
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 700, main_viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(400, 100), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0);

    if (!ImGui::Begin("Time Background", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground))
    {
        return;
        ImGui::End();
    }

    //Draw menu

    std::string time = "Left Time: " + std::to_string((int)amount_time / 60) + "m "
        + std::to_string((int)amount_time % 60) + "s";
	std::string time_AI = "Cleaner active time: " + std::to_string(AI_time) + "s";
    std::string time_AI02 = "Cleaner02 active time: " + std::to_string(AI_time02) + "s";
    ImGui::TextColored(ImVec4(0.5, 0.2, 0.0, 1.0), time.c_str());
    ImGui::TextColored(ImVec4(0.5, 0.2, 0.0, 1.0), time_AI.c_str());
    ImGui::TextColored(ImVec4(0.5, 0.2, 0.0, 1.0), time_AI02.c_str());

    ImGui::End();
}

void TimeMenu::AddTime(const float& dt) {
    amount_time -= dt;
}

