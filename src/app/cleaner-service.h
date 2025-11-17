#pragma once
#include "service.h"
#include "applistener.h"
#include "../graphics/basemodel.h"

namespace ulvl::app {
	class CleanerService : public Service, ApplicationListener {
	public:
		std::vector<gfx::BaseModel*> modelsForDeletion;

		virtual void AddCallback() override;
		virtual void PostRenderUI() override { deleteAll(); }
		void deleteModel(gfx::BaseModel* model);
		void deleteAll();
	};
}
