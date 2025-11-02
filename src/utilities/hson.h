#pragma once
#include <hedgelib/sets/hl_hson.h>
#include <hedgelib/sets/hl_set_obj_type.h>

namespace ulvl::ut {
	hl::hson::parameter createParameterByType(const char* type);
}
