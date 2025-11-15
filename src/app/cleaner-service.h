#pragma once
#include "service.h"
#include "../graphics/model.h"

namespace ulvl::app {
	class CleanerService : public Service {
	public:
		std::vector<gfx::Model*> modelsForDeletion;

		void deleteModel(gfx::Model* model);
		void deleteAll();
	};
}
