#pragma once
#include "service.h"
#include "../graphics/instancedmodel.h"

namespace ulvl::app {
	class DebugVisualService : public Service {
	public:
		gfx::InstancedModel* cube;

		virtual ~DebugVisualService() override;
		virtual void AddCallback() override;
		void addCube(gfx::InstancedMesh mesh);
		void removeCubes(int id);
	};
}
