#include "sq-funcs.h"
#include "../object-service.h"
#include "../debug-visual-service.h"
#include "../../app.h"
#include <hedgelib/models/hl_hh_model.h>
#include <glm/gtx/quaternion.hpp>

using namespace ulvl::app;

SQInteger ulvl::app::ToBytes(HSQUIRRELVM vm) {
	switch (sq_gettype(vm, 2)) {
	case OT_FLOAT: {
		SQFloat f{};
		sq_getfloat(vm, 2, &f);

		char* b = (char*)&f;

		sq_newarray(vm, 0);
		for (auto i = 0; i < sizeof(float); i++) {
			sq_pushinteger(vm, b[i]);
			sq_arrayappend(vm, -2);
		}
		break;
	}
	}

	return 1;
}

// std::string

SQInteger ulvl::app::StringCtor(HSQUIRRELVM vm) {
	const char* str = "";
	sq_getstring(vm, 2, &str);

	std::string* s = new std::string(str);

	sq_setinstanceup(vm, 1, s);
	sq_setreleasehook(vm, 1, StringReleaseHook);

	return 0;
}

SQInteger ulvl::app::StringReleaseHook(SQUserPointer p, SQInteger size) {
	delete (std::string*)p;
	return 0;
}

SQInteger ulvl::app::StringToCString(HSQUIRRELVM vm) {
	std::string* self{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);
	sq_pushstring(vm, self->c_str(), -1);
	return 1;
}

// hl::radix_tree<hl::hson::parameter>

SQInteger ulvl::app::GetParameterFloat(HSQUIRRELVM vm) {
	hl::radix_tree<hl::hson::parameter>* params{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&params, nullptr, SQFalse);

	const char* name{};
	sq_getstring(vm, 2, &name);

	float value{ 0 };

	for (auto param : *params) {
		if (strcmp(param.first, name) == 0) {
			value = param.second.value_floating();
			break;
		}
	}

	sq_pushfloat(vm, value);

	return 1;
}

SQInteger ulvl::app::GetParameterInt(HSQUIRRELVM vm) {
	hl::radix_tree<hl::hson::parameter>* params{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&params, nullptr, SQFalse);

	const char* name{};
	sq_getstring(vm, 2, &name);

	int value{ 0 };

	for (auto param : *params) {
		if (strcmp(param.first, name) == 0) {
			value = param.second.value_int();
			break;
		}
	}

	sq_pushinteger(vm, value);

	return 1;
}

SQInteger ulvl::app::GetParameterUInt(HSQUIRRELVM vm) {
	hl::radix_tree<hl::hson::parameter>* params{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&params, nullptr, SQFalse);

	const char* name{};
	sq_getstring(vm, 2, &name);

	unsigned int value{ 0 };

	for (auto param : *params) {
		if (strcmp(param.first, name) == 0) {
			value = param.second.value_uint();
			break;
		}
	}

	sq_pushinteger(vm, value);

	return 1;
}

