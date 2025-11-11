#include "project-manager.h"
#include "../app.h"

using namespace ulvl::app;

ProjectManager::Project::Project(const std::filesystem::path& folderPath) : folderPath{ folderPath } {
	for (const auto& file : std::filesystem::directory_iterator(folderPath))
		if (file.path().extension() == ".hson")
			loadLayer(file);
}

ProjectManager::Project::Project(const std::string& name) : folderPath{ name } { }

void ProjectManager::Project::loadLayer(const std::filesystem::path& hsonPath) {
	layers.push_back(new Layer{ hsonPath });
}

ProjectManager::Layer* ProjectManager::Project::getLayer(const hl::hson::project* project) {
	for (auto* layer : layers)
		if (layer->hson == project)
			return layer;

	return nullptr;
}

void ProjectManager::Project::save() {
	for (auto* layer : layers)
		layer->save();
}

void ProjectManager::loadProject(const std::filesystem::path& folderPath) {
	projects.push_back(new Project{ folderPath });
}

void ProjectManager::addProject(const std::string& name) {
	projects.push_back(new Project{ name });
}

ProjectManager::Layer* ProjectManager::getLayer(const hl::hson::project* project) {
	for (auto* proj : projects)
		if (auto* layer = proj->getLayer(project))
			return layer;

	return nullptr;
}

void ProjectManager::setUnsaved(bool unsaved) {
	if (this->unsaved == unsaved) return;

	this->unsaved = unsaved;
	gfx::Graphics::instance->setUnsaved(unsaved);
}

ProjectManager::Layer::Layer(const std::filesystem::path& hsonPath) : hsonPath{ hsonPath } {
	hson = new hl::hson::project{ hsonPath };
	auto* objectService = Application::instance->getService<ObjectService>();
	for (auto& object : hson->objects)
		objects.push_back(objectService->addObject(object.first, &object.second, hson));
	for (auto* object : objects)
		object->updateChildren();
}

void ProjectManager::Layer::save() {
	Application::instance->getService<ProjectManager>()->setUnsaved(false);
	hson->save(hsonPath);
}
