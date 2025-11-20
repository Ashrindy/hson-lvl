#include "object-library.h"

using namespace ulvl;

void ObjectLibrary::RenderObjectNode(app::Template::TreeNode* node) {
	ImGui::TreeNodeEx(node->name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);

	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("hsonTemObj", &node, sizeof(app::Template::TreeNode*));
		ImGui::Text("%s", node->name.c_str());

		ImGui::EndDragDropSource();
	}
}

void ObjectLibrary::RenderCategoryNode(app::Template::TreeNode* node) {
	if (ImGui::TreeNode(node->name.c_str())) {
		for (auto* child : node->childNodes)
			RenderTreeNode(child);
		ImGui::TreePop();
	}
}
