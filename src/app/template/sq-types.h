#pragma once
#include <squirrel.h>
#include "sq-funcs.h"

#define NEW_CLASS(NAME, REGISTER_BODY) { \
	sq_pushroottable(vm); \
	sq_pushstring(vm, #NAME, -1); \
	sq_newclass(vm, SQFalse); \
	REGISTER_BODY \
	sq_newslot(vm, -3, SQFalse); \
	sq_pop(vm, 1); \
}

#define NEW_ENUM(NAME, REGISTER_BODY) { \
	sq_pushroottable(vm); \
	sq_pushstring(vm, #NAME, -1); \
	sq_newtable(vm); \
	REGISTER_BODY \
	sq_newslot(vm, -3, SQFalse); \
	sq_pop(vm, 1); \
}

#define NEW_FUNC(NAME, FUNC) sq_pushstring(vm, NAME, -1); sq_newclosure(vm, FUNC, 0); sq_newslot(vm, -3, SQFalse);
#define NEW_MEMBER_RW(NAME, GET, SET) NEW_FUNC(NAME, GET) NEW_FUNC(NAME, SET)
#define NEW_MEMBER_R(NAME, GET) NEW_FUNC(NAME, GET)
#define NEW_ENUM_VALUE(NAME, VALUE) sq_pushstring(vm, NAME, -1); sq_pushinteger(vm, (SQInteger)VALUE); sq_newslot(vm, -3, SQFalse);

#define NEW_RENDERFORMAT_TYPE(NAME, TYPE) NEW_ENUM_VALUE(XSTR(NAME##_##TYPE), plume::RenderFormat::NAME##_##TYPE)
#define NEW_RENDERFORMAT_FSUT(NAME) NEW_RENDERFORMAT_TYPE(NAME, FLOAT) NEW_RENDERFORMAT_TYPE(NAME, SINT) NEW_RENDERFORMAT_TYPE(NAME, UINT) NEW_RENDERFORMAT_TYPE(NAME, TYPELESS)
#define NEW_RENDERFORMAT_SUT(NAME) NEW_RENDERFORMAT_TYPE(NAME, SINT) NEW_RENDERFORMAT_TYPE(NAME, UINT) NEW_RENDERFORMAT_TYPE(NAME, TYPELESS)

namespace ulvl::app {
	void registerTypes(SquirrelWrap& wrap) {
		auto& vm = wrap.vm;

		NEW_CLASS(
			String,
			NEW_FUNC("constructor", StringCtor)
			NEW_FUNC("c_str", StringToCString)
		);

		NEW_CLASS(
			Vec3, 
			NEW_FUNC("constructor", Vec3Ctor)
			NEW_MEMBER_R("x", Vec3GetX, Vec3SetX)
			NEW_MEMBER_R("y", Vec3GetY, Vec3SetY)
			NEW_MEMBER_R("z", Vec3GetZ, Vec3SetZ)
			NEW_FUNC("rotate", Vec3RotateByQuat)
			NEW_FUNC("normalize", Vec3Normalize)
			NEW_FUNC("multiplyMat3", Vec3MultiplyMat3)
			NEW_FUNC("_add", Vec3Add)
			NEW_FUNC("_sub", Vec3Sub)
			NEW_FUNC("_mul", Vec3Multiply)
			NEW_FUNC("_div", Vec3Divide)
			NEW_FUNC("distancesq", Vec3DistanceSq)
			NEW_FUNC("length", Vec3Length)
		);

		NEW_CLASS(
			Vec4,
			NEW_FUNC("constructor", Vec4Ctor)
			NEW_MEMBER_R("x", Vec3GetX, Vec3SetX)
			NEW_MEMBER_R("y", Vec3GetY, Vec3SetY)
			NEW_MEMBER_R("z", Vec3GetZ, Vec3SetZ)
			NEW_MEMBER_R("w", Vec4GetW, Vec4SetW)
		);

		NEW_CLASS(
			Mat3,
			{}
		);

		NEW_CLASS(
			Parameters,
			NEW_FUNC("GetParameterFloat", GetParameterFloat)
			NEW_FUNC("GetParameterInt", GetParameterInt)
			NEW_FUNC("GetParameterUInt", GetParameterUInt)
			NEW_FUNC("GetParameterString", GetParameterString)
			NEW_FUNC("GetParameterArray", GetParameterArray)
			NEW_FUNC("GetParameterObject", GetParameterObject)
		);

		NEW_CLASS(
			Object,
			NEW_MEMBER_R("position", GetObjWorldPosition)
			NEW_MEMBER_R("rotation", GetObjRotation)
			NEW_MEMBER_R("linear", GetObjLinear)
			NEW_MEMBER_R("localPosition", GetObjLocalPosition)
			NEW_MEMBER_R("name", GetObjName)
			NEW_MEMBER_R("parameters", GetObjParameters)
			NEW_MEMBER_R("parent", GetObjParent)
			NEW_MEMBER_R("id", GetObjID)
			NEW_FUNC("HasParent", GetObjHasParent)
		);

		NEW_ENUM(
			VertexFormat,
			NEW_RENDERFORMAT_SUT(R8)
			NEW_RENDERFORMAT_FSUT(R16)
			NEW_RENDERFORMAT_FSUT(R16G16)
			NEW_RENDERFORMAT_FSUT(R32)
			NEW_RENDERFORMAT_FSUT(R32G32)
			NEW_RENDERFORMAT_FSUT(R32G32B32)
			NEW_RENDERFORMAT_FSUT(R32G32B32A32)
		);

		NEW_CLASS(
			VertexElement,
			NEW_FUNC("constructor", VertexElementCtor)
		);

		NEW_CLASS(
			MeshData,
			NEW_FUNC("constructor", MeshDataCtor)

			NEW_FUNC("GetVertexCount", MeshDataGetVertexCount)
			NEW_FUNC("SetVertexCount", MeshDataSetVertexCount)
			NEW_FUNC("SetVertices", MeshDataSetVertices)

			NEW_FUNC("GetIndexCount", MeshDataGetIndexCount)
			NEW_FUNC("SetIndexCount", MeshDataSetIndexCount)
			NEW_FUNC("SetIndices", MeshDataSetIndices)

			NEW_FUNC("AddVertexElement", MeshDataAddVertexElement)
			NEW_FUNC("SetVertexLayout", MeshDataSetVertexLayout)
			NEW_FUNC("GetVertexStride", MeshDataGetVertexStride)
		);

		NEW_CLASS(
			ModelData,
			NEW_FUNC("AddMesh", ModelDataAddMesh)
		);

		NEW_CLASS(
			DebugVisual,
			NEW_FUNC("DrawCube", DebugVisualDrawCube)
			NEW_FUNC("DrawSphere", DebugVisualDrawSphere)
			NEW_FUNC("DrawCylinder", DebugVisualDrawCylinder)
			NEW_FUNC("DrawLine", DebugVisualDrawLine)
		);

		NEW_CLASS(
			HLModel,
			NEW_FUNC("GetModelData", HLModelGetModelData)
		);
	}
}
