#pragma once
#include "service.h"
#include "../graphics/instancedmodel.h"
#include "../graphics/model.h"

namespace ulvl::app {
	class DebugVisualService : public Service {
	public:
		struct Model {
			int id{ 0 };
			gfx::Model* model{ nullptr };

			Model() {}
			Model(int id, gfx::Model* model) : id{ id }, model{ model } {}
		};

		struct LineDesc {
			std::vector<glm::vec3> positions{};
			glm::mat4 worldTransform{ 1 };
			int id{ 0 };
			glm::vec4 color;
		};

		gfx::InstancedModel* cube;
		gfx::InstancedModel* sphere;
		gfx::InstancedModel* cylinder;
		std::vector<Model> lines;

		virtual ~DebugVisualService() override;
		void initCube();
		void initSphere();
		void initCylinder();
		virtual void AddCallback() override;
		void addCube(gfx::InstancedMesh mesh);
		void addSphere(gfx::InstancedMesh mesh);
		void addCylinder(gfx::InstancedMesh mesh);
		void addLine(LineDesc& mesh);
		void removeMeshes(int id);
	};
}
