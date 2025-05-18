#include "scene.h"
#include "engine/render/debug_arrow.h"

#include <list>
#include <iostream>


void render_arrows_bones(const std::vector<Mesh::Bone>& bones, const mat4& transform) {
  const float length = 0.3f;
  const float width = 0.01f;
  for (const Mesh::Bone& bone : bones) {
    DebugArrow::add_arrow(transform * bone.bindPose, vec3(0), vec3(length * bone.weight, 0, 0), vec3(1, 0, 0), width);
    DebugArrow::add_arrow(transform * bone.bindPose, vec3(0), vec3(0, length * bone.weight, 0), vec3(0, 1, 0), width);
    DebugArrow::add_arrow(transform * bone.bindPose, vec3(0), vec3(0, 0, length * bone.weight), vec3(0, 0, 1), width);
  }
}

void render_arrows_nodes(std::list<Mesh::Node>& nodes, const mat4& transform, vec3 color) {
  if (nodes.empty()) return;
  for (Mesh::Node& node : nodes) {
    node.nodeTransformAcc = node.nodeTransform;
  }
  for (Mesh::Node& node: nodes) {
    for (auto& node_child : node.children) {
      // calculate transform
      node_child->nodeTransformAcc = node.nodeTransformAcc * node_child->nodeTransform;

      // calculate positions by averaging bones
      glm::vec3 position = node.get_position(transform);
      glm::vec3 position_child = node_child->get_position(transform);

      // draw the skeleton (parent->child)
      DebugArrow::add_arrow(position, position_child, vec3(0, 0.5f, 0.5f), 0.03f);
      // draw the orientation
      vec3 vector = vec3(0.1, 0, 0);
      DebugArrow::add_arrow(position, position + vec3((node_child->nodeTransformAcc * vec4(vector, 1))), color, 0.01f);
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
    render_arrows_bones(mesh->bones, character.transform);
    render_arrows_nodes(mesh->nodes, character.transform, vec3(0.5f, 0, 0.5f));
    render_arrows_nodes(mesh->nodesArmature, character.transform, vec3(1.0f, 0, 1.0f));
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
