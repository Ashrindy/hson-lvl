#include "basic.h"
#include <imgui.h>

bool ulvl::Editor(const char* label, bool& value) {
	bool changed{ false };
	changed |= ImGui::Checkbox(label, &value);
	return changed;
}

bool ulvl::Editor(const char* label, unsigned int& value) {
	bool changed{ false };
	changed |= ImGui::DragScalar(label, ImGuiDataType_U32, &value);
	return changed;
}

bool ulvl::Editor(const char* label, int& value) {
	bool changed{ false };
	changed |= ImGui::DragInt(label, &value);
	return changed;
}

bool ulvl::Editor(const char* label, unsigned long long& value) {
	bool changed{ false };
	changed |= ImGui::DragScalar(label, ImGuiDataType_U64, &value);
	return changed;
}

bool ulvl::Editor(const char* label, long long& value) {
	bool changed{ false };
	changed |= ImGui::DragScalar(label, ImGuiDataType_S64, &value);
	return changed;
}

bool ulvl::Editor(const char* label, float& value) {
	bool changed{ false };
	changed |= ImGui::DragFloat(label, &value);
	return changed;
}

bool ulvl::Editor(const char* label, double& value) {
	bool changed{ false };
	changed |= ImGui::DragScalar(label, ImGuiDataType_Double, &value);
	return changed;
}

bool ulvl::Editor(const char* label, std::string& value) {
	bool changed{ false };

	std::vector<char> buf{ value.begin(), value.end() };
	buf.push_back('\0');

	changed |= ImGui::InputText(label, buf.data(), buf.size(), ImGuiInputTextFlags_CallbackResize,
		[](ImGuiInputTextCallbackData* data) -> int {
			if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
				auto* str = (std::string*)data->UserData;
				str->resize(data->BufTextLen);
				data->Buf = str->data();
			}
			return 0;
		},
		&value);

	if (changed)
		value = buf.data();

	return changed;
}

void ulvl::Editor(const char* label, const char* value) {
	ImGui::Text("%s: %s", label, value);
}
