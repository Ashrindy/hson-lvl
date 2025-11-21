#include "cleaner-service.h"
#include "../graphics/graphics.h"
#include "../app.h"

using namespace ulvl::app;

void CleanerService::AddCallback() {
	Application::instance->addListener(this);
}

void CleanerService::deleteModel(gfx::BaseModel* model) {
	modelsForDeletion.push_back(model);
}

void CleanerService::deleteObject(ObjectService::Object* obj) {
	objectsForDeletion.push_back(obj);
}

void CleanerService::deleteAll() {
	auto& models = gfx::Graphics::instance->models;
	for (auto* model : modelsForDeletion) {
		models.erase(std::remove(models.begin(), models.end(), model));
		model->shutdown();
		delete model;
	}
	modelsForDeletion.clear();

	auto* objService = Application::instance->getService<ObjectService>();
	for (auto* obj : objectsForDeletion) {
		objService->removeObject(obj);
	}
	objectsForDeletion.clear();
}
