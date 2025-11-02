#pragma once
#include <glm/glm.hpp>

namespace ulvl {
	bool Editor(const char* label, glm::vec2& value);
	bool Editor(const char* label, glm::vec3& value);
	bool Editor(const char* label, glm::vec4& value);
	bool Editor(const char* label, glm::quat& value);
	bool Editor(const char* label, glm::mat4& value);
}
