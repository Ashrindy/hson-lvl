#pragma once
#include <hedgelib/sets/hl_set_obj_type.h>
#include "squirrelwrap.h"

namespace ulvl::app {
	struct Template {
		struct TreeNode {
			enum class NodeType {
				CATEGORY,
				OBJECT
			};

			const char* name;
			NodeType type;
			hl::set_object_type* objectType;
			std::vector<TreeNode*> childNodes;

			~TreeNode() { delete name; }
		};

		struct Tree {
			std::vector<TreeNode*> tree;

			void generateTree(hl::set_object_type_database* hsonTemplate);
			TreeNode* getNode(const char* name, const std::vector<TreeNode*>& tree);
			TreeNode* getNode(const char* name);
			TreeNode* getNodeByPath(const char* path);
			TreeNode* createNode(const char* path);
		};

		const char* name;
		hl::set_object_type_database* hsonTemplate;
		Tree objectTree{};
		SquirrelWrap squirrelWrap{};

		Template(const char* templateName);
		ModelData getModelData(ObjectService::Object* obj);
	};
}
