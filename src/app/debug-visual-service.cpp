#include "debug-visual-service.h"
#include "../graphics/graphics.h"
#include "../app.h"
#include "cleaner-service.h"

using namespace ulvl::app;

DebugVisualService::~DebugVisualService() {
    Application::instance->getService<CleanerService>()->deleteModel(cube);
}

void DebugVisualService::AddCallback() {
    auto* gfx = gfx::Graphics::instance;

	cube = new gfx::InstancedModel{};

    gfx::PosVertex cubeVertices[]{
        {{-0.5f, -0.5f,  0.5f}},
        {{ 0.5f, -0.5f,  0.5f}},
        {{ 0.5f,  0.5f,  0.5f}},
        {{-0.5f,  0.5f,  0.5f}},
        {{ 0.5f, -0.5f, -0.5f}},
        {{-0.5f, -0.5f, -0.5f}},
        {{-0.5f,  0.5f, -0.5f}},
        {{ 0.5f,  0.5f, -0.5f}},
        {{-0.5f, -0.5f, -0.5f}},
        {{-0.5f, -0.5f,  0.5f}},
        {{-0.5f,  0.5f,  0.5f}},
        {{-0.5f,  0.5f, -0.5f}},
        {{ 0.5f, -0.5f,  0.5f}},
        {{ 0.5f, -0.5f, -0.5f}},
        {{ 0.5f,  0.5f, -0.5f}},
        {{ 0.5f,  0.5f,  0.5f}},
        {{-0.5f,  0.5f,  0.5f}},
        {{ 0.5f,  0.5f,  0.5f}},
        {{ 0.5f,  0.5f, -0.5f}},
        {{-0.5f,  0.5f, -0.5f}},
        {{-0.5f, -0.5f, -0.5f}},
        {{ 0.5f, -0.5f, -0.5f}},
        {{ 0.5f, -0.5f,  0.5f}},
        {{-0.5f, -0.5f,  0.5f}}
    };

    unsigned short cubeIndices[] {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9,10,10,11, 8,
        12,13,14,14,15,12,
        16,17,18,18,19,16,
        20,21,22,22,23,20
    };

    cube->setModel(cubeVertices, ARRAY_SIZE(cubeVertices), cubeIndices, ARRAY_SIZE(cubeIndices));
    gfx->models.push_back(cube);
}

void DebugVisualService::addCube(gfx::InstancedMesh mesh) {
    cube->addMesh(mesh);
}

void ulvl::app::DebugVisualService::removeCubes(int id) {
    cube->removeMeshes(id);
}
