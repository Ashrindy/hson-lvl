#include "hson.h"
#include "../app.h"
#include "../app/template-manager.h"

hl::hson::parameter ulvl::ut::createParameterByType(const char* type) {
	hl::hson::parameter param{};
	auto* app = ulvl::Application::instance;
	auto* tem = app->getService<ulvl::app::TemplateManager>()->currentTemplate;

	if (hl::reflect::is_bool_type(type))
	{
		param = hl::hson::parameter{ hl::hson::parameter_type::boolean };
	}
	else if (hl::reflect::is_int_type(type))
	{
		param = hl::hson::parameter{ hl::hson::parameter_type::signed_integer };
	}
	else if (hl::reflect::is_uint_type(type))
	{
		param = hl::hson::parameter{ hl::hson::parameter_type::unsigned_integer };
	}
	else if (hl::reflect::is_floating_type(type))
	{
		param = hl::hson::parameter{ hl::hson::parameter_type::floating };
	}
	else if (hl::reflect::is_string_type(type))
	{
		param = hl::hson::parameter{ hl::hson::parameter_type::string };
	}
	else if (hl::reflect::is_array_type(type))
	{
		param = hl::hson::parameter{ hl::hson::parameter_type::array };
	}
	else if (hl::reflect::is_object_reference_type(type))
	{
		param = hl::hson::parameter{ hl::hson::parameter_type::string };
		param.value_string() = hl::guid{ nullptr }.as_string();
	}
	else if (hl::reflect::is_vec2_type(type))
	{
		param = hl::hson::parameter{ hl::hson::parameter_type::array };
		for (unsigned int x = 0; x < 2; x++)
			param.value_array().push_back(hl::hson::parameter{ hl::hson::parameter_type::floating });
	}
	else if (hl::reflect::is_vec3_type(type))
	{
		param = hl::hson::parameter{ hl::hson::parameter_type::array };
		for (unsigned int x = 0; x < 3; x++)
			param.value_array().push_back(hl::hson::parameter{ hl::hson::parameter_type::floating });
	}
	else if (hl::reflect::is_vec4_type(type) || hl::reflect::is_quat_type(type))
	{
		param = hl::hson::parameter{ hl::hson::parameter_type::array };
		for (unsigned int x = 0; x < 4; x++)
			param.value_array().push_back(hl::hson::parameter{ hl::hson::parameter_type::floating });

		if (hl::reflect::is_quat_type(type))
			param.value_array()[3].value_floating() = 1;
	}
	else if (tem->hsonTemplate->enums.contains(type)) {
		auto enu = tem->hsonTemplate->enums.get(type);

		param = hl::hson::parameter{ hl::hson::parameter_type::string };
		param.value_string() = enu->get_name_of_value(0);
	}
	else if (tem->hsonTemplate->structs.contains(type)) {
		auto stru = tem->hsonTemplate->structs.get(type);

		param = hl::hson::parameter{ hl::hson::parameter_type::object };
		for (auto fld : tem->hsonTemplate->structs.get(stru->parent)->fields)
			param.value_object().insert(fld.name, createParameterByType(fld.type().c_str()));
		for (auto fld : stru->fields)
			param.value_object().insert(fld.name, createParameterByType(fld.type().c_str()));
	}

	return param;
}
