#include "object-list.h"
#include "../app/project-manager.h"
#include "../app/template-manager.h"
#include "../app/cleaner-service.h"
#include "../ui/editors/basic.h"
#include "../utilities/hson.h"
#include "layer-properties.h"

using namespace ulvl;

void AcceptObjServiceObjectPayload(app::ObjectService::Object* object) {
	auto* app = Application::instance;
	const ImGuiPayload* payload{ ImGui::AcceptDragDropPayload("objServiceObject") };
	if (payload && *(app::ObjectService::Object**)payload->Data) {
		auto* dropped = *(app::ObjectService::Object**)payload->Data;

		if (dropped->guid != object->hson->parentID) {
			bool containsChild{ false };

			for (auto* child : object->children)
				if (child == dropped) {
					containsChild = true;
					break;
				}

			auto* oldParent = dropped->getParent();

			if (containsChild)
				dropped->hson->parentID = object->hson->parentID;
			else
				dropped->hson->parentID = object->guid;

			object->updateChildren();
			if (object->hasParent())
				object->getParent()->updateChildren();
			if (oldParent)
				oldParent->updateChildren();
			dropped->updateModelMat();
			app->getService<app::ProjectManager>()->setUnsaved(true);
		}
	}
}

void AcceptHsonTemObjectPayload(app::ObjectService::Object* object) {
	auto* app = Application::instance;
	auto* objectService = app->getService<app::ObjectService>();
	auto* tem = app->getService<app::TemplateManager>()->currentTemplate;
	const ImGuiPayload* payload{ ImGui::AcceptDragDropPayload("hsonTemObj") };
	if (payload && *(app::Template::TreeNode**)payload->Data) {
		auto* dropped = *(app::Template::TreeNode**)payload->Data;

		auto* obj = objectService->createObject(object->owner, dropped->name, dropped->objectType);
		auto* hsonObj = obj->hson;
		hsonObj->name = hsonObj->type + std::to_string(objectService->getObjectNameId(hsonObj->type) - 1);
		hsonObj->parentID = object->guid;
		object->updateChildren();
		app->getService<app::ProjectManager>()->setUnsaved(true);
	}
}

void RenderObject(app::ObjectService::Object* object) {
	auto* app = Application::instance;
	auto* objectService = app->getService<app::ObjectService>();
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

	if (ImGui::BeginDragDropTarget()) {
		AcceptObjServiceObjectPayload(object);
		AcceptHsonTemObjectPayload(object);

		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginPopupContextItem()) {
		if (object->hson->instanceOf == hl::guid::zero() && 
			ImGui::MenuItem("Create New Instance")) 
		{
			auto* newInstance = objectService->createInstanceOf(object->owner, object->guid);
			newInstance->hson->type = object->hson->type;
			if (object->hson->name.has_value()) {
				size_t instanceIdx = 0;
				newInstance->hson->name = std::string{};
				auto& instanceName = newInstance->hson->name.value();
				instanceName = object->hson->name.value() + "_instance";

				for (auto& objec : object->owner->objects)
					if (objec.second.name.has_value()) {
						auto& existingName = objec.second.name.value();
						if (objec.first != newInstance->guid && existingName.find(instanceName) != std::string::npos)
							instanceIdx++;
					}

				instanceName += std::to_string(instanceIdx);
			}
		}

		if (ImGui::MenuItem("Remove")) {
			app->getService<app::CleanerService>()->deleteObject(object);
		}

		ImGui::EndPopup();
	}

	if (isOpen) {
		for (auto* child : object->children)
			RenderObject(child);

		ImGui::TreePop();
	}

	ImGui::PopID();
}

