#pragma once
#include <vector>
#include <filesystem>
#include <chrono>

#include "app/applistener.h"
#include "app/service.h"
#include "graphics/graphics.h"
#include "ui/panel.h"

namespace ulvl {
	class Application {
	public:
		struct FrameTimer {
			using clock = std::chrono::high_resolution_clock;
			std::chrono::time_point<clock> lastTime = clock::now();
			float deltaTime = 0.0f;
			float fps = 0.0f;

			void update();
		};

		static Application* instance;

		gfx::Graphics* graphics;
		bool isRunning{ true };
		FrameTimer frameTimer;
		SDL_Event e;
		std::vector<Panel*> panels;
		std::vector<Panel*> panelsToBeAdded;
		std::vector<app::Service*> services;
		std::vector<app::ApplicationListener*> appListeners;

		void init(int argc, char** argv);
		void loop();
		template<typename T>
		bool panelExists() {
			return getPanel<T>();
		}
		template<typename T>
		bool panelExists(unsigned int id = 0) {
			return getPanel<T>(id);
		}
		template<typename T>
		void addPanel() {
			if (panelExists<T>()) return;
			panelsToBeAdded.push_back(new T{});
		}
		template<typename T>
		void addPanel(unsigned int id) {
			if (panelExists<T>(id)) return;
			auto* panel = new T{};
			panel->id = id;
			panelsToBeAdded.push_back(panel);
		}
		template<typename T>
		T* getPanel() {
			for (auto* panel : panels)
				if (T* tPanel = dynamic_cast<T*>(panel))
					return tPanel;
			for (auto* panel : panelsToBeAdded)
				if (T* tPanel = dynamic_cast<T*>(panel))
					return tPanel;
			return nullptr;
		}
		template<typename T>
		T* getPanel(unsigned int id) {
			for (auto* panel : panels)
				if (T* tPanel = dynamic_cast<T*>(panel))
					if (panel->id == id)
						return tPanel;
			for (auto* panel : panelsToBeAdded)
				if (T* tPanel = dynamic_cast<T*>(panel))
					if (panel->id == id)
						return tPanel;
			return nullptr;
		}
		void removePanel(Panel* panel) {
			panels.erase(std::remove(panels.begin(), panels.end(), panel));
			delete panel;
		}
		template<typename T>
		void addService() {
			services.push_back(new T{});
			services[services.size() - 1]->AddCallback();
		}
		template<typename T>
		T* getService() {
			for (auto* service : services)
				if (T* tService = dynamic_cast<T*>(service))
					return tService;
			return nullptr;
		}
		inline void addListener(app::ApplicationListener* listener) {
			appListeners.push_back(listener);
		}
		~Application();

	private:
		void update();
		void render();
	};
}
