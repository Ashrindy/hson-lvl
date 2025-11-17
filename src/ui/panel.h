#pragma once
#include "imgui.h"

namespace ulvl {
	class Panel {
	public:
		struct Properties {
			const char* name;
			ImVec2 position;
			ImVec2 size;
			ImVec2 pivot;
			bool closeable{ false };
		};

		bool open{ true };
		unsigned int id{ 0 };

		virtual Properties GetProperties() { return {}; }
		virtual void AddCallback() {}
		virtual void RenderPanel() {}
		void Render();
	};
}
