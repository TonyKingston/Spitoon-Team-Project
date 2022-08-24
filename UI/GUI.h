#pragma once
#ifdef _WIN64
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_win32.h"

#include <vector>
#include <memory>

namespace NCL {
	namespace CSC8503 {
		class Menu {
		public:
			Menu() = default;
			~Menu() = default;

			virtual void Content() = 0;
		};

		class D_GUI {
			class Win32Window;
		public:
			D_GUI() :init_mark(false) {  }
			~D_GUI();

			void Init();
			void UI_Frame();
			void UI_Render();

			void AddMenu(const std::shared_ptr<Menu> &m);
			void RemoveMenu(const std::shared_ptr<Menu> &m);

			bool MenuEmpty() { return menus.empty(); }

			bool Win32Mark() const{ return init_mark; }

			ImFont* dFont;
		
		private:
			bool init_mark;

			std::vector<std::shared_ptr<Menu>> menus;
			void DrawContent();
		};
	}
}
#endif