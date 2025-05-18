#pragma once
#include <map>
#include <memory>
#include <string>
#include <span>
#include "3dmath.h"
#include <vector>
#include <assimp/defs.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <list>


struct Mesh
{
  struct Bone
  {
    std::string name;
    glm::mat4x4 bindPose;
    glm::mat4x4 invBindPose;
    float weight = 0;

    Bone(const ai_real* matrix_value, const char* name_cstr) {
      glm::mat4x4 mOffsetMatrix = glm::make_mat4x4(matrix_value);
      mOffsetMatrix = glm::transpose(mOffsetMatrix);
      invBindPose = mOffsetMatrix;
      bindPose = glm::inverse(mOffsetMatrix);
      name = name_cstr;
    }
  };

  struct Node
  {
    aiNode* self; // TODO dont use ever

    glm::mat4x4 nodeTransform = glm::mat4x4(1.0f);
    glm::mat4x4 nodeTransformAcc = glm::mat4x4(1.0f);

    Node* parent;
    std::vector<Node*> children;
    std::vector<Bone> bones;

    glm::vec3 get_position(const glm::mat4x4& transform) const {
      glm::vec3 result = glm::vec3(0.f);
      for (const Bone& bone : bones) {
        result += glm::vec3((transform * bone.bindPose)[3]);
      }
      return result;
    }

    Node(aiNode* self) : self(self), parent(nullptr) {
      nodeTransform = glm::mat4x4(glm::make_mat4x4(&(self->mTransformation).a1));
    }
  };

  std::string name;
  std::vector<Bone> bones;
  std::list<Node> nodes;
  std::list<Node> nodesArmature;
  const uint32_t vertexArrayBufferObject;
  const int numIndices;

  Mesh(const char *name, uint32_t vertexArrayBufferObject, int numIndices, const std::vector<Bone>& bones, const std::list<Node>& nodes, const std::list<Node>& nodesArmature) :
    name(name),
    vertexArrayBufferObject(vertexArrayBufferObject),
    numIndices(numIndices),
    bones(bones),
    nodes(nodes),
    nodesArmature(nodesArmature)
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