#include "object-selection-manager.h"
#include "../app.h"
#include "../graphics/graphics.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <ImGuizmo.h>

using namespace ulvl::app;

void ObjectSelectionManager::updateMatrix() {
	objectMatrix = glm::translate(glm::mat4(1), selected->getPosition()) * glm::toMat4(selected->getQuaternion());
}

void ObjectSelectionManager::updateObject() {
	selected->setPosition({ objectMatrix[3] });
	selected->setRotation(glm::eulerAngles(glm::quat_cast(objectMatrix)));
}

void ObjectSelectionManager::AddCallback() {
	Application::instance->addListener(this);
}

void ObjectSelectionManager::select(ObjectService::Object* object) {
	selected = object;
	updateMatrix();
}

void ObjectSelectionManager::deselect() {
	selected = nullptr;
}

void ObjectSelectionManager::Render() {
	if (!selected) return;
	auto* graphics = gfx::Graphics::instance;
	auto* camera = graphics->camera;

	auto viewMatrix = camera->viewMatrix();
	auto projMatrix = camera->projMatrix();

	projMatrix[1][1] *= -1.0f;

	updateMatrix();
	if (ImGuizmo::Manipulate((const float*)&viewMatrix, (const float*)&projMatrix, ImGuizmo::OPERATION::UNIVERSAL, ImGuizmo::MODE::WORLD, (float*)&objectMatrix))
		updateObject();
}
