#pragma once
#include "basemodel.h"

namespace ulvl::gfx {
	struct InstancedMesh {
		glm::mat4 worldMatrix;
		glm::vec4 color;
		int id{ 0 };

		InstancedMesh();

		inline bool operator==(const InstancedMesh& rhs) const { return id == rhs.id; }
		inline bool operator!=(const InstancedMesh& rhs) const { return !(*this == rhs); }

		void setPosition(const glm::vec3& pos);
	};

	struct PosVertex {
		glm::vec3 pos;
	};

	class InstancedModel : public BaseModel {
		std::vector<InstancedMesh> meshes;
		std::unique_ptr<plume::RenderBuffer> instanceBuffer;
	public:
		InstancedModel();

		void updateInstanceBuffer();
		void setModel(void* vertices, unsigned int vcount, unsigned short* indices, unsigned int icount);
		void addMesh(InstancedMesh mesh);
		void removeMeshes(int id);
		void clearMeshes();

		virtual void init() override;
		virtual void render() override;
		virtual void shutdown() override;
	};
}
