#pragma once
#include <squirrel.h>
#include "squirrelwrap.h"

#define SQFUNC(FUNC) SQInteger FUNC##(HSQUIRRELVM vm)
#define RELEASE_HOOK(NAME) SQInteger NAME##ReleaseHook(SQUserPointer p, SQInteger size)

namespace ulvl::app {
	// ----------
	//	Utilities
	// ----------

	SQFUNC(ToBytes);

	// std::string
	SQFUNC(StringCtor);
	RELEASE_HOOK(String);
	SQFUNC(StringToCString);

	// ------
	//	HSON
	// ------

	// hl::radix_tree<hl::hson::parameter>
	SQFUNC(GetParameterFloat);
	SQFUNC(GetParameterInt);
	SQFUNC(GetParameterUInt);
	SQFUNC(GetParameterString);
	SQFUNC(GetParameterArray);
	SQFUNC(GetParameterObject);
	RELEASE_HOOK(RadixTree);

	// Object
	SQFUNC(GetObjLocalPosition);
	SQFUNC(GetObjWorldPosition);
	SQFUNC(GetObjRotation);
	SQFUNC(GetObjLinear);
	SQFUNC(GetObjName);
	SQFUNC(GetObjParameters);
	SQFUNC(GetObjParent);
	SQFUNC(GetObjID);
	SQFUNC(GetObjHasParent);

	SQFUNC(GetObjByID);

	// -----
	//	glm
	// -----

	// glm::vec3
	SQFUNC(Vec3Ctor);
	RELEASE_HOOK(Vec3);
	SQFUNC(Vec3GetX);
	SQFUNC(Vec3SetX);
	SQFUNC(Vec3GetY);
	SQFUNC(Vec3SetY);
	SQFUNC(Vec3GetZ);
	SQFUNC(Vec3SetZ);
	SQFUNC(Vec3RotateByQuat);
	SQFUNC(Vec3Normalize);
	SQFUNC(Vec3Add);
	SQFUNC(Vec3Sub);
	SQFUNC(Vec3Multiply);
	SQFUNC(Vec3MultiplyMat3);
	SQFUNC(Vec3Divide);
	SQFUNC(Vec3DistanceSq);
	SQFUNC(Vec3Length);

	// glm::vec4
	SQFUNC(Vec4Ctor);
	RELEASE_HOOK(Vec4);
	SQFUNC(Vec4GetW);
	SQFUNC(Vec4SetW);

	// glm::mat3
	RELEASE_HOOK(Mat3);

	// -----------
	//	ModelData
	// -----------

	SQFUNC(ModelDataGetVertexCount);
	SQFUNC(ModelDataSetVertexCount);
	SQFUNC(ModelDataGetVertexStride);
	SQFUNC(ModelDataSetVertexStride);
	SQFUNC(ModelDataSetVertices);
	SQFUNC(ModelDataGetIndexCount);
	SQFUNC(ModelDataSetIndexCount);
	SQFUNC(ModelDataSetIndices);

	// -------------
	//	DebugVisual
	// -------------

	SQFUNC(DebugVisualDrawCube);
	SQFUNC(DebugVisualDrawSphere);
	SQFUNC(DebugVisualDrawCylinder);
	SQFUNC(DebugVisualDrawLine);

	// ------
	//	math
	// ------

	SQFUNC(sqrt);
	SQFUNC(fmax);
	SQFUNC(fmin);
	SQFUNC(fabs);

	static void registerFuncs(SquirrelWrap& wrap) {
		wrap.RegisterFunc(ToBytes);
		wrap.RegisterFunc(GetObjByID);
		wrap.RegisterFunc(sqrt);
		wrap.RegisterFunc(fmax);
		wrap.RegisterFunc(fmin);
		wrap.RegisterFunc(fabs);
	}
}
