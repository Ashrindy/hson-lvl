#include "cleaner-service.h"
#include "../graphics/graphics.h"

using namespace ulvl::app;

void CleanerService::deleteModel(gfx::Model* model) {
	modelsForDeletion.push_back(model);
}

void CleanerService::deleteAll() {
	auto& models = gfx::Graphics::instance->models;
	for (auto* model : modelsForDeletion) {
		models.erase(std::remove(models.begin(), models.end(), model));
		model->shutdown();
		delete model;
	}
	modelsForDeletion.clear();
}
