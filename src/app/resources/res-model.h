#pragma once
#include "resource.h"
#include "../../graphics/instancedmodel.h"

namespace ulvl::gfx {
	struct MeshData {
		void* vertices{ nullptr };
		int vertexCount{ -1 };
		gfx::VertexInfo vertexInfo{};
		unsigned short* indices{ nullptr };
		int indexCount{ -1 };
	};

	struct ModelData {
		std::vector<MeshData> meshes{};
		std::string name{};
	};

	class ResModel : public app::Resource {
	public:
		RESOURCE(ResModel);

		InstancedModel* model{ nullptr };

		virtual void Load(void* data, size_t size) override;
		virtual void Unload() override {};
		inline void addMesh(InstancedMesh mesh) { model->addMesh(mesh); }
		inline void removeMeshes(int id) { model->removeMeshes(id); }
		inline ModelData& getModelData() { return *(ModelData*)binaryData; }
	};
}
