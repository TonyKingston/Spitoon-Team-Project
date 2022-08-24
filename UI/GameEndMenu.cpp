#include "GameEndMenu.h"
#include <string>
#ifdef _WIN64
using namespace NCL;
using namespace CSC8503;

void GameEndMenu::Content() {
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	/////////////////////////////////////
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y));
	ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, main_viewport->Size.y));

	if (!ImGui::Begin("End Background", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::End();
		return;
	}

	//Draw menu
	ImGui::SetNextWindowPos(ImVec2(main_viewport->GetCenter().x - 150, main_viewport->GetCenter().y - 100));

	ImGui::BeginChild("End Menu", ImVec2(300, 180), true, ImGuiWindowFlags_NoSavedSettings);

	ImGui::SetWindowFontScale(1.2);
	float contentWidth = ImGui::GetWindowContentRegionWidth();
	///////////
	if (win_state == RedWin) {
		std::string red = "You score: " + std::to_string(red_score);
		ImGui::Text("Player one win the game!", ImVec2(contentWidth, 50));
		ImGui::Text(red.c_str(), ImVec2(contentWidth, 50));
	}
	else if (win_state == BlueWin) {
		std::string red = "You score: " + std::to_string(blue_score);
		ImGui::Text("Player two win the game!", ImVec2(contentWidth, 50));
	}
	else {
		std::string red = "Red Player gain: " + std::to_string(red_score) + " scores";
		std::string blue = "Blue Player gain: " + std::to_string(blue_score) + " scores";
		ImGui::Text("This Round is draw!", ImVec2(contentWidth, 50));
		ImGui::Text(red.c_str(), ImVec2(contentWidth, 50));
		ImGui::Text(blue.c_str(), ImVec2(contentWidth, 50));
	}
	//////////////
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05, 0.05, 0.05, 1));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1, 0.1, 0.1, 1));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4, 0.4, 0.4, 1));
	AgainGame = ImGui::Button("Restart Game", ImVec2(contentWidth, 50));
	ImGui::PopStyleColor(3);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3, 0.05, 0.05, 1));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6, 0.1, 0.1, 1));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9, 0.1, 0.1, 1));
	QuitGame = ImGui::Button("Quit Game", ImVec2(contentWidth, 50));
	ImGui::PopStyleColor(3);

	ImGui::EndChild();

	ImGui::End();
}
#endif