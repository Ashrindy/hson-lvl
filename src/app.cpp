#include "app.h"
#include "panels/object-list.h"
#include "panels/object-inspector.h"
#include "panels/object-library.h"
#include "panels/viewport.h"
#include "app/camera-service.h"
#include "app/object-service.h"
#include "app/project-manager.h"
#include "app/object-selection-manager.h"
#include "app/template-manager.h"
#include "app/file-dialog-service.h"
#include "app/cleaner-service.h"
#include "app/debug-visual-service.h"
#include "app/settings-manager.h"
#include "app/model-service.h"
#include <imgui_impl_sdl3.h>

using namespace ulvl;
namespace fs = std::filesystem;

Application* Application::instance = nullptr;

void Application::init(int argc, char** argv) {
    graphics = new gfx::Graphics;
    graphics->init();

    instance = this;

    addPanel<ObjectList>();
    addPanel<ObjectInspector>();
    addPanel<ObjectLibrary>();
    addPanel<Viewport>();

    addService<app::CameraService>();
    addService<app::ObjectService>();
    addService<app::ProjectManager>();
    addService<app::ObjectSelectionManager>();
    addService<app::SettingsManager>();
    addService<app::TemplateManager>();
    addService<app::FileDialogService>();
    addService<app::CleanerService>();
    addService<app::DebugVisualService>();
    addService<app::ModelService>();

    if (argc > 1) {
        fs::path path{ rel_to_exe(argv[1]) };
        if (fs::exists(path)) {
            auto* projMgr = getService<app::ProjectManager>();
            if (path.has_extension()) {
                projMgr->addProject(path.parent_path().stem().string().c_str());
                projMgr->projects[0]->loadLayer(path);
            }
            else {
                projMgr->loadProject(path);
            }
        }
    }
}

void Application::loop()
{
    while (isRunning) {
        frameTimer.update();

        for (auto* panel : panelsToBeAdded) {
            panel->AddCallback();
            panels.push_back(panel);
        }
        panelsToBeAdded.clear();

        for (auto* listener : appListeners) listener->PreUpdate();

        update();

        for (auto* listener : appListeners) listener->PreRender();

        graphics->renderBegin();

        for (auto* listener : appListeners) listener->Render();

        graphics->renderEnd();

        for (auto* listener : appListeners) listener->PreRenderUI();

        graphics->renderUIBegin();

        render();

        graphics->renderUIEnd();

        for (auto* listener : appListeners) listener->PostRenderUI();
    }
}

Application::~Application()
{
    graphics->shutdown();
    delete graphics;
}

void Application::render()
{
    for (auto* panel : panels)
        panel->Render();
}

void Application::update() {
    for (auto* service : services)
        service->Update();

    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL3_ProcessEvent(&e);

        for (auto* listener : appListeners)
            listener->EventCallback(e);

        switch (e.type) {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            isRunning = false;
            break;

        case SDL_EVENT_WINDOW_RESIZED:
            graphics->resize(e.window.data1, e.window.data2);
            break;
        }
    }
}

void Application::FrameTimer::update() {
    auto now = clock::now();
    std::chrono::duration<float> delta = now - lastTime;
    deltaTime = delta.count();
    fps = 1.0f / deltaTime;
    lastTime = now;
}
