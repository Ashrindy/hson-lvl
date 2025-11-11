#pragma once
#include "service.h"
#include "../graphics/model.h"

namespace ulvl::app {
	class DebugVisualService : public Service {
	public:
		gfx::Model* model;

		virtual void AddCallback() override;
	};
}
