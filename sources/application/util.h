#pragma once

#include <glm/glm.hpp>
#include <assimp/scene.h>

glm::mat4x4 ai_to_glm(const aiMatrix4x4& from);