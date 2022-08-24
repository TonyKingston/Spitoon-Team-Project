#include "StartMenu.h"

using namespace NCL;
using namespace CSC8503;

void StartMenu::Content() {
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

    //Draw background
    ImGui::SetNextWindowPos(ImVec2(main_viewport->GetCenter().x -305, main_viewport->GetCenter().y-105));
    ImGui::SetNextWindowSize(ImVec2(600, 500));
    ImGui::SetNextWindowBgAlpha(0);

    if (!ImGui::Begin("Enter Background", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground))
    {
        ImGui::End();
        return;
    }

    //Draw menu
    ImGui::SetNextWindowPos(ImVec2(main_viewport->GetCenter().x - 305, main_viewport->GetCenter().y - 100));

    ImGui::BeginChild("Enter Menu", ImVec2(600, 500), true, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);

    ImGui::SetWindowFontScale(4.0);
    float contentWidth = ImGui::GetWindowContentRegionWidth();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05, 0.05, 0.05, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6, 0.1, 0.1, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9, 0.4, 0.4, 1));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(155, 46, 168, 255));
    EnterGame = ImGui::Button("Enter Game", ImVec2(contentWidth, 90));
    ImGui::PopStyleColor(4);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05, 0.05, 0.05, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6, 0.1, 0.1, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9, 0.4, 0.4, 1));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
    NetworkGame = ImGui::Button("Network Game", ImVec2(contentWidth, 90));
    ImGui::PopStyleColor(4);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3, 0.05, 0.05, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6, 0.1, 0.1, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9, 0.1, 0.1, 1));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
    QuitGame = ImGui::Button("Quit Game", ImVec2(contentWidth, 90));
    ImGui::PopStyleColor(4);

    ImGui::EndChild();

    ImGui::End();
}