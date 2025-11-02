#include "object-inspector.h"
#include "../utilities/hson.h"

using namespace ulvl;

bool ObjectInspector::ParamEditor(const char* name, hl::hson::parameter& param, hl::reflect::field_definition& fieldDef) {
	bool changed{ false };
	auto* app = Application::instance;
	auto* hsonTemplate = app->getService<app::TemplateManager>()->currentTemplate->hsonTemplate;
	auto* objService = app->getService<app::ObjectService>();
	const std::string& type = fieldDef.type();

	bool rendered{ false };

	if (fieldDef.is_vec2())
	{
		auto& arrayValue = param.value_array();
		glm::vec2 vec2{ (float)arrayValue[0].value_floating(), (float)arrayValue[1].value_floating() };
		if (changed |= Editor(name, vec2))
		{
			arrayValue[0].value_floating() = vec2.x;
			arrayValue[1].value_floating() = vec2.y;
		}
		rendered |= true;
	}
	else if (fieldDef.is_vec3()) {
		auto& arrayValue = param.value_array();
		glm::vec3 vec3{ (float)arrayValue[0].value_floating(), (float)arrayValue[1].value_floating(), (float)arrayValue[2].value_floating() };
		if (changed |= Editor(name, vec3))
		{
			arrayValue[0].value_floating() = vec3.x;
			arrayValue[1].value_floating() = vec3.y;
			arrayValue[2].value_floating() = vec3.z;
		}
		rendered |= true;
	}
	else if (fieldDef.is_vec4()) {
		auto& arrayValue = param.value_array();
		glm::vec4 vec4{ (float)arrayValue[0].value_floating(), (float)arrayValue[1].value_floating(), (float)arrayValue[2].value_floating(), (float)arrayValue[3].value_floating() };
		if (changed |= Editor(name, vec4))
		{
			arrayValue[0].value_floating() = vec4.x;
			arrayValue[1].value_floating() = vec4.y;
			arrayValue[2].value_floating() = vec4.z;
			arrayValue[3].value_floating() = vec4.w;
		}
		rendered |= true;
	}
	else if (fieldDef.is_quat()) {
		auto& arrayValue = param.value_array();
		glm::quat quat{ (float)arrayValue[3].value_floating(), (float)arrayValue[0].value_floating(), (float)arrayValue[1].value_floating(), (float)arrayValue[2].value_floating() };
		if (changed |= Editor(name, quat))
		{
			arrayValue[0].value_floating() = quat.x;
			arrayValue[1].value_floating() = quat.y;
			arrayValue[2].value_floating() = quat.z;
			arrayValue[3].value_floating() = quat.w;
		}
		rendered |= true;
	}
	else if (param.type() == hl::hson::parameter_type::string && !fieldDef.is_string()) {
		if (fieldDef.is_object_reference()) {
			auto& currentValue = param.value_string();

			auto* object = objService->getObject(hl::guid{ currentValue.c_str() });
			std::string previewString = "None";
			if (object)
				previewString = object->getDisplayName();

			const char* previewName = previewString.c_str();

			if (ImGui::BeginCombo(name, previewName)) {
				bool noneSelected{ strcmp(previewName, "None") == 0 };
				if (ImGui::Selectable("None", &noneSelected)) {
					param.value_string() = hl::guid{ nullptr }.as_string();
					changed |= true;
				}

				if (noneSelected)
					ImGui::SetItemDefaultFocus();
				
				for (auto* obj : objService->objects) {
					auto nameStr = obj->getDisplayName();
					bool selected{ strcmp(previewName, nameStr.c_str()) == 0 };

					if (ImGui::Selectable(nameStr.c_str(), &selected)) {
						param.value_string() = obj->guid.as_string();
						changed |= true;
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginDragDropTarget()) {
				const ImGuiPayload* payload{ ImGui::AcceptDragDropPayload("objServiceObject") };
				if (payload && *(app::ObjectService::Object**)payload->Data) {
					auto* dropped = *(app::ObjectService::Object**)payload->Data;
					param.value_string() = dropped->guid.as_string();
				}

				ImGui::EndDragDropTarget();
			}

			rendered |= true;
		}
		else {
			auto& enumDef = hsonTemplate->enums[fieldDef.type()];
			auto& currentValue = param.value_string();

			if (ImGui::BeginCombo(name, currentValue.c_str())) {
				for (auto value : enumDef.values) {
					bool selected{ strcmp(value.first, currentValue.c_str()) == 0 };

					if (ImGui::Selectable(value.first, &selected))
						currentValue = value.first;
				}

				ImGui::EndCombo();
			}

			rendered |= true;
		}
	}

	if (rendered) return changed;
	
	switch (param.type()) {
	case hl::hson::parameter_type::boolean:
		changed |= Editor(name, param.value_bool());
		break;
	case hl::hson::parameter_type::signed_integer:
		changed |= Editor(name, param.value_int());
		break;
	case hl::hson::parameter_type::unsigned_integer:
		changed |= Editor(name, param.value_uint());
		break;
	case hl::hson::parameter_type::floating:
		changed |= Editor(name, param.value_floating());
		break;
	case hl::hson::parameter_type::string:
		changed |= Editor(name, param.value_string());
		break;
	case hl::hson::parameter_type::array: {
		ImGui::BeginGroup();

		bool isOpen{ ImGui::TreeNode(name) };

		if (fieldDef.array_count() == 0 && ImGui::BeginPopupContextItem("Parameter array menu")) {
			if (ImGui::MenuItem("Add"))
				param.value_array().push_back(ut::createParameterByType(fieldDef.subtype().c_str()));

			if (ImGui::MenuItem("Clear", "", nullptr, param.value_array().size() > 0))
				param.value_array().clear();

			ImGui::EndPopup();
		}

		if (isOpen) {
			int removeIndex{ -1 };
			size_t m{ 0 };
			for (auto& parameter : param.value_array()) {
				ImGui::PushID(m);
				hl::reflect::field_definition aFieldDef{};
				aFieldDef.set_type(fieldDef.subtype());
				ImGui::BeginGroup();
				changed |= ParamEditor("##", parameter, aFieldDef);
				ImGui::EndGroup();

				if (fieldDef.array_count() == 0 && ImGui::BeginPopupContextItem("Parameter array item menu")) {
					if (ImGui::MenuItem("Remove"))
						removeIndex = m;

					ImGui::EndPopup();
				}

				ImGui::PopID();
				m++;
			}
			if (removeIndex != -1)
				param.value_array().erase(param.value_array().begin() + removeIndex);
			ImGui::TreePop();
		}
		ImGui::EndGroup();
		break;
	}
	case hl::hson::parameter_type::object:
		if (ImGui::TreeNode(name)) {
			changed |= StructEditor(param.value_object(), hsonTemplate->structs[fieldDef.type()]);
			ImGui::TreePop();
		}
		break;
	}

	if (ImGui::BeginItemTooltip()) {
		for (auto desc : fieldDef.descriptions) {
			ImGui::Text("%s: %s", desc.first, desc.second);
		}

		ImGui::EndTooltip();
	}

	return changed;
}

bool ObjectInspector::StructEditor(hl::radix_tree<hl::hson::parameter>& parameters, hl::reflect::struct_definition& structDef) {
	bool changed{ false };
	auto* hsonTemplate = Application::instance->getService<app::TemplateManager>()->currentTemplate->hsonTemplate;

	for (auto parameter : parameters) {
		for (auto& field : structDef.fields)
			if (field.name == parameter.first) {
				changed |= ParamEditor(parameter.first, parameter.second, field);
				break;
			}
	}
	return changed;
}
