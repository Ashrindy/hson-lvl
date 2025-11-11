#pragma once
#include "../ui/panel.h"
#include "../graphics/graphics.h"
#include "../app.h"
#include "../app/object-selection-manager.h"

namespace ulvl {
	class Viewport : public Panel {
	public:
		virtual Properties GetProperties() {
			return { .name = "Viewport", .position = { 150, 100 }, .size = { 500, 350 }, .pivot{ 0, 0 } };
		}

		ImVec2 pos{};
		ImVec2 size{};
		bool isHovered{ false };

		virtual void RenderPanel() override;
		void AfterRender();
	};
}
