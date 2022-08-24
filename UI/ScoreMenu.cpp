#include "ScoreMenu.h"
#include <string>

using namespace NCL;
using namespace CSC8503;

void ScoreMenu::Content() {
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

	//Draw background
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 100, main_viewport->WorkPos.y));
	ImGui::SetNextWindowSize(ImVec2(400, 50), ImGuiCond_Always); 
	ImGui::SetNextWindowBgAlpha(0);

	if (!ImGui::Begin("Red Score Background", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground))
	{
		ImGui::End();
		return;
	}

	//Draw menu
	std::string player_one_string = "player one score: " + std::to_string(red_score);
	
	ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), player_one_string.c_str());

	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 1600, main_viewport->WorkPos.y));
	ImGui::SetNextWindowSize(ImVec2(400, 50), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0);

	if (!ImGui::Begin("Blue Score Background", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground))
	{
		ImGui::End();
		return;
	}

	//Draw menu
	std::string player_two_string = "player two score: " + std::to_string(blue_score);

	ImGui::TextColored(ImVec4(0.0, 0.0, 1.0, 1.0), player_two_string.c_str());

	ImGui::End();
}
