#pragma once
#include "../ui/panel.h"
#include "../ui/editors/basic.h"
#include "../ui/editors/glm.h"
#include "../app.h"
#include "../app/project-manager.h"
#include "../app/object-selection-manager.h"
#include "../app/template-manager.h"
#include <hedgelib/sets/hl_set_obj_type.h>

namespace ulvl {
	class ObjectInspector : public Panel {
	public:
		virtual Properties GetProperties() {
			return { .name = "Object Inspector", .position = { 150, 100 }, .size = { 100, 350 }, .pivot{ 0, 0 } };
		}

		bool StructEditor(hl::radix_tree<hl::hson::parameter>& parameters, hl::reflect::struct_definition& structDef);

		bool ParamEditor(const char* name, hl::hson::parameter& param, hl::reflect::field_definition& fieldDef);

		virtual void RenderPanel() {
			auto* app = Application::instance;

			auto* selected = app->getService<app::ObjectSelectionManager>()->selected;
			if (!selected) return;

			if (selected->hson->name.has_value())
				Editor("Name", selected->hson->name.value());

			Editor("Type", selected->hson->type.c_str());

			auto localPos = selected->getLocalPosition();
			if (Editor("Position", localPos))
				selected->setLocalPosition(localPos);

			auto localRot = selected->getLocalRotation();
			if (Editor("Rotation", localRot))
				selected->setLocalRotation(localRot);

			ImGui::SeparatorText("Parameters");

			auto* hsonTemplate = Application::instance->getService<app::TemplateManager>()->currentTemplate->hsonTemplate;
			StructEditor(selected->hson->parameters, hsonTemplate->structs[hsonTemplate->operator[](selected->hson->type).structType]);
		}
	};
}
