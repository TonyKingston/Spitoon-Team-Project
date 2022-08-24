#include "GUI.h"
#include "../../Common/Window.h"
#include "../../Common/Win32Window.h"

#include <GLFW/glfw3.h>


using namespace NCL;
using namespace CSC8503;

Win32Code::ExInputResult ImguiProcessInput(void* data);

void D_GUI::Init() {
#ifdef _WIN32
    Win32Code::Win32Window* window32 = (Win32Code::Win32Window*)Window::GetWindow();
    window32->SetExtraMsgFunc(ImguiProcessInput);

    init_mark = true;
#endif
    if (!init_mark)
        return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    dFont = io.Fonts->AddFontFromFileTTF("../../Assets/Fonts/GrinchedRegular.ttf", 20);

    ImGui_ImplWin32_Init(window32->GetHandle());
    ImGui_ImplOpenGL3_Init("#version 400");

    ImGui::StyleColorsClassic();
}

D_GUI::~D_GUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void D_GUI::UI_Frame() {
    if (!init_mark)
        return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    DrawContent();
}

void D_GUI::UI_Render() {
    if (!init_mark)
        return;
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void D_GUI::DrawContent() {
    for (auto& m : menus) {
        m->Content();
    }
}


Win32Code::ExInputResult ImguiProcessInput(void* data)
{
    ImGuiIO& io = ImGui::GetIO();
    RAWINPUT* rawInput = (RAWINPUT*)data;

    if (rawInput->header.dwType == RIM_TYPEMOUSE)
    {
        static int btDowns[5] = { RI_MOUSE_BUTTON_1_DOWN,
                                  RI_MOUSE_BUTTON_2_DOWN,
                                  RI_MOUSE_BUTTON_3_DOWN,
                                  RI_MOUSE_BUTTON_4_DOWN,
                                  RI_MOUSE_BUTTON_5_DOWN };

        static int btUps[5] = { RI_MOUSE_BUTTON_1_UP,
                                RI_MOUSE_BUTTON_2_UP,
                                RI_MOUSE_BUTTON_3_UP,
                                RI_MOUSE_BUTTON_4_UP,
                                RI_MOUSE_BUTTON_5_UP };

        for (int i = 0; i < 5; ++i) {
            if (rawInput->data.mouse.usButtonFlags & btDowns[i]) {
                io.MouseDown[i] = true;
            }
            else if (rawInput->data.mouse.usButtonFlags & btUps[i]) {
                io.MouseDown[i] = false;
            }
        }
    }
    else if (rawInput->header.dwType == RIM_TYPEKEYBOARD)
    {
        USHORT key = rawInput->data.keyboard.VKey;
        bool down = !(rawInput->data.keyboard.Flags & RI_KEY_BREAK);

        if (key < 256)
            io.KeysDown[key] = down;
        if (key == VK_CONTROL)
            io.KeyCtrl = down;
        if (key == VK_SHIFT)
            io.KeyShift = down;
        if (key == VK_MENU)
            io.KeyAlt = down;

    }
    return { false, io.WantCaptureKeyboard };
}

void D_GUI::AddMenu(const std::shared_ptr<Menu>& m) {
    menus.push_back(m);
}

void D_GUI::RemoveMenu(const std::shared_ptr<Menu>& m) {
    auto iter = std::find(menus.begin(), menus.end(), m);
    if (iter != menus.end())
       menus.erase(iter);
}