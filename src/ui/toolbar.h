#pragma once
#include <imgui.h>
#include "../app.h"
#include "../app/object-selection-manager.h"
#include "../app/project-manager.h"

namespace ulvl {
	void ToolBar() {
		auto* app = Application::instance;
		auto* projectMgr = app->getService<app::ProjectManager>();
		auto* objSelectMgr = app->getService<app::ObjectSelectionManager>();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New", "Ctrl+N")) {}
				if (ImGui::MenuItem("Open", "Ctrl+O")) {}
				if (ImGui::MenuItem("Save", "Ctrl+S")) objSelectMgr->save();
				if (ImGui::MenuItem("Save As", "")) {}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}
