#pragma once
#include "../ui/panel.h"
#include "../app.h"
#include "../app/project-manager.h"
#include "../app/object-selection-manager.h"

namespace ulvl {
	class ObjectList : public Panel {
	public:
		virtual Properties GetProperties() {
			return { .name = "Object List", .position = { 150, 100 }, .size = { 100, 350 }, .pivot{ 0, 0 } };
		}

		void RenderObject(app::ObjectService::Object* object) {
			auto* app = Application::instance;
			auto* objectSelectionMgr = app->getService<app::ObjectSelectionManager>();
			ImGui::PushID(object);
			auto nameStr = object->getDisplayName();
			const char* objectName = nameStr.c_str();

			ImGuiTreeNodeFlags treeFlags{ ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick };

			if (objectSelectionMgr->selected == object)
				treeFlags |= ImGuiTreeNodeFlags_Selected;

			bool isOpen{ false };
			if (!object->hasChildren())
				ImGui::TreeNodeEx(objectName, treeFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
			else
				isOpen = ImGui::TreeNodeEx(objectName, treeFlags);

			if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
				objectSelectionMgr->select(object);

			if (ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload("objServiceObject", &object, sizeof(app::ObjectService::Object*));
				ImGui::Text("%s", objectName);

				ImGui::EndDragDropSource();
			}

			if (isOpen) {
				for (auto* child : object->children) {
					RenderObject(child);
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
		}

		virtual void RenderPanel() {
			auto* app = Application::instance;

			auto* objectSelectionMgr = app->getService<app::ObjectSelectionManager>();
			if (auto* projectManager = app->getService<app::ProjectManager>()) {
				for (auto* project : projectManager->projects) {
					if (ImGui::TreeNode(project->hsonPath.filename().string().c_str())) {

						for (auto* object : project->objects) {
							if (object->hasParent()) continue;

							RenderObject(object);
						}

						ImGui::TreePop();
					}
				}
			}
		}
	};
}
