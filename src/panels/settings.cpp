#include "settings.h"
#include "../app.h"
#include "../app/settings-manager.h"
#include "../app/project-manager.h"
#include <filesystem>

using namespace ulvl;

void SettingsPanel::RenderPanel() {
	auto* app = Application::instance;
	auto* settingsMgr = app->getService<app::SettingsManager>();
	auto* projMgr = app->getService<app::ProjectManager>();
	auto& settings = settingsMgr->settings;

	if (!std::filesystem::exists("templates"))
		std::filesystem::create_directory("templates");

	if (projMgr->layers.size() > 0 || projMgr->projects.size() > 0)
		ImGui::BeginDisabled();

	if (ImGui::BeginCombo("Template", settings.selectedTemplateName.c_str())) {
		for (auto& path : std::filesystem::directory_iterator{ "templates" }) {
			auto temName = path.path().stem().string();

			bool selected{ temName == settings.selectedTemplateName };

			if (ImGui::Selectable(temName.c_str(), &selected)) {
				if (temName != settings.selectedTemplateName) {
					app->getService<app::ProjectManager>()->closeAll();
					auto* temMgr = app->getService<app::TemplateManager>();
					temMgr->deloadTemplates();
					temMgr->loadTemplate(temName);
					temMgr->setTemplate(temName);
					settings.selectedTemplateName = temName;
					settingsMgr->save();
				}
			}
		}

		ImGui::EndCombo();
	}

	if (projMgr->layers.size() > 0 || projMgr->projects.size() > 0) {
		ImGui::EndDisabled();
		ImGui::SetItemTooltip("Can't switch between templates while a HSON is loaded.");
	}
}
