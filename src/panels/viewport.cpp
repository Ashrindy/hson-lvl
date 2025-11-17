#include "viewport.h"
#include <ImGuizmo.h>
#include <imgui_internal.h>

using namespace ulvl;

void Viewport::RenderPanel() {
	auto* gfx = gfx::Graphics::instance;

	pos = ImGui::GetCursorScreenPos();
	size = ImGui::GetContentRegionAvail();

	ImGui::Image(gfx->renderCtx.screenDs.get(), size);

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
