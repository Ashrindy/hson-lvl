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
	char* buf = value.data();
	size_t buf_size = value.capacity() + 1;

	ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize;

	bool changed = ImGui::InputText(label, buf, buf_size, flags,
		[](ImGuiInputTextCallbackData* data) -> int {
			if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
				auto* str = static_cast<std::string*>(data->UserData);
				str->resize(data->BufTextLen);
				data->Buf = str->data();
			}
			return 0;
		},
		&value
	);

	return changed;
}

bool ulvl::EditorMultiLine(const char* label, std::string& value) {
	char* buf = value.data();
	size_t buf_size = value.capacity() + 1;

	ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize;

	bool changed = ImGui::InputTextMultiline(label, buf, buf_size, { 0, 150 }, flags,
		[](ImGuiInputTextCallbackData* data) -> int {
			if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
				auto* str = static_cast<std::string*>(data->UserData);
				str->resize(data->BufTextLen);
				data->Buf = str->data();
			}
			return 0;
		},
		&value
	);

	return changed;
}

void ulvl::Editor(const char* label, const char* value) {
	ImGui::Text("%s: %s", label, value);
}
