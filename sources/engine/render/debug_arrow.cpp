#include "debug_arrow.h"

std::unique_ptr<DebugArrow> DebugArrow::_instance = nullptr;

static void add_triangle(vec3 a, vec3 b, vec3 c, std::vector<uint>& indices, std::vector<vec3>& vert, std::vector<vec3>& normal)
{
  // takes coordinates of a triangle, pushes them appropriately to vectors
  uint k = vert.size();
  vec3 n = normalize(cross(b - a, c - a));
  indices.push_back(k);
  indices.push_back(k + 2);
  indices.push_back(k + 1);
  vert.push_back(a);
  vert.push_back(b);
  vert.push_back(c);
  normal.push_back(n);
  normal.push_back(n);
  normal.push_back(n);
}


DebugArrow::DebugArrow() {
  // Create material
  arrowMaterial = make_material("arrow", "sources/shaders/arrow_vs.glsl", "sources/shaders/arrow_ps.glsl");

  // Create mesh out of triangles pointing towards C and a "handle"
  std::vector<uint> indices;
  std::vector<vec3> vert;
  std::vector<vec3> normal;
  vec3 c = vec3(1, 0, 0);
  const int N = 4;
  vec3 p[N];
  for (int i = 0; i < N; i++)
  {
    float a1 = ((float)(i) / N) * 2 * PI;
    float a2 = ((float)(i + 1) / N) * 2 * PI;
    vec3 p1 = p[i] = vec3(0, cos(a1), sin(a1));
    vec3 p2 = vec3(0, cos(a2), sin(a2));
    add_triangle(p2, p1, c, indices, vert, normal);
  }
  add_triangle(p[0], p[1], p[2], indices, vert, normal);
  add_triangle(p[0], p[2], p[3], indices, vert, normal);
  arrowMesh = create_mesh("arrow", indices, vert, normal);
}

void DebugArrow::add_arrow(const glm::mat4x4& transform, const vec3& from, const vec3& to, const vec3 color, float size) {
  instance()->_add_arrow(transform, from, to, color, size);
}
void DebugArrow::add_arrow(const vec3& from, const vec3& to, const vec3 color, float size) {
  instance()->_add_arrow(from, to, color, size);
}

void DebugArrow::render(const mat4& cameraProjView, vec3 cameraPosition, const DirectionLight& light) {
  instance()->_render(cameraProjView, cameraPosition, light);
}

void DebugArrow::_add_arrow(const glm::mat4x4& transform, const vec3& from, const vec3& to, const vec3 color, float size) {
  _add_arrow(transform * vec4(from, 1), transform * vec4(to, 1), color, size);
}
void DebugArrow::_add_arrow(const vec3& from, const vec3& to, const vec3 color, float size) {
  arrows.push_back(Arrow(from, to, color, size));
}

void DebugArrow::_render(const mat4& cameraProjView, vec3 cameraPosition, const DirectionLight& light) {
  if (arrows.empty()) return;

  // Manage shader
  const auto& shader = arrowMaterial->get_shader();
  glDepthFunc(GL_ALWAYS);
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  shader.use();

  shader.set_mat4x4("ViewProjection", cameraProjView);
  shader.set_vec3("CameraPosition", cameraPosition);
  shader.set_vec3("LightDirection", glm::normalize(light.lightDirection));
  shader.set_vec3("AmbientLight", light.ambient);
  shader.set_vec3("SunLight", light.lightColor);

  // Render the arrows themselves
  for (int i = 0; i < arrows.size(); i += 1)
  {
    shader.set_mat4x4("ArrowTm", arrows[i].calc_transform());
    shader.set_vec4("ArrowColor", arrows[i].calc_color());
    glBindVertexArray(arrowMesh->vertexArrayBufferObject);
    glDrawElementsBaseVertex(GL_TRIANGLES, arrowMesh->numIndices, GL_UNSIGNED_INT, 0, 0);
  }

  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);

  // Reset arrows
  arrows.clear();
}