void AcceptHsonTemObjectPayload(app::ProjectManager::Layer* layer) {
	auto* app = Application::instance;
	auto* objectService = app->getService<app::ObjectService>();
	auto* tem = app->getService<app::TemplateManager>()->currentTemplate;
	const ImGuiPayload* payload{ ImGui::AcceptDragDropPayload("hsonTemObj") };
	if (payload && *(app::Template::TreeNode**)payload->Data) {
		auto* dropped = *(app::Template::TreeNode**)payload->Data;

		auto* obj = objectService->createObject(layer->hson, dropped->name, dropped->objectType);
		auto* hsonObj = obj->hson;
		hsonObj->name = hsonObj->type + std::to_string(objectService->getObjectNameId(hsonObj->type) - 1);
		auto* cam = app->graphics->camera;
		auto pos = cam->position + cam->front();
		obj->setPosition(pos);
		app->getService<app::ProjectManager>()->setUnsaved(true);
	}
}

void RenderLayer(app::ProjectManager::Layer* layer) {
	ImGui::PushID(layer);

	auto name = layer->getName();
	bool isOpen = ImGui::TreeNode(name.c_str());

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Save", "Ctrl+S"))
			layer->save();

		if (ImGui::MenuItem("Delete")) {
			if (std::filesystem::exists(layer->hsonPath))
				std::filesystem::remove(layer->hsonPath);

			auto* projMgr = Application::instance->getService<app::ProjectManager>();
			bool inProj{ true };
			for (auto* lay : projMgr->layers)
				if (lay == layer) {
					projMgr->closeLayer(layer);
					inProj = false;
					break;
				}

			if (inProj) {
				for (auto* proj : projMgr->projects)
					for (auto* lay : proj->layers)
						if (lay == layer) {
							proj->closeLayer(layer);
							break;
						}
			}
		}

		if (ImGui::MenuItem("Close")) {
			auto* projMgr = Application::instance->getService<app::ProjectManager>();
			bool inProj{ true };
			for (auto* lay : projMgr->layers)
				if (lay == layer) {
					projMgr->closeLayer(layer);
					inProj = false;
					break;
				}

			if (inProj) {
				for (auto* proj : projMgr->projects)
					for (auto* lay : proj->layers)
						if (lay == layer) {
							proj->closeLayer(layer);
							break;
						}
			}
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Properties", "")) {
			auto* app = Application::instance;
			unsigned int id = (unsigned int)layer;
			app->addPanel<LayerProperties>(id);
			app->getPanel<LayerProperties>(id)->layer = layer;
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginDragDropTarget()) {
		AcceptHsonTemObjectPayload(layer);

		ImGui::EndDragDropTarget();
	}

	if (isOpen) {
		size_t layerSize = layer->objects.size();
		for (size_t x = 0; x < layerSize; x++) {
			auto* object = layer->objects[x];

			if (object->hasParent()) continue;

			RenderObject(object);
		}

		ImGui::TreePop();
	}

	ImGui::PopID();
}

void RenderProject(app::ProjectManager::Project* project) {
	ImGui::PushID(project);

	bool isOpen = ImGui::TreeNode(project->folderPath.stem().string().c_str());

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Save"))
			project->save();
		if (ImGui::MenuItem("Close"))
			Application::instance->getService<app::ProjectManager>()->closeProject(project);

		ImGui::EndPopup();
	}

	if (isOpen) {
		for (auto* layer : project->layers)
			RenderLayer(layer);

		ImGui::TreePop();
	}

	ImGui::PopID();
}

void ObjectList::RenderPanel() {
	auto* app = Application::instance;

	Editor("Include Projects", includeProjects);

	ImGui::Separator();

	auto* objectSelectionMgr = app->getService<app::ObjectSelectionManager>();
	if (auto* projectManager = app->getService<app::ProjectManager>()) {
		if (includeProjects)
			for (auto* project : projectManager->projects)
				RenderProject(project);
		else
			for (auto* project : projectManager->projects)
				for (auto* layer : project->layers)
					RenderLayer(layer);

		for (auto* layer : projectManager->layers)
			RenderLayer(layer);
	}
}
