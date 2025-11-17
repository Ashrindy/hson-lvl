#pragma once
#include "service.h"
#include "applistener.h"
#include "object-service.h"

namespace ulvl::app {
	class ObjectSelectionManager : public Service, public ApplicationListener {
		glm::mat4 objectMatrix{};
		bool shouldUpdateDebug{ false };

		void updateMatrix();
		void updateObject();
	public:
		ObjectService::Object* selected{ nullptr };

		virtual void AddCallback() override;

		void select(ObjectService::Object* object);
		void deselect();
		void renderGizmo();
		void save();

		virtual void PreRender() override;
		virtual void EventCallback(SDL_Event e) override;
	};
}
