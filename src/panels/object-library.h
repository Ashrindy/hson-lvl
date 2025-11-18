#pragma once
#include "../ui/panel.h"
#include "../app.h"
#include "../app/template-manager.h"

namespace ulvl {
	class ObjectLibrary : public Panel {
	public:
		virtual Properties GetProperties() {
			return { .name = "Object Library", .position = { 150, 100 }, .size = { 100, 350 }, .pivot{ 0, 0 } };
		}

		void RenderTreeNode(app::Template::TreeNode* node) {
			ImGuiTreeNodeFlags flags{};
			if (node->type == app::Template::TreeNode::NodeType::OBJECT)
				flags |= ImGuiTreeNodeFlags_Leaf;

			if (ImGui::TreeNodeEx(node->name.c_str(), flags)) {
				if (node->type == app::Template::TreeNode::NodeType::CATEGORY)
					for (auto* nodes : node->childNodes)
						RenderTreeNode(nodes);

				ImGui::TreePop();
			}
		}

		virtual void RenderPanel() {
			auto* app = Application::instance;
			auto* tem = app->getService<app::TemplateManager>()->currentTemplate;

			if (!tem) return;

			for (auto* node : tem->objectTree.tree)
				RenderTreeNode(node);
		}
	};
}
