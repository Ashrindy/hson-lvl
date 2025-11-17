#include "panel.h"
#include "../app.h"

using namespace ulvl;

void Panel::Render() {
	if (!open) {
		Application::instance->removePanel(this);
		return;
	}

	Properties traits{ GetProperties() };

	ImGui::SetNextWindowPos(traits.position, ImGuiCond_FirstUseEver, traits.pivot);
	ImGui::SetNextWindowSize(traits.size, ImGuiCond_FirstUseEver);

	const char* name = traits.name;
	if (id != 0) {
		char buffer[0x100];
		snprintf(buffer, sizeof(buffer), "%s##%d", traits.name, id);
		name = buffer;
	}

	bool isOpen{ false };
	if (traits.closeable)
		isOpen = ImGui::Begin(name, &open, 0);
	else
		isOpen = ImGui::Begin(name, 0, ImGuiWindowFlags_NoCollapse);

	if (isOpen)
		RenderPanel();

	ImGui::End();
}
