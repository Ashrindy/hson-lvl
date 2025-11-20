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

		void RenderObjectNode(app::Template::TreeNode* node);
		void RenderCategoryNode(app::Template::TreeNode* node);

		void RenderTreeNode(app::Template::TreeNode* node) {
			if (node->type == app::Template::TreeNode::NodeType::OBJECT)
				RenderObjectNode(node);
			else
				RenderCategoryNode(node);
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
