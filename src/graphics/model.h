#pragma once
#include "../math/aabb.h"
#include "basemodel.h"

namespace ulvl::gfx {
	struct Mesh {
		int indexOffset;
		int indexCount;
		void* texture;

		Mesh(int indexOffset, int indexCount, void* texture) : indexOffset{ indexOffset }, indexCount{ indexCount }, texture{ texture } {}
	};

	class Model : public BaseModel {
	public:
		glm::vec3 position{ 0, 0, 0 };
		glm::quat rotation{ 1, 0, 0, 0 };
		glm::vec3 scale{ 1, 1, 1 };
		math::Aabb aabb{};

	private:
		glm::mat4 worldMatrix{};
		std::vector<Mesh> meshes;

		void updateWorldMatrix();
		void updateAabb();
	public:
		Model();

		virtual void init() override;
		virtual void shutdown() override;

		void setPosition(const glm::vec3& pos);
		void setRotation(const glm::quat& rot);
		void setRotation(const glm::vec3& rot);
		void setScale(const glm::vec3& scale);
		void setWorldMatrix(const glm::mat4& mat);

		void addMesh(void* vertices, unsigned int vcount, unsigned short* indices, unsigned int icount, void* texture);
		void clearMeshes();
		inline const glm::mat4& getWorldMatrix() const { return worldMatrix; }

		virtual void render() override;
	};
}
