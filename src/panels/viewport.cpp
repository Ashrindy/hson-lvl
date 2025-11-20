#include "viewport.h"
#include "../app/template-manager.h"
#include "../app/project-manager.h"
#include <ImGuizmo.h>
#include <imgui_internal.h>

using namespace ulvl;

void AcceptHsonTemObjectPayload() {
	const ImGuiPayload* payload{ ImGui::AcceptDragDropPayload("hsonTemObj") };
	if (payload && *(app::Template::TreeNode**)payload->Data) {
		auto* app = Application::instance;
		auto* objectService = app->getService<app::ObjectService>();
		auto* tem = app->getService<app::TemplateManager>()->currentTemplate;
		auto* objSelectService = app->getService<app::ObjectSelectionManager>();
		auto* projMgr = app->getService<app::ProjectManager>();

		auto* dropped = *(app::Template::TreeNode**)payload->Data;

		if (projMgr->layers.size() == 0 && projMgr->projects.size() == 0) return;

		hl::hson::project* owner{ nullptr };

		if (objSelectService->selected) {
			owner = objSelectService->selected->owner;
		}
		else if (projMgr->projects.size() > 0) {
			if (projMgr->projects[0]->layers.size() > 0)
				owner = projMgr->projects[0]->layers[0]->hson;
		}
		else if (projMgr->layers.size() > 0)
			owner = projMgr->layers[0]->hson;

		if (!owner) return;

		auto* obj = objectService->createObject(owner, dropped->name, dropped->objectType);
		auto* hsonObj = obj->hson;
		hsonObj->name = hsonObj->type + std::to_string(objectService->getObjectNameId(hsonObj->type) - 1);
		auto* cam = app->graphics->camera;
		auto pos = cam->position + cam->front();
		obj->setPosition(pos);
		app->getService<app::ProjectManager>()->setUnsaved(true);
	}
}

void Viewport::RenderPanel() {
	auto* gfx = gfx::Graphics::instance;

	pos = ImGui::GetCursorScreenPos();
	size = ImGui::GetContentRegionAvail();

	ImGui::Image(gfx->renderCtx.screenDs.get(), size);
	if (ImGui::BeginDragDropTarget()) {
		AcceptHsonTemObjectPayload();
		ImGui::EndDragDropTarget();
	}

	ImGuizmo::BeginFrame();
	ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
	ImGuizmo::SetRect(pos.x, pos.y, (float)size.x, (float)size.y);
	ImGuizmo::SetAlternativeWindow(ImGui::GetCurrentWindow());
	Application::instance->getService<app::ObjectSelectionManager>()->renderGizmo();


	isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
}

void Viewport::PostRenderUI() {
	auto* gfx = gfx::Graphics::instance;

	if (pos.x != gfx->screenPosX || pos.y != gfx->screenPosY)
		gfx->reposScreen(pos.x, pos.y);

	if (size.x != gfx->screenWidth || size.y != gfx->screenHeight)
		gfx->resizeScreen(size.x, size.y);
}
