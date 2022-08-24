#include "PauseMenu.h"
#include "CourseworkGame.h"

using namespace NCL;
using namespace CSC8503;
#ifdef _WIN64
void PauseMenu::Content() {
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

    //Draw background
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, main_viewport->Size.y));

    if (!ImGui::Begin("Pause Background", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::End();
        return;
    }

    //Draw menu
    ImGui::SetNextWindowPos(ImVec2(main_viewport->GetCenter().x - 150, main_viewport->GetCenter().y - 100));

    ImGui::BeginChild("Pause Menu", ImVec2(300, 190), true, ImGuiWindowFlags_NoSavedSettings);

    ImGui::SetWindowFontScale(1.2);
    float contentWidth = ImGui::GetWindowContentRegionWidth();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05, 0.05, 0.05, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1, 0.1, 0.1, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4, 0.4, 0.4, 1));
    ResumeGame = ImGui::Button("Resume Game", ImVec2(contentWidth, 50));
    ImGui::PopStyleColor(3);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3, 0.05, 0.05, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6, 0.1, 0.1, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9, 0.1, 0.1, 1));
    QuitGame = ImGui::Button("Quit Game", ImVec2(contentWidth, 50));
    ImGui::PopStyleColor(3);

    ShowConfiguration();

    ImGui::EndChild();

    ImGui::End();
}

void PauseMenu::ShowConfiguration() {
    static bool full_screen = Window::GetWindow()->IsFullScreen();
	if (ImGui::Checkbox("Full screen", &full_screen)) {
		Window::GetWindow()->SetFullScreen(full_screen);
//        game->renderer->ResizeSceneTextures(0, 0);
	}

    static bool DebugMode = game->debug_mode;
    if (ImGui::Checkbox("Debug mode", &DebugMode)) {
        game->debug_mode = !game->debug_mode;
    }
}
#endif _WIN64
