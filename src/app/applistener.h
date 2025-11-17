#pragma once
#include <SDL3/SDL.h>

namespace ulvl::app {
	class ApplicationListener {
	public:
		virtual void EventCallback(SDL_Event e) {};
		virtual void PreUpdate() {};
		virtual void PreRender() {};
		virtual void Render() {};
		virtual void PreRenderUI() {};
		virtual void PostRenderUI() {};
	};
}
