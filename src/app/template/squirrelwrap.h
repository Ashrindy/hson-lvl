#pragma once
#include <squirrel.h>
#include "../object-service.h"
#include "../../graphics/basemodel.h"
#include "../resources/res-model.h"
#include <filesystem>

#define RegisterFunc(FUNC) registerFunction(FUNC, #FUNC)

namespace ulvl::app {
	struct SquirrelWrap {
		typedef SQInteger(*SQFUNCTION)(HSQUIRRELVM);
		HSQUIRRELVM vm;

		~SquirrelWrap() { kill(); }
		void init();
		void loadFile(const std::filesystem::path& path);
		void registerFunction(SQFUNCTION func, const char* funcName);
		gfx::ModelData callGetModelData(ObjectService::Object* obj);
		void callAddDebugVisual(ObjectService::Object* obj);
		void callAddDynamicDebugVisual(ObjectService::Object* obj);
		void callDynamicDebugVisualEnd(ObjectService::Object* obj);
		void kill();
	};
}
