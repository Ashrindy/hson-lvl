#include "glm.h"
#include <imgui.h>

bool ulvl::Editor(const char* label, glm::vec2& value) {
	bool changed{ false };
	changed |= ImGui::DragScalarN(label, ImGuiDataType_Float, &value, 2);
	return changed;
}

bool ulvl::Editor(const char* label, glm::vec3& value) {
	bool changed{ false };
	changed |= ImGui::DragScalarN(label, ImGuiDataType_Float, &value, 3);
	return changed;
}

bool ulvl::Editor(const char* label, glm::vec4& value) {
	bool changed{ false };
	changed |= ImGui::DragScalarN(label, ImGuiDataType_Float, &value, 4);
	return changed;
}

bool ulvl::Editor(const char* label, glm::quat& value) {
	bool changed{ false };
	changed |= ImGui::DragScalarN(label, ImGuiDataType_Float, &value, 4);
	return changed;
}

bool ulvl::Editor(const char* label, glm::mat4& value) {
	bool changed{ false };
	changed |= ImGui::DragScalarN(label, ImGuiDataType_Float, &value, 4*4);
	return changed;
}
