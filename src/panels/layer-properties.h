#pragma once
#include "../ui/panel.h"
#include "../app.h"
#include "../app/project-manager.h"

namespace ulvl {
	class LayerProperties : public Panel {
	public:
		virtual Properties GetProperties() {
			return { .name = "Layer Properties", .position = { 150, 100 }, .size = { 500, 350 }, .pivot{ 0, 0 }, .closeable = true };
		}

		app::ProjectManager::Layer* layer{ nullptr };

		virtual void RenderPanel() override;
	};
}
