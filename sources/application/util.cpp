#include "application/util.h"

glm::mat4x4 ai_to_glm(const aiMatrix4x4& from)
{
  return glm::mat4x4(
    (double)from.a1, (double)from.b1, (double)from.c1, (double)from.d1,
    (double)from.a2, (double)from.b2, (double)from.c2, (double)from.d2,
    (double)from.a3, (double)from.b3, (double)from.c3, (double)from.d3,
    (double)from.a4, (double)from.b4, (double)from.c4, (double)from.d4
  );
}