#include "scene.h"
#include <iostream>
#include "engine/render/debug_arrow.h"

void render_arrows(const std::vector<Mesh::Bone>& bones, const mat4& transform) {
  for (const Mesh::Bone& bone : bones) {
    // std::cout << "bone " << bone.name << std::endl;
    DebugArrow::add_arrow(transform * bone.bindPose, vec3(0), vec3(0.1f, 0, 0), vec3(1, 0, 0), 0.01f);
    DebugArrow::add_arrow(transform * bone.bindPose, vec3(0), vec3(0, 0.1f, 0), vec3(0, 1, 0), 0.01f);
    DebugArrow::add_arrow(transform * bone.bindPose, vec3(0), vec3(0, 0, 0.1f), vec3(0, 0, 1), 0.01f);
    for (const auto& bone_child : bone.children) {
      DebugArrow::add_arrow(glm::vec3((transform * bone.bindPose)[3]), glm::vec3((transform * bone.bindPose * bone_child.bindPose)[3]), vec3(0, 0.5f, 0.5f), 0.03f);
    }
  }
}

void render(const MeshPtr& mesh)
{
  glBindVertexArray(mesh->vertexArrayBufferObject);
  glDrawElementsBaseVertex(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, 0, 0);
}

void render_character(const Character &character, const mat4 &cameraProjView, vec3 cameraPosition, const DirectionLight &light)
{
  const Material &material = *character.material;
  const Shader &shader = material.get_shader();

  shader.use();
  material.bind_uniforms_to_shader();
  shader.set_mat4x4("Transform", character.transform);
  shader.set_mat4x4("ViewProjection", cameraProjView);
  shader.set_vec3("CameraPosition", cameraPosition);
  shader.set_vec3("LightDirection", glm::normalize(light.lightDirection));
  shader.set_vec3("AmbientLight", light.ambient);
  shader.set_vec3("SunLight", light.lightColor);

  for (const MeshPtr& mesh : character.meshes) {
    render(mesh);
    render_arrows(mesh->bones, character.transform);
  }
}

void application_render(Scene &scene)
{
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  const float grayColor = 0.3f;
  glClearColor(grayColor, grayColor, grayColor, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  const mat4 &projection = scene.userCamera.projection;
  const glm::mat4 &transform = scene.userCamera.transform;
  mat4 projView = projection * inverse(transform);

  for (const Character& character : scene.characters)
    render_character(character, projView, glm::vec3(transform[3]), scene.light);

  DebugArrow::render(projView, glm::vec3(transform[3]), scene.light);
}
