#pragma once
#include "../math/aabb.h"
#include "basemodel.h"

namespace ulvl::gfx {
	struct Mesh {
		int indexOffset;
		int indexCount;

		Mesh(int indexOffset, int indexCount) : indexOffset{ indexOffset }, indexCount{ indexCount } {}
	};

	class Model : public BaseModel {
	public:
		glm::vec3 position{ 0, 0, 0 };
		glm::quat rotation{ 1, 0, 0, 0 };
		glm::vec3 scale{ 1, 1, 1 };

	private:
		glm::mat4 worldMatrix{};
		std::vector<Mesh> meshes;

		void updateWorldMatrix();
	public:
		Model(ModelDesc desc = {});

		virtual void init(ModelDesc desc) override;
		virtual void shutdown() override;

		void setPosition(const glm::vec3& pos);
		void setRotation(const glm::quat& rot);
		void setRotation(const glm::vec3& rot);
		void setScale(const glm::vec3& scale);
		void setWorldMatrix(const glm::mat4& mat);

		void addMesh(void* vertices, unsigned int vcount, unsigned short* indices, unsigned int icount);
		void clearMeshes();
		inline const glm::mat4& getWorldMatrix() const { return worldMatrix; }

		virtual void render() override;
	};
}
