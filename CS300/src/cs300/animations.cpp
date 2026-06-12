/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen
Institute of Technology is prohibited.
Language: C++
Platform: Windows
Author: Jon Sanchez
End Header --------------------------------------------------------*/
#include "animations.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Animations {

glm::vec3 Anim::Update(const glm::vec3& pos, float time) const {
	return updater(pos, param, time);
}

glm::vec3 Sinusoidal(const glm::vec3& pos, const glm::vec3& param, float time) {
	return pos + glm::sin(param.x + time * param.y) * glm::vec3(0.0f, param.z, 0.0f);
}

glm::vec3 Orbit(const glm::vec3& pos, const glm::vec3& center, float time) {
	glm::vec3 radialVec = pos - center;
	float r = glm::length(radialVec);
	glm::vec3 rotationaxis = glm::cross(glm::cross(radialVec, glm::vec3(0.0f, 1.0f, 0.0f)), radialVec);
	glm::vec4 rotatedRadialVec = glm::rotate(glm::mat4(1.0f), time, rotationaxis) * glm::vec4(radialVec, 0.0f);

	return center + glm::vec3(rotatedRadialVec);
}

glm::vec3 Apply(const glm::vec3& original, const std::vector<Anim>& anims, float time) {
	glm::vec3 pos = original;
	for (const auto& anim : anims) {
		pos = anim.Update(pos, time);
	}
	return pos;
}

}    // namespace Animations
