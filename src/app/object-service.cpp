#include "object-service.h"
#include "template-manager.h"
#include "../app.h"
#include "../graphics/graphics.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace ulvl::app;

ObjectService::Object::Object(const hl::guid& guid, hl::hson::object* object) : guid{ guid }, hson { object } {
	model = new gfx::Model{};

    updateModel();

    model->setWorldMatrix(getWorldMatrix());

    gfx::Graphics::instance->models.push_back(model);
}

void ObjectService::Object::setPosition(const glm::vec3& pos) {
    hson->position = { pos.x, pos.y, pos.z };

    model->setPosition(pos);
}

void ObjectService::Object::setLocalPosition(const glm::vec3& pos) {
    hson->position = { pos.x, pos.y, pos.z };

    if (hasParent()) {
        model->setPosition(getWorldMatrix()[3]);
    }
    else
        model->setPosition(pos);
}

void ObjectService::Object::setLocalRotation(const glm::vec3& rot) {
    auto quat = glm::quat{ rot };
    hson->rotation = { quat.x, quat.y, quat.z, quat.w };

    if (hasParent()) {
        auto worldMatrix = getWorldMatrix();
        model->setRotation(glm::quat_cast(worldMatrix));
    }
    else
        model->setRotation(rot);
}

void ObjectService::Object::setRotation(const glm::vec3& rot) {
    auto quat = glm::quat{ rot };
    hson->rotation = { quat.x, quat.y, quat.z, quat.w };

    model->setRotation(rot);
}

glm::vec3 ObjectService::Object::getLocalPosition() const {
    if (!hson->position.has_value()) return glm::vec3{ 0, 0, 0 };

    auto& hsonPos = hson->position.value();
    return glm::vec3{ hsonPos.x, hsonPos.y, hsonPos.z };
}

glm::vec3 ObjectService::Object::getLocalRotation() const {
    if (!hson->rotation.has_value()) return glm::vec3{ 0, 0, 0 };

    auto& hsonRot = hson->rotation.value();
    auto quat = glm::quat{ hsonRot.w, hsonRot.x, hsonRot.y, hsonRot.z };
    return glm::eulerAngles(quat);
}

glm::quat ObjectService::Object::getLocalQuaternion() const {
    if (!hson->rotation.has_value()) return glm::quat{ 1, 0, 0, 0 };

    auto& hsonRot = hson->rotation.value();
    return glm::quat{ hsonRot.w, hsonRot.x, hsonRot.y, hsonRot.z };
}

glm::vec3 ObjectService::Object::getPosition() const {
    auto worldMat = getWorldMatrix();
    return worldMat[3];
}

glm::vec3 ObjectService::Object::getRotation() const {
    return glm::eulerAngles(getQuaternion());
}

glm::quat ObjectService::Object::getQuaternion() const {
    auto worldMat = getWorldMatrix();
    return glm::quat_cast(worldMat);
}

glm::mat4 ObjectService::Object::getWorldMatrix() const {
    if (hasParent())
        return getLocalMatrix() * getParent()->getWorldMatrix();
    else
        return getLocalMatrix();
}

glm::mat4 ObjectService::Object::getLocalMatrix() const {
    return glm::translate(glm::mat4(1), getLocalPosition()) * glm::toMat4(getLocalQuaternion());
}

ObjectService::Object* ObjectService::Object::getParent() const {
    if (hasParent())
        return Application::instance->getService<ObjectService>()->getObject(hson->parentID.value());

    return nullptr;
}

bool ObjectService::Object::hasParent() const {
    return hson->parentID.has_value();
}

bool ObjectService::Object::hasChildren() const {
    return children.size() != 0;
}

void ObjectService::Object::updateChildren() {
    children.clear();

    auto* objService = Application::instance->getService<ObjectService>();

    for (auto* obj : objService->objects)
        if (obj->getParent() == this)
            children.push_back(obj);
}

void ObjectService::Object::updateModel() {
    model->clearMeshes();

    ModelData modelData = Application::instance->getService<TemplateManager>()->currentTemplate->getModelData(this);
    if (modelData.vertices != nullptr) {
        model->addMesh(modelData.vertices, modelData.vertexCount, modelData.indices, modelData.indexCount, nullptr);
        delete modelData.vertices;
        delete modelData.indices;
    }
    else {
        gfx::BaseVertex vertices[]{
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}}
        };

        unsigned short indices[]{
            0, 1, 2,
            2, 3, 0
        };

        model->addMesh(vertices, ARRAY_SIZE(vertices), indices, ARRAY_SIZE(indices), nullptr);
    }
}

ObjectService::Object* ObjectService::addObject(const hl::guid& guid, hl::hson::object* hson) {
    auto* object = new Object{ guid, hson };
    objects.push_back(object);
    return object;
}

ObjectService::Object* ObjectService::getObject(const hl::guid& guid) {
    for (auto* object : objects)
        if (object->guid == guid)
            return object;

    return nullptr;
}
