#include "project-manager.h"
#include "../app.h"

using namespace ulvl::app;

ProjectManager::Project::Project(const std::filesystem::path& hsonPath) : hsonPath{ hsonPath } {
	hson = new hl::hson::project{ hsonPath };
	auto* objectService = Application::instance->getService<ObjectService>();
	for (auto& object : hson->objects)
		objects.push_back(objectService->addObject(object.first, &object.second));
	for (auto* object : objects)
		object->updateChildren();
}

void ProjectManager::loadProject(const std::filesystem::path& hsonPath) {
	projects.push_back(new Project{ hsonPath });
}
