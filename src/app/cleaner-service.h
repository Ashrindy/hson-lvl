#pragma once
#include "service.h"
#include "applistener.h"
#include "object-service.h"
#include "../graphics/basemodel.h"

namespace ulvl::app {
	class CleanerService : public Service, ApplicationListener {
	public:
		std::vector<gfx::BaseModel*> modelsForDeletion;
		std::vector<ObjectService::Object*> objectsForDeletion;

		virtual void AddCallback() override;
		virtual void PostRenderUI() override { deleteAll(); }
		void deleteModel(gfx::BaseModel* model);
		void deleteObject(ObjectService::Object* obj);
		void deleteAll();
	};
}
