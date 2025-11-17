#include "layer-properties.h"
#include "../ui/editors/basic.h"

using namespace ulvl;

void LayerProperties::RenderPanel() {
	if (!layer) return;

	ImGui::SeparatorText("Metadata");

	auto& metadata = layer->hson->metadata;

	Editor("Name", metadata.name);
	Editor("Author(s)", metadata.author);
	Editor("Version", metadata.version);
	EditorMultiLine("Description", metadata.description);

	if (metadata.date != "")
		ImGui::Text("Last saved: %s", metadata.date.c_str());
}
