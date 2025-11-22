#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace ulvl::math {
	static glm::mat4 flipZ(glm::mat4& m) {
		glm::vec3 scaleVec{ 1, 1, -1 };
		auto scaled = glm::scale(m, scaleVec);
		scaled[3].z *= -1;
		return scaled;
	}

	static void flipZ(glm::vec3& v) {
		v.z *= -1;
	}

	static void flipZ(glm::quat& v) {
		v.x *= -1;
		v.y *= -1;
	}

	static void flipZEuler(glm::vec3& v) {
		glm::quat q{ v };
		q.x *= -1;
		q.y *= -1;
		v = glm::eulerAngles(q);
	}
}
