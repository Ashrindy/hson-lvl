#pragma once
#include <hedgelib/sets/hl_hson.h>
#include <hedgelib/sets/hl_set_obj_type.h>

namespace ulvl::ut {
	hl::hson::parameter createParameterByType(const char* type);
	inline hl::hson::parameter createParameterByType(const std::string& type) { return createParameterByType(type.c_str()); }
	hl::hson::parameter createParam(hl::reflect::field_definition& fieldDef);
	hl::radix_tree<hl::hson::parameter> createStruct(hl::reflect::struct_definition& structDef);
}