SQInteger ulvl::app::GetParameterString(HSQUIRRELVM vm) {
	hl::radix_tree<hl::hson::parameter>* params{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&params, nullptr, SQFalse);

	const char* name{};
	sq_getstring(vm, 2, &name);

	std::string* value{};

	for (auto param : *params) {
		if (strcmp(param.first, name) == 0) {
			value = &param.second.value_string();
			break;
		}
	}

	sq_pushroottable(vm);
	sq_pushstring(vm, "String", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	sq_remove(vm, -2);
	sq_remove(vm, -2);
	sq_setinstanceup(vm, -1, value);
	return 1;
}

SQInteger ulvl::app::GetParameterArray(HSQUIRRELVM vm) {
	hl::radix_tree<hl::hson::parameter>* params{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&params, nullptr, SQFalse);

	const char* name{};
	sq_getstring(vm, 2, &name);

	std::vector<hl::hson::parameter> value;

	for (auto param : *params) {
		if (strcmp(param.first, name) == 0) {
			value = param.second.value_array();
			break;
		}
	}

	sq_newarray(vm, 0);

	for (auto& param : value) {
		switch (param.type()) {
		case hl::hson::parameter_type::boolean: sq_pushbool(vm, param.value_bool()); break;
		case hl::hson::parameter_type::signed_integer: sq_pushinteger(vm, param.value_int()); break;
		case hl::hson::parameter_type::unsigned_integer: sq_pushinteger(vm, param.value_uint()); break;
		case hl::hson::parameter_type::floating: sq_pushfloat(vm, (float)param.value_floating()); break;
		case hl::hson::parameter_type::string: {
			sq_pushroottable(vm);
			sq_pushstring(vm, "String", -1);
			sq_get(vm, -2);
			sq_createinstance(vm, -1);
			sq_remove(vm, -2);
			sq_remove(vm, -2);
			std::string* string = new std::string{ param.value_string() };
			sq_setinstanceup(vm, -1, string);
			sq_setreleasehook(vm, -1, StringReleaseHook);
			break;
		}
		case hl::hson::parameter_type::object: {
			sq_pushroottable(vm);
			sq_pushstring(vm, "Parameters", -1);
			sq_get(vm, -2);
			sq_createinstance(vm, -1);
			sq_remove(vm, -2);
			sq_remove(vm, -2);
			hl::radix_tree<hl::hson::parameter>* parameters = new hl::radix_tree<hl::hson::parameter>{ param.value_object() };
			sq_setinstanceup(vm, -1, parameters);
			sq_setreleasehook(vm, -1, RadixTreeReleaseHook);
			break;
		}
		}
		sq_arrayappend(vm, -2);
	}

	HSQOBJECT arrObj;
	sq_getstackobj(vm, -1, &arrObj);
	sq_addref(vm, &arrObj);

	return 1;
}

SQInteger ulvl::app::GetParameterObject(HSQUIRRELVM vm) {
	hl::radix_tree<hl::hson::parameter>* params{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&params, nullptr, SQFalse);

	const char* name{};
	sq_getstring(vm, 2, &name);

	hl::radix_tree<hl::hson::parameter>* value{};

	for (auto param : *params) {
		if (strcmp(param.first, name) == 0) {
			value = &param.second.value_object();
			break;
		}
	}

	sq_pushroottable(vm);
	sq_pushstring(vm, "Parameters", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	sq_setinstanceup(vm, -1, value);
	return 1;
}

SQInteger ulvl::app::RadixTreeReleaseHook(SQUserPointer p, SQInteger size) {
	delete (hl::radix_tree<hl::hson::parameter>*)p;
	return 0;
}

// Object

SQInteger ulvl::app::GetObjLocalPosition(HSQUIRRELVM vm) {
	ObjectService::Object* self = nullptr;
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec3", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	glm::vec3* copy = new glm::vec3{ self->getLocalPosition() };
	sq_setinstanceup(vm, -1, copy);
	sq_setreleasehook(vm, -1, Vec3ReleaseHook);
	return 1;
}

SQInteger ulvl::app::GetObjWorldPosition(HSQUIRRELVM vm) {
	ObjectService::Object* self = nullptr;
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec3", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	glm::vec3* copy = new glm::vec3{ self->getPosition() };
	sq_setinstanceup(vm, -1, copy);
	sq_setreleasehook(vm, -1, Vec3ReleaseHook);
	return 1;
}
SQInteger ulvl::app::GetObjRotation(HSQUIRRELVM vm) {
	ObjectService::Object* self = nullptr;
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec4", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	glm::quat rot = self->getQuaternion();
	glm::vec4* copy = new glm::vec4(rot.x, rot.y, rot.z, rot.w);
	sq_setinstanceup(vm, -1, copy);
	sq_setreleasehook(vm, -1, Vec4ReleaseHook);
	return 1;
}
SQInteger ulvl::app::GetObjLinear(HSQUIRRELVM vm) {
	ObjectService::Object* self = nullptr;
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Mat3", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	glm::mat3* linear = new glm::mat3{ self->getWorldMatrix() };
	sq_setinstanceup(vm, -1, linear);
	sq_setreleasehook(vm, -1, Mat3ReleaseHook);
	return 1;
}
SQInteger ulvl::app::GetObjName(HSQUIRRELVM vm) {
	ObjectService::Object* self = nullptr;
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "String", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	sq_setinstanceup(vm, -1, &self->hson->name.value());
	return 1;
}

SQInteger ulvl::app::GetObjParameters(HSQUIRRELVM vm) {
	ObjectService::Object* self{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Parameters", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	sq_setinstanceup(vm, -1, &self->hson->parameters);
	return 1;
}

SQInteger ulvl::app::GetObjParent(HSQUIRRELVM vm) {
	ObjectService::Object* self{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Object", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	sq_setinstanceup(vm, -1, self->getParent());
	return 1;
}

SQInteger ulvl::app::GetObjID(HSQUIRRELVM vm) {
	ObjectService::Object* self{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	auto guid = self->guid.as_string();
	sq_pushstring(vm, guid.c_str(), -1);
	return 1;
}

SQInteger ulvl::app::GetObjHasParent(HSQUIRRELVM vm) {
	ObjectService::Object* self{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&self, nullptr, SQFalse);

	sq_pushbool(vm, self->hasParent());
	return 1;
}

SQInteger ulvl::app::GetObjByID(HSQUIRRELVM vm) {
	const char* id{};
	sq_getstring(vm, 2, &id);

	hl::guid guid{ id };
	auto* obj = Application::instance->getService<app::ObjectService>()->getObject(guid);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Object", -1);
	sq_get(vm, -2);
	sq_createinstance(vm, -1);
	sq_setinstanceup(vm, -1, obj);
	return 1;
}

// glm::vec3

SQInteger ulvl::app::Vec3Ctor(HSQUIRRELVM vm) {
	SQFloat x = 0, y = 0, z = 0;

	sq_getfloat(vm, 2, &x);
	sq_getfloat(vm, 3, &y);
	sq_getfloat(vm, 4, &z);

	glm::vec3* obj = new glm::vec3{ x, y, z };
	sq_setinstanceup(vm, 1, obj);
	sq_setreleasehook(vm, 1, Vec3ReleaseHook);

	return 0;
}

SQInteger ulvl::app::Vec3GetX(HSQUIRRELVM vm) {
	glm::vec3* vec3{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec3, nullptr, SQFalse);

	sq_pushfloat(vm, vec3->x);

	return 1;
}

SQInteger ulvl::app::Vec3SetX(HSQUIRRELVM vm) {
	glm::vec3* vec3{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec3, nullptr, SQFalse);
	SQFloat val{};
	sq_getfloat(vm, 2, &val);
	vec3->x = val;
	return 0;
}

SQInteger ulvl::app::Vec3GetY(HSQUIRRELVM vm) {
	glm::vec3* vec3{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec3, nullptr, SQFalse);

	sq_pushfloat(vm, vec3->y);

	return 1;
}

SQInteger ulvl::app::Vec3SetY(HSQUIRRELVM vm) {
	glm::vec3* vec3{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec3, nullptr, SQFalse);
	SQFloat val{};
	sq_getfloat(vm, 2, &val);
	vec3->y = val;
	return 0;
}

SQInteger ulvl::app::Vec3GetZ(HSQUIRRELVM vm) {
	glm::vec3* vec3{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec3, nullptr, SQFalse);

	sq_pushfloat(vm, vec3->z);

	return 1;
}

SQInteger ulvl::app::Vec3RotateByQuat(HSQUIRRELVM vm) {
	// Easier to do it this way than remake it in squirrel

	glm::vec3* vec;
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec, 0, SQFalse);
	glm::vec4* quat;
	sq_getinstanceup(vm, 2, (SQUserPointer*)&quat, 0, SQFalse);

	glm::quat q = glm::quat(quat->w, quat->x, quat->y, quat->z);
	glm::vec3 rotated = q * (*vec);
	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec3", -1);
	sq_get(vm, -2);
	sq_remove(vm, -2);

	sq_createinstance(vm, -1);
	glm::vec3* retVec = new glm::vec3(rotated);
	sq_setinstanceup(vm, -1, retVec);
	sq_setreleasehook(vm, -1, Vec3ReleaseHook);

	return 1;
}

SQInteger ulvl::app::Vec3Normalize(HSQUIRRELVM vm) {
	// Easier to do it this way than remake it in squirrel

	glm::vec3* vec;
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec, 0, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec3", -1);
	sq_get(vm, -2);
	sq_remove(vm, -2);
	sq_createinstance(vm, -1);
	glm::vec3* retVec = new glm::vec3(glm::normalize(*vec));
	sq_setinstanceup(vm, -1, retVec);
	sq_setreleasehook(vm, -1, Vec3ReleaseHook);

	return 1;
}

SQInteger ulvl::app::Vec3Add(HSQUIRRELVM vm) {
	glm::vec3* a{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&a, 0, SQFalse);

	auto type = sq_gettype(vm, 2);
	glm::vec3* b{ nullptr };
	float fB{ 0 };
	if (type == SQObjectType::OT_INSTANCE)
		sq_getinstanceup(vm, 2, (SQUserPointer*)&b, 0, SQFalse);
	else if (type == SQObjectType::OT_FLOAT)
		sq_getfloat(vm, 2, &fB);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec3", -1);
	sq_get(vm, -2);
	sq_remove(vm, -2);
	sq_createinstance(vm, -1);
	glm::vec3* retVec{};
	if (b)
		retVec = new glm::vec3{ *a + *b };
	else
		retVec = new glm::vec3{ *a + fB };
	sq_setinstanceup(vm, -1, retVec);
	sq_setreleasehook(vm, -1, Vec3ReleaseHook);

	return 1;
}

SQInteger ulvl::app::Vec3Sub(HSQUIRRELVM vm) {
	glm::vec3* a{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&a, 0, SQFalse);

	auto type = sq_gettype(vm, 2);
	glm::vec3* b{ nullptr };
	float fB{ 0 };
	if (type == SQObjectType::OT_INSTANCE)
		sq_getinstanceup(vm, 2, (SQUserPointer*)&b, 0, SQFalse);
	else if (type == SQObjectType::OT_FLOAT)
		sq_getfloat(vm, 2, &fB);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec3", -1);
	sq_get(vm, -2);
	sq_remove(vm, -2);
	sq_createinstance(vm, -1);
	glm::vec3* retVec{};
	if (b)
		retVec = new glm::vec3{ *a - *b };
	else
		retVec = new glm::vec3{ *a - fB };
	sq_setinstanceup(vm, -1, retVec);
	sq_setreleasehook(vm, -1, Vec3ReleaseHook);

	return 1;
}

SQInteger ulvl::app::Vec3Multiply(HSQUIRRELVM vm) {
	auto type = sq_gettype(vm, 1);
	glm::vec3* a{ nullptr };
	float fA{ 1 };
	if (type == SQObjectType::OT_INSTANCE)
		sq_getinstanceup(vm, 1, (SQUserPointer*)&a, 0, SQFalse);
	else if (type == SQObjectType::OT_FLOAT)
		sq_getfloat(vm, 1, &fA);

	type = sq_gettype(vm, 2);
	glm::vec3* b{ nullptr };
	float fB{ 1 };
	if (type == SQObjectType::OT_INSTANCE)
		sq_getinstanceup(vm, 2, (SQUserPointer*)&b, 0, SQFalse);
	else if (type == SQObjectType::OT_FLOAT)
		sq_getfloat(vm, 2, &fB);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec3", -1);
	sq_get(vm, -2);
	sq_remove(vm, -2);
	sq_createinstance(vm, -1);
	glm::vec3* retVec{};
	if (a && b)
		retVec = new glm::vec3{ *a * *b };
	else if (a && !b)
		retVec = new glm::vec3{ *a * fB };
	else if (!a && b)
		retVec = new glm::vec3{ fA * *b };
	sq_setinstanceup(vm, -1, retVec);
	sq_setreleasehook(vm, -1, Vec3ReleaseHook);

	return 1;
}

SQInteger ulvl::app::Vec3MultiplyMat3(HSQUIRRELVM vm) {
	glm::vec3* a{ nullptr };
	sq_getinstanceup(vm, 1, (SQUserPointer*)&a, 0, SQFalse);

	glm::mat3* b{ nullptr };
	sq_getinstanceup(vm, 2, (SQUserPointer*)&b, 0, SQFalse);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec3", -1);
	sq_get(vm, -2);
	sq_remove(vm, -2);
	sq_createinstance(vm, -1);
	glm::vec3* retVec{};
	retVec = new glm::vec3{ *b * *a };
	sq_setinstanceup(vm, -1, retVec);
	sq_setreleasehook(vm, -1, Vec3ReleaseHook);

	return 1;
}

SQInteger ulvl::app::Vec3Divide(HSQUIRRELVM vm) {
	glm::vec3* a{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&a, 0, SQFalse);

	auto type = sq_gettype(vm, 2);
	glm::vec3* b{ nullptr };
	float fB{ 1 };
	if (type == SQObjectType::OT_INSTANCE)
		sq_getinstanceup(vm, 2, (SQUserPointer*)&b, 0, SQFalse);
	else if (type == SQObjectType::OT_FLOAT)
		sq_getfloat(vm, 2, &fB);

	sq_pushroottable(vm);
	sq_pushstring(vm, "Vec3", -1);
	sq_get(vm, -2);
	sq_remove(vm, -2);
	sq_createinstance(vm, -1);
	glm::vec3* retVec{};
	if (b)
		retVec = new glm::vec3{ *a / *b };
	else
		retVec = new glm::vec3{ *a / fB };
	sq_setinstanceup(vm, -1, retVec);
	sq_setreleasehook(vm, -1, Vec3ReleaseHook);

	return 1;
}

SQInteger ulvl::app::Vec3DistanceSq(HSQUIRRELVM vm) {
	glm::vec3* a{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&a, 0, SQFalse);

	glm::vec3* b{};
	sq_getinstanceup(vm, 2, (SQUserPointer*)&b, 0, SQFalse);

	float dx = a->x - b->x;
	float dy = a->y - b->y;
	
	float retVal = dx*dx + dy*dy;
	sq_pushfloat(vm, retVal);

	return 1;
}

SQInteger ulvl::app::Vec3Length(HSQUIRRELVM vm) {
	glm::vec3* a{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&a, 0, SQFalse);
	float retVal = sqrtf(a->x* a->x + a->y * a->y + a->z * a->z);
	sq_pushfloat(vm, retVal);

	return 1;
}

SQInteger ulvl::app::Vec3SetZ(HSQUIRRELVM vm) {
	glm::vec3* vec3{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec3, nullptr, SQFalse);
	SQFloat val{};
	sq_getfloat(vm, 2, &val);
	vec3->z = val;
	return 0;
}

SQInteger ulvl::app::Vec3ReleaseHook(SQUserPointer p, SQInteger size) {
	delete (glm::vec3*)p;
	return 0;
}

// glm::vec4
SQInteger ulvl::app::Vec4Ctor(HSQUIRRELVM vm) {
	SQFloat x = 0, y = 0, z = 0, w = 0;

	sq_getfloat(vm, 2, &x);
	sq_getfloat(vm, 3, &y);
	sq_getfloat(vm, 4, &z);
	sq_getfloat(vm, 5, &w);

	glm::vec4* obj = new glm::vec4{ x, y, z, w };
	sq_setinstanceup(vm, 1, obj);
	sq_setreleasehook(vm, 1, Vec4ReleaseHook);

	return 0;
}

SQInteger ulvl::app::Vec4GetW(HSQUIRRELVM vm) {
	glm::vec4* vec4{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec4, nullptr, SQFalse);

	sq_pushfloat(vm, vec4->w);

	return 1;
}

SQInteger ulvl::app::Vec4SetW(HSQUIRRELVM vm) {
	glm::vec4* vec4{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&vec4, nullptr, SQFalse);
	SQFloat val{};
	sq_getfloat(vm, 2, &val);
	vec4->w = val;
	return 0;
}

SQInteger ulvl::app::Vec4ReleaseHook(SQUserPointer p, SQInteger size) {
	delete (glm::vec4*)p;
	return 0;
}

// glm::mat3

SQInteger ulvl::app::Mat3ReleaseHook(SQUserPointer p, SQInteger size) {
	delete (glm::mat3*)p;
	return 0;
}

// ModelData

SQInteger ulvl::app::VertexElementCtor(HSQUIRRELVM vm) {
	SQInteger nargs{ sq_gettop(vm) };

	const char* name{};
	sq_getstring(vm, 2, &name);

	SQInteger fmtInt{};
	sq_getinteger(vm, 3, &fmtInt);
	plume::RenderFormat format = (plume::RenderFormat)fmtInt;

	SQInteger alignedByteOffset{ -1 };
	if (nargs >= 4)
		sq_getinteger(vm, 4, &alignedByteOffset);

	plume::RenderInputElement* inputElem = new plume::RenderInputElement{ name, 0, 0, format, 0, (unsigned int)alignedByteOffset };
	sq_setinstanceup(vm, 1, inputElem);
	sq_setreleasehook(vm, 1, VertexElementReleaseHook);

	return 1;
}

SQInteger ulvl::app::VertexElementReleaseHook(SQUserPointer p, SQInteger size) {
	delete (plume::RenderInputElement*)p;
	return 0;
}

SQInteger ulvl::app::MeshDataCtor(HSQUIRRELVM vm) {
	MeshData* meshData = new MeshData{};
	sq_setinstanceup(vm, 1, meshData);
	sq_setreleasehook(vm, 1, MeshDataReleaseHook);

	return 1;
}

SQInteger ulvl::app::MeshDataGetVertexCount(HSQUIRRELVM vm) {
	MeshData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	sq_pushinteger(vm, modelData->vertexCount);

	return 1;
}

SQInteger ulvl::app::MeshDataSetVertexCount(HSQUIRRELVM vm) {
	MeshData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	sq_getinteger(vm, 2, (SQInteger*)&modelData->vertexCount);

	return 0;
}

SQInteger ulvl::app::MeshDataSetVertices(HSQUIRRELVM vm) {
	MeshData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	if (modelData->vertices) delete modelData->vertices;

	modelData->vertexInfo.calcStrideByLayout();
	int vertSize = modelData->vertexCount * modelData->vertexInfo.stride;
	modelData->vertices = new char[vertSize];
	for (int x = 0; x < vertSize; x++) {
		sq_pushinteger(vm, x);
		sq_get(vm, 2);
		SQInteger val{};
		sq_getinteger(vm, -1, &val);
		((char*)modelData->vertices)[x] = (char)val;
		sq_pop(vm, 1);
	}

	return 0;
}

SQInteger ulvl::app::MeshDataGetIndexCount(HSQUIRRELVM vm) {
	MeshData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	sq_pushinteger(vm, modelData->indexCount);

	return 1;
}

SQInteger ulvl::app::MeshDataSetIndexCount(HSQUIRRELVM vm) {
	MeshData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	sq_getinteger(vm, 2, (SQInteger*)&modelData->indexCount);

	return 0;
}

SQInteger ulvl::app::MeshDataSetIndices(HSQUIRRELVM vm) {
	MeshData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	if (modelData->indices) delete modelData->indices;

	modelData->indices = new unsigned short[modelData->indexCount];
	for (int x = 0; x < modelData->indexCount; x++) {
		sq_pushinteger(vm, x);
		sq_get(vm, 2);
		SQInteger val{};
		sq_getinteger(vm, -1, &val);
		modelData->indices[x] = val;
		sq_pop(vm, 1);
	}

	return 0;
}

SQInteger ulvl::app::MeshDataAddVertexElement(HSQUIRRELVM vm) {
	MeshData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	plume::RenderInputElement* elem{};
	sq_getinstanceup(vm, 2, (SQUserPointer*)&elem, nullptr, SQTrue);

	modelData->vertexInfo.calcStrideByLayout();
	size_t byteOffset{ modelData->vertexInfo.stride };
	if (elem->alignedByteOffset != -1)
		byteOffset = elem->alignedByteOffset;
	modelData->vertexInfo.vertexLayout.emplace_back(elem->semanticName, 0, 0, elem->format, 0, byteOffset);

	return 0;
}

SQInteger ulvl::app::MeshDataSetVertexLayout(HSQUIRRELVM vm) {
	MeshData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	modelData->vertexInfo.vertexLayout.clear();

	HSQOBJECT elements;
	sq_resetobject(&elements);
	sq_getstackobj(vm, 2, &elements);
	SQInteger elemLen = sq_getsize(vm, 2);
	for (auto x = 0; x < elemLen; x++) {
		sq_pushinteger(vm, x);
		if (SQ_SUCCEEDED(sq_get(vm, 2))) {
			plume::RenderInputElement* elem{};
			sq_getinstanceup(vm, -1, (SQUserPointer*)&elem, nullptr, SQFalse);

			modelData->vertexInfo.calcStrideByLayout();
			size_t byteOffset{ modelData->vertexInfo.stride };
			if (elem->alignedByteOffset != -1)
				byteOffset = elem->alignedByteOffset;
			modelData->vertexInfo.vertexLayout.emplace_back(elem->semanticName, 0, 0, elem->format, 0, byteOffset);
		}
		sq_pop(vm, 1);
	}

	return 0;
}

SQInteger ulvl::app::MeshDataGetVertexStride(HSQUIRRELVM vm) {
	MeshData* modelData{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&modelData, nullptr, SQFalse);

	modelData->vertexInfo.calcStrideByLayout();
	sq_pushinteger(vm, modelData->vertexInfo.stride);

	return 1;
}

SQInteger ulvl::app::MeshDataReleaseHook(SQUserPointer p, SQInteger size) {
	delete (MeshData*)p;
	return 0;
}

SQInteger ulvl::app::ModelDataAddMesh(HSQUIRRELVM vm) {
	ModelData* model{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&model, nullptr, SQFalse);

	MeshData* mesh{};
	sq_getinstanceup(vm, 2, (SQUserPointer*)&mesh, nullptr, SQFalse);

	model->meshes.emplace_back(*mesh);

	return 0;
}

SQInteger ulvl::app::ModelDataAssign(HSQUIRRELVM vm) {
	ModelData* self{};
	sq_getinstanceup(vm, 2, (SQUserPointer*)&self, nullptr, SQFalse);

	ModelData* other{};
	sq_getinstanceup(vm, 3, (SQUserPointer*)&other, nullptr, SQFalse);

	self->meshes = std::move(other->meshes);

	return 0;
}

SQInteger ulvl::app::ModelDataReleaseHook(SQUserPointer p, SQInteger size) {
	delete (ModelData*)p;
	return 0;
}

// HLModel
constexpr static plume::RenderFormat toPlume(const hl::hh::mirage::raw_vertex_format format) {
	switch (format) {
	case hl::hh::mirage::raw_vertex_format::float1: return plume::RenderFormat::R32_FLOAT;
	case hl::hh::mirage::raw_vertex_format::float2: return plume::RenderFormat::R32G32_FLOAT;
	case hl::hh::mirage::raw_vertex_format::float3: return plume::RenderFormat::R32G32B32_FLOAT;
	case hl::hh::mirage::raw_vertex_format::float4: return plume::RenderFormat::R32G32B32A32_FLOAT;

	case hl::hh::mirage::raw_vertex_format::int1: 
	case hl::hh::mirage::raw_vertex_format::int1_norm: return plume::RenderFormat::R32_SINT;
	case hl::hh::mirage::raw_vertex_format::int2: 
	case hl::hh::mirage::raw_vertex_format::int2_norm: return plume::RenderFormat::R32G32_SINT;
	case hl::hh::mirage::raw_vertex_format::int4: 
	case hl::hh::mirage::raw_vertex_format::int4_norm: return plume::RenderFormat::R32G32B32A32_SINT;

	case hl::hh::mirage::raw_vertex_format::uint1: 
	case hl::hh::mirage::raw_vertex_format::uint1_norm: return plume::RenderFormat::R32_UINT;
	case hl::hh::mirage::raw_vertex_format::uint2: 
	case hl::hh::mirage::raw_vertex_format::uint2_norm: return plume::RenderFormat::R32G32_UINT;
	case hl::hh::mirage::raw_vertex_format::uint4: 
	case hl::hh::mirage::raw_vertex_format::uint4_norm: return plume::RenderFormat::R32G32B32A32_UINT;

	case hl::hh::mirage::raw_vertex_format::byte4:
	case hl::hh::mirage::raw_vertex_format::byte4_norm: return plume::RenderFormat::R8G8B8A8_SINT;

	case hl::hh::mirage::raw_vertex_format::d3d_color:
	case hl::hh::mirage::raw_vertex_format::ubyte4:
	case hl::hh::mirage::raw_vertex_format::ubyte4_norm: return plume::RenderFormat::R8G8B8A8_UINT;

	case hl::hh::mirage::raw_vertex_format::short2: return plume::RenderFormat::R16G16_SINT;
	case hl::hh::mirage::raw_vertex_format::short2_norm: return plume::RenderFormat::R16G16_SNORM;
	case hl::hh::mirage::raw_vertex_format::short4: return plume::RenderFormat::R16G16B16A16_SINT;
	case hl::hh::mirage::raw_vertex_format::short4_norm: return plume::RenderFormat::R16G16B16A16_SNORM;

	case hl::hh::mirage::raw_vertex_format::ushort2: return plume::RenderFormat::R16G16_UINT;
	case hl::hh::mirage::raw_vertex_format::ushort2_norm: return plume::RenderFormat::R16G16_UNORM;
	case hl::hh::mirage::raw_vertex_format::ushort4: return plume::RenderFormat::R16G16B16A16_UINT;
	case hl::hh::mirage::raw_vertex_format::ushort4_norm: return plume::RenderFormat::R16G16B16A16_UNORM;

	case hl::hh::mirage::raw_vertex_format::udec3:
	case hl::hh::mirage::raw_vertex_format::udec3_norm: return plume::RenderFormat::R32_UINT;

	case hl::hh::mirage::raw_vertex_format::dec3:
	case hl::hh::mirage::raw_vertex_format::dec3_norm: return plume::RenderFormat::R32_SINT;

	case hl::hh::mirage::raw_vertex_format::uhend3:
	case hl::hh::mirage::raw_vertex_format::uhend3_norm:
	case hl::hh::mirage::raw_vertex_format::udhen3:
	case hl::hh::mirage::raw_vertex_format::udhen3_norm: return plume::RenderFormat::R32_UINT;

	case hl::hh::mirage::raw_vertex_format::hend3:
	case hl::hh::mirage::raw_vertex_format::hend3_norm:
	case hl::hh::mirage::raw_vertex_format::dhen3:
	case hl::hh::mirage::raw_vertex_format::dhen3_norm: return plume::RenderFormat::R32_SINT;

	case hl::hh::mirage::raw_vertex_format::float16_2: return plume::RenderFormat::R16G16_FLOAT;
	case hl::hh::mirage::raw_vertex_format::float16_4: return plume::RenderFormat::R16G16B16A16_FLOAT;
	}
}

constexpr static const char* toPlume(const hl::hh::mirage::raw_vertex_type type) {
	switch (type) {
	case hl::hh::mirage::raw_vertex_type::position:     return "POSITION";
	case hl::hh::mirage::raw_vertex_type::blend_weight: return "BLENDWEIGHT";
	case hl::hh::mirage::raw_vertex_type::blend_indices:return "BLENDINDICES";
	case hl::hh::mirage::raw_vertex_type::normal:       return "NORMAL";
	case hl::hh::mirage::raw_vertex_type::psize:        return "PSIZE";
	case hl::hh::mirage::raw_vertex_type::texcoord:     return "TEXCOORD";
	case hl::hh::mirage::raw_vertex_type::tangent:      return "TANGENT";
	case hl::hh::mirage::raw_vertex_type::binormal:     return "BINORMAL";
	case hl::hh::mirage::raw_vertex_type::tess_factor:  return "TESSFACTOR";
	case hl::hh::mirage::raw_vertex_type::position_t:   return "POSITIONT";
	case hl::hh::mirage::raw_vertex_type::color:        return "COLOR";
	case hl::hh::mirage::raw_vertex_type::fog:          return "FOG";
	case hl::hh::mirage::raw_vertex_type::depth:        return "DEPTH";
	case hl::hh::mirage::raw_vertex_type::sample:       return "SAMPLE";
	default: return "";
	}
}

SQInteger ulvl::app::HLModelGetModelData(HSQUIRRELVM vm) {
	hl::hh::mirage::skeletal_model* model{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&model, nullptr, SQFalse);

	ModelData* modelData = new ModelData{};
	for (auto& mesh : model->meshGroups[0].opaq) {
		MeshData& meshData = modelData->meshes.emplace_back();

		for (auto& elem : mesh.vertexElements) meshData.vertexInfo.vertexLayout.emplace_back(toPlume(elem.type), elem.index, 0, toPlume(elem.format), 0, elem.offset);
		meshData.vertexInfo.calcStrideByLayout();

		size_t vertSize{ meshData.vertexInfo.stride * mesh.vertexCount };
		meshData.vertices = new char[vertSize];
		memcpy(meshData.vertices, mesh.vertices.get(), vertSize);
		meshData.vertexCount = mesh.vertexCount;

		meshData.indices = new unsigned short[mesh.faces.size()];
		memcpy(meshData.indices, mesh.faces.data(), mesh.faces.size() * sizeof(unsigned short));
		meshData.indexCount = mesh.faces.size();
	}

	sq_pushroottable(vm);
	sq_pushstring(vm, "ModelData", -1);
	sq_get(vm, -2);
	sq_remove(vm, -2);
	sq_createinstance(vm, -1);
	sq_setinstanceup(vm, -1, modelData);
	sq_setreleasehook(vm, -1, ModelDataReleaseHook);

	return 1;
}

SQInteger ulvl::app::HLModelLoadFilepath(HSQUIRRELVM vm) {
	const char* filepath{ nullptr };
	sq_getstring(vm, 2, &filepath);

	std::filesystem::path path{ filepath };

	sq_pushroottable(vm);
	sq_pushstring(vm, "HLModel", -1);
	sq_get(vm, -2);
	sq_remove(vm, -2);
	sq_createinstance(vm, -1);
	hl::hh::mirage::skeletal_model* retVal = new hl::hh::mirage::skeletal_model{ path };
	sq_setinstanceup(vm, -1, retVal);
	sq_setreleasehook(vm, -1, HLModelReleaseHook);

	return 1;
}

SQInteger ulvl::app::HLModelReleaseHook(SQUserPointer p, SQInteger size) {
	delete (hl::hh::mirage::skeletal_model*)p;
	return 0;
}

// DebugVisual

SQInteger ulvl::app::DebugVisualDrawCube(HSQUIRRELVM vm) {
	DebugVisualService* debugVisual{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&debugVisual, nullptr, SQFalse);

	glm::vec4* color{};
	sq_getinstanceup(vm, 2, (SQUserPointer*)&color, nullptr, SQFalse);

	glm::vec3* pos{};
	sq_getinstanceup(vm, 3, (SQUserPointer*)&pos, nullptr, SQFalse);

	glm::vec4* rot{};
	sq_getinstanceup(vm, 4, (SQUserPointer*)&rot, nullptr, SQFalse);

	glm::vec3* scale{};
	sq_getinstanceup(vm, 5, (SQUserPointer*)&scale, nullptr, SQFalse);

	ObjectService::Object* object{};
	sq_getinstanceup(vm, 6, (SQUserPointer*)&object, nullptr, SQFalse);

	gfx::InstancedMesh mesh{};
	mesh.id = (int)object;
	mesh.color = *color;
	mesh.worldMatrix = glm::translate(glm::mat4{ 1 }, *pos) * glm::toMat4(*(glm::quat*)rot) * glm::scale(glm::mat4{ 1 }, *scale);
	debugVisual->addCube(mesh);

	return 0;
}

SQInteger ulvl::app::DebugVisualDrawSphere(HSQUIRRELVM vm) {
	DebugVisualService* debugVisual{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&debugVisual, nullptr, SQFalse);

	glm::vec4* color{};
	sq_getinstanceup(vm, 2, (SQUserPointer*)&color, nullptr, SQFalse);

	glm::vec3* pos{};
	sq_getinstanceup(vm, 3, (SQUserPointer*)&pos, nullptr, SQFalse);

	float radius{ 0 };
	sq_getfloat(vm, 4, &radius);

	ObjectService::Object* object{};
	sq_getinstanceup(vm, 5, (SQUserPointer*)&object, nullptr, SQFalse);

	gfx::InstancedMesh mesh{};
	mesh.id = (int)object;
	mesh.color = *color;
	mesh.worldMatrix = glm::translate(glm::mat4{ 1 }, *pos) * glm::scale(glm::mat4{ 1 }, glm::vec3{ radius * 2 });
	debugVisual->addSphere(mesh);

	return 0;
}

SQInteger ulvl::app::DebugVisualDrawCylinder(HSQUIRRELVM vm) {
	DebugVisualService* debugVisual{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&debugVisual, nullptr, SQFalse);

	glm::vec4* color{};
	sq_getinstanceup(vm, 2, (SQUserPointer*)&color, nullptr, SQFalse);

	glm::vec3* pos{};
	sq_getinstanceup(vm, 3, (SQUserPointer*)&pos, nullptr, SQFalse);

	glm::vec4* rot{};
	sq_getinstanceup(vm, 4, (SQUserPointer*)&rot, nullptr, SQFalse);

	float radius{ 0 };
	sq_getfloat(vm, 5, &radius);

	float height{ 0 };
	sq_getfloat(vm, 6, &height);

	ObjectService::Object* object{};
	sq_getinstanceup(vm, 7, (SQUserPointer*)&object, nullptr, SQFalse);

	gfx::InstancedMesh mesh{};
	mesh.id = (int)object;
	mesh.color = *color;
	mesh.worldMatrix = glm::translate(glm::mat4{ 1 }, *pos) * glm::toMat4(*(glm::quat*)rot) * glm::scale(glm::mat4{ 1 }, glm::vec3{ radius * 2, height, radius * 2 });
	debugVisual->addCylinder(mesh);

	return 0;
}

SQInteger ulvl::app::DebugVisualDrawLine(HSQUIRRELVM vm) {
	DebugVisualService* debugVisual{};
	sq_getinstanceup(vm, 1, (SQUserPointer*)&debugVisual, nullptr, SQFalse);

	glm::vec4* color{};
	sq_getinstanceup(vm, 2, (SQUserPointer*)&color, nullptr, SQFalse);

	int curIdx{ 3 };

	std::vector<glm::vec3> glmPos{};
	glm::vec3* rawPos{ nullptr };
	glm::vec4* rawRot{ nullptr };

	auto type = sq_gettype(vm, curIdx);
	if (type == SQObjectType::OT_INSTANCE) {
		sq_getinstanceup(vm, curIdx, (SQUserPointer*)&rawPos, nullptr, SQFalse);
		curIdx++;

		sq_getinstanceup(vm, curIdx, (SQUserPointer*)&rawRot, nullptr, SQFalse);
		curIdx++;
	}

	HSQOBJECT positions;
	sq_resetobject(&positions);
	sq_getstackobj(vm, curIdx, &positions);
	SQInteger posLen = sq_getsize(vm, curIdx);
	for (auto x = 0; x < posLen; x++) {
		sq_pushinteger(vm, x);
		if (SQ_SUCCEEDED(sq_get(vm, curIdx))) {
			glm::vec3* pos{};
			sq_getinstanceup(vm, -1, (SQUserPointer*)&pos, nullptr, SQFalse);
			glmPos.push_back(*pos);
		}
		sq_pop(vm, 1);
	}
	curIdx++;

	glm::vec3 pos{ 0,0,0 };
	if (rawPos)
		pos = *rawPos;

	glm::quat rot{ 1,0,0,0 };
	if (rawRot)
		rot = *(glm::quat*)rawRot;

	ObjectService::Object* object{};
	sq_getinstanceup(vm, curIdx, (SQUserPointer*)&object, nullptr, SQFalse);

	app::DebugVisualService::LineDesc mesh{};
	mesh.id = (int)object;
	mesh.positions = std::move(glmPos);
	mesh.worldTransform = glm::translate(glm::mat4{ 1 }, pos) * glm::toMat4(rot);
	mesh.color = *color;
	debugVisual->addLine(mesh);

	return 0;
}

// math

SQInteger ulvl::app::sqrt(HSQUIRRELVM vm) {
	float value{ 0 };
	sq_getfloat(vm, 1, &value);

	sq_pushfloat(vm, sqrtf(value));

	return 1;
}

SQInteger ulvl::app::fmax(HSQUIRRELVM vm) {
	float value{ 0 };
	sq_getfloat(vm, 1, &value);
	float max{ 0 };
	sq_getfloat(vm, 2, &max);

	sq_pushfloat(vm, fmaxf(value, max));

	return 1;
}

SQInteger ulvl::app::fmin(HSQUIRRELVM vm) {
	float value{ 0 };
	sq_getfloat(vm, 1, &value);
	float max{ 0 };
	sq_getfloat(vm, 2, &max);

	sq_pushfloat(vm, fminf(value, max));

	return 1;
}

SQInteger ulvl::app::fabs(HSQUIRRELVM vm) {
	float value{ 0 };
	sq_getfloat(vm, 1, &value);

	sq_pushfloat(vm, fabsf(value));

	return 1;
}
