#pragma once
#include <map>
#include <memory>
#include <string>
#include <span>
#include "3dmath.h"
#include <vector>


struct Mesh
{
  struct Bone
  {
    std::string name;
    glm::mat4x4 bindPose;
    glm::mat4x4 invBindPose;
    std::vector<Bone> children;
  };

  std::string name;
  std::vector<Bone> bones;
  const uint32_t vertexArrayBufferObject;
  const int numIndices;

  Mesh(const char *name, uint32_t vertexArrayBufferObject, int numIndices, const std::vector<Bone>& bones) :
    name(name),
    vertexArrayBufferObject(vertexArrayBufferObject),
    numIndices(numIndices),
    bones(bones)
    {}
};

using MeshPtr = std::shared_ptr<Mesh>;

MeshPtr create_mesh(
  const char* name,
  std::span<const uint32_t> indices,
  std::span<const vec3> vertices,
  std::span<const vec3> normals);

MeshPtr create_mesh(
    const char *name,
    std::span<const uint32_t> indices,
    std::span<const vec3> vertices,
    std::span<const vec3> normals,
    std::span<const vec2> uv,
    std::span<const vec4> weights,
    std::span<const uvec4> weightsIndex);

MeshPtr create_mesh(
    const char *name,
    std::span<const uint32_t> indices,
    std::span<const vec3> vertices,
    std::span<const vec3> normals,
    std::span<const vec2> uv);

MeshPtr make_plane_mesh();

void render(const MeshPtr &mesh);