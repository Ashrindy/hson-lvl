#pragma once
#include "../ui/panel.h"
#include "../app.h"
#include "../app/template-manager.h"

namespace ulvl {
	class SettingsPanel : public Panel {
	public:
		virtual Properties GetProperties() {
			return { .name = "Settings", .position = { 150, 100 }, .size = { 500, 350 }, .pivot{ 0, 0 }, .closeable = true };
		}

		virtual void RenderPanel() override;
	};
}
