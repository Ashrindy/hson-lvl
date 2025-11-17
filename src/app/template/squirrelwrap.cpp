#include "squirrelwrap.h"
#include "../../app.h"
#include "../debug-visual-service.h"

#include <sqstdio.h>
#include <sqstdaux.h>

#include <cstdarg>

using namespace ulvl::app;

void printFunc(HSQUIRRELVM, const SQChar* s, ...) {
    va_list args;
    va_start(args, s);
    vprintf(s, args);
    va_end(args);
}

void errorFunc(HSQUIRRELVM, const SQChar* s, ...) {
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    va_end(args);
}

void SquirrelWrap::init() {
	vm = sq_open(1024);
    sq_setprintfunc(vm, printFunc, errorFunc);
}

void SquirrelWrap::loadFile(const std::filesystem::path& path) {
	sq_pushroottable(vm);
	sqstd_dofile(vm, path.string().c_str(), SQFalse, SQTrue);
    sq_pop(vm, 1);
}

void SquirrelWrap::registerFunction(SQFUNCTION func, const char* funcName) {
    sq_pushroottable(vm);
    sq_pushstring(vm, funcName, -1);
    sq_newclosure(vm, func, 0);
    sq_newslot(vm, -3, SQFalse);
    sq_pop(vm, 1);
}

ModelData ulvl::app::SquirrelWrap::callGetModelData(ObjectService::Object* obj)
{
    ModelData modelData{};

    sq_pushroottable(vm);
    char funcName[0x100];
    snprintf(funcName, sizeof(funcName), "%s_%s", obj->hson->type.c_str(), "GetModelData");
    sq_pushstring(vm, funcName, -1);
    if (SQ_FAILED(sq_get(vm, -2))) {
        sq_pop(vm, 1);
        return modelData;
    }

    sq_pushroottable(vm);
    sq_pushstring(vm, "Object", -1);
    sq_get(vm, -2);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, obj);

    sq_pushstring(vm, "ModelData", -1);
    sq_get(vm, -3);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, &modelData);

    if (SQ_FAILED(sq_call(vm, 3, SQFalse, SQTrue))) {
        sq_pushroottable(vm);
        sq_getlasterror(vm);

        const SQChar* errStr = nullptr;

        if (SQ_SUCCEEDED(sq_tostring(vm, -1))) {
            sq_getstring(vm, -1, &errStr);
            if (errStr)
                printf("Squirrel Error: %s\n", errStr);

            sq_pop(vm, 1);
        }
        sq_pop(vm, 2);
        return modelData;
    }

    sq_pop(vm, 2);

    return modelData;
}

void SquirrelWrap::callAddDebugVisual(ObjectService::Object* obj) {
    sq_pushroottable(vm);
    char funcName[0x100];
    snprintf(funcName, sizeof(funcName), "%s_%s", obj->hson->type.c_str(), "AddDebugVisual");
    sq_pushstring(vm, funcName, -1);
    if (SQ_FAILED(sq_get(vm, -2))) {
        sq_pop(vm, 1);
        return;
    }

    sq_pushroottable(vm);
    sq_pushstring(vm, "Object", -1);
    sq_get(vm, -2);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, obj);

    sq_pushstring(vm, "DebugVisual", -1);
    sq_get(vm, -3);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, Application::instance->getService<DebugVisualService>());

    if (SQ_FAILED(sq_call(vm, 3, SQFalse, SQTrue))) {
        sq_pushroottable(vm);
        sq_getlasterror(vm);

        const SQChar* errStr = nullptr;

        if (SQ_SUCCEEDED(sq_tostring(vm, -1))) {
            sq_getstring(vm, -1, &errStr);
            if (errStr)
                printf("Squirrel Error: %s\n", errStr);

            sq_pop(vm, 1);
        }
        sq_pop(vm, 2);
        return;
    }

    sq_pop(vm, 2);
}
