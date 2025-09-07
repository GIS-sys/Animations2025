#pragma once
#include "3dmath.h"
#include "material.h"
#include "mesh.h"
#include "application/scene.h"


static mat4 directionMatrix(vec3 from, vec3 to)
{
  from = normalize(from);
  to = normalize(to);
  quat q(from, to);
  return toMat4(q);
}


class DebugArrow
{
private:
  struct Arrow
  {
    const vec3 from;
    const vec3 to;
    const vec3 color;
    float size;

    mat4 calc_transform() const {
      vec3 d = to - from;
      mat4 t = translate(mat4(1.f), from);
      float len = length(d);
      mat4 s = scale(mat4(1.f), vec3(len, size, size));

      mat4 r = directionMatrix(vec3(1, 0, 0), d);
      return t * r * s;
    }

    vec4 calc_color() const {
      return vec4(color, 1.0);
    }
  };

  MaterialPtr arrowMaterial;
  MeshPtr arrowMesh;

  std::vector<Arrow> arrows;

  static std::unique_ptr<DebugArrow> _instance;
  static std::unique_ptr<DebugArrow>& instance() {
    if (!_instance) {
      _instance = std::make_unique<DebugArrow>();
    }
    return _instance;
  }

  void _add_arrow(const glm::mat4x4& transform, const vec3& from, const vec3& to, const vec3 color, float size);
  void _add_arrow(const vec3& from, const vec3& to, const vec3 color, float size);
  void _render(const mat4& cameraProjView, vec3 cameraPosition, const DirectionLight& light);

public:
  DebugArrow();

  static void add_arrow(const glm::mat4x4& transform, const vec3& from, const vec3& to, const vec3 color, float size);
  static void add_arrow(const vec3& from, const vec3& to, const vec3 color, float size);
  static void render(const mat4& cameraProjView, vec3 cameraPosition, const DirectionLight& light);
};