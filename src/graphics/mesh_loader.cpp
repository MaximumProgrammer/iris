#include "graphics/mesh_loader.h"

#include <cstdint>
#include <stack>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "core/exception.h"
#include "core/matrix4.h"
#include "core/quaternion.h"
#include "core/transform.h"
#include "core/vector3.h"
#include "graphics/animation.h"
#include "graphics/bone.h"
#include "graphics/skeleton.h"
#include "graphics/texture.h"
#include "graphics/texture_factory.h"
#include "graphics/vertex_data.h"
#include "log/log.h"
#include "platform/resource_loader.h"

namespace
{

/**
 * Helper function to convert an assimp Matrix to an engine one.
 *
 * @param matrix
 *   Assimp matrix.
 *
 * @returns
 *   Engine matrix.
 */
iris::Matrix4 convert_matrix(const ::aiMatrix4x4 &matrix)
{
    return iris::Matrix4{
        {{matrix.a1,
          matrix.a2,
          matrix.a3,
          matrix.a4,
          matrix.b1,
          matrix.b2,
          matrix.b3,
          matrix.b4,
          matrix.c1,
          matrix.c2,
          matrix.c3,
          matrix.c4,
          matrix.d1,
          matrix.d2,
          matrix.d3,
          matrix.d4}}};
}

/**
 * Get the animations from an assimp scene.
 *
 * @param scene
 *   Scene to get animations from.
 *
 * @returns
 *   Animations.
 */
std::vector<iris::Animation> process_animations(const ::aiScene *scene)
{
    std::vector<iris::Animation> animations{};

    // parse each animation
    for (auto i = 0u; i < scene->mNumAnimations; ++i)
    {
        const auto *animation = scene->mAnimations[i];

        auto ticks_per_second = animation->mTicksPerSecond;
        if (ticks_per_second == 0.0f)
        {
            LOG_WARN("ms", "no ticks per second - guessing");
            ticks_per_second = 33.0f;
        }

        const std::chrono::milliseconds tick_time{
            static_cast<std::uint32_t>((1.0f / ticks_per_second) * 1000u)};
        const auto duration =
            tick_time * static_cast<std::uint32_t>(animation->mDuration);

        std::map<std::string, std::vector<iris::KeyFrame>> nodes;

        // each channel is a collection of keys
        for (auto j = 0u; j < animation->mNumChannels; ++j)
        {
            const auto *channel = animation->mChannels[j];

            // sanity check we have an equal number of keys for position,
            // rotation and scale
            if ((channel->mNumPositionKeys != channel->mNumRotationKeys) &&
                (channel->mNumRotationKeys != channel->mNumScalingKeys))
            {
                throw iris::Exception("incomplete frame data");
            }

            std::vector<iris::KeyFrame> keyframes{};

            // convert assimp keys into keyframes
            for (auto k = 0u; k < channel->mNumPositionKeys; ++k)
            {
                const auto assimp_pos = channel->mPositionKeys[k].mValue;
                const auto assimp_rot = channel->mRotationKeys[k].mValue;
                const auto assimp_scale = channel->mScalingKeys[k].mValue;
                const auto time =
                    static_cast<std::uint32_t>(channel->mPositionKeys[k].mTime);

                keyframes.emplace_back(
                    iris::Transform{
                        {assimp_pos.x, assimp_pos.y, assimp_pos.z},
                        {assimp_rot.x,
                         assimp_rot.y,
                         assimp_rot.z,
                         assimp_rot.w},
                        {assimp_scale.x, assimp_scale.y, assimp_scale.z}},
                    time * tick_time);
            }

            nodes[channel->mNodeName.C_Str()] = keyframes;
        }

        animations.emplace_back(duration, animation->mName.C_Str(), nodes);
    }

    return animations;
}

/**
 * Get the bones from an assimp mesh.
 *
 * Assimp has two separate concepts for animation data:
 *   bone - offset matrix and collection of vertices it effects
 *   node - hierarchical object containing transformation
 *
 * An assimp node may refer to a bone, or it may just represent an intermediate
 * transformation between bones. We unify both of these assimp concepts into an
 * engine Bone. Our Bone may or may not effect vertices and maintains the same
 * hierarchy as the assimp nodes.
 *
 * @param mesh
 *   Mesh to get bones from.
 *
 * @param root
 *   Root of hierarchy for bones.
 *
 * @returns
 *   Bones.
 */
std::vector<iris::Bone> process_bones(
    const ::aiMesh *mesh,
    const ::aiNode *root)
{
    std::vector<iris::Bone> bones{};

    std::stack<std::tuple<const ::aiNode *, std::string>> nodes;
    nodes.emplace(root, std::string{});

    // walk the node hierarchy
    do
    {
        auto [node, parent_name] = nodes.top();
        nodes.pop();

        const std::string name{node->mName.C_Str()};

        // create a bone which represents the nodes transformation but effects
        // no vertices
        iris::Bone bone{
            name, parent_name, {}, {}, convert_matrix(node->mTransformation)};

        // see if this node represents an assimp bone
        for (auto i = 0u; i < mesh->mNumBones; ++i)
        {
            const ::aiBone *ai_bone = mesh->mBones[i];

            if (std::string(ai_bone->mName.C_Str()) == name)
            {
                std::vector<iris::Weight> weights;
                for (auto j = 0u; j < ai_bone->mNumWeights; ++j)
                {
                    const auto &weight = ai_bone->mWeights[j];
                    weights.emplace_back(weight.mVertexId, weight.mWeight);
                }

                // replace bone with one that stores the weights as well the
                // correct matrices
                bone = {
                    name,
                    parent_name,
                    weights,
                    convert_matrix(ai_bone->mOffsetMatrix),
                    convert_matrix(node->mTransformation)};

                break;
            }
        }

        bones.emplace_back(bone);

        for (auto i = 0u; i < node->mNumChildren; ++i)
        {
            nodes.emplace(node->mChildren[i], name);
        }
    } while (!nodes.empty());

    // always return at least one default bone
    if (bones.empty())
    {
        bones.emplace_back(
            "root",
            "",
            std::vector<iris::Weight>{{0u, 1.0f}},
            iris::Matrix4{},
            iris::Matrix4{});
    }

    return bones;
}

/**
 * Get the texture for the given material.
 *
 * @param material
 *   Material to get texture for.
 *
 * @returns
 *   Texture.
 */
iris::Texture *process_texture(const ::aiMaterial *material)
{
    static const auto type = ::aiTextureType_DIFFUSE;

    auto *texture = iris::texture_factory::blank();

    // only support a single texture
    if (material->GetTextureCount(type) == 1u)
    {
        ::aiString str;
        material->GetTexture(type, 0u, &str);

        texture = iris::texture_factory::load(str.C_Str());
    }

    return texture;
}

/**
 * Get the indices for the given mesh.
 *
 * @param mesh
 *   Mesh to get vertices for.
 *
 * @returns
 *   Indices.
 */
std::vector<std::uint32_t> process_indices(const ::aiMesh *mesh)
{
    std::vector<std::uint32_t> indices{};

    for (auto i = 0u; i < mesh->mNumFaces; ++i)
    {
        const auto &face = mesh->mFaces[i];
        for (auto j = 0u; j < face.mNumIndices; ++j)
        {
            indices.emplace_back(face.mIndices[j]);
        }
    }

    return indices;
}

/**
 * Get the vertices for the given mesh.
 *
 * @param mesh
 *   Mesh to get vertices for.
 *
 * @param material
 *   Material for current mesh.
 *
 * @returns
 *   Vertex data.
 */
std::vector<iris::vertex_data> process_vertices(
    const ::aiMesh *mesh,
    const ::aiMaterial *material)
{
    std::vector<iris::vertex_data> vertices{};

    for (auto i = 0u; i < mesh->mNumVertices; ++i)
    {
        const auto &vertex = mesh->mVertices[i];
        const auto &normal = mesh->mNormals[i];
        iris::Vector3 colour{1.0f, 1.0f, 1.0f};
        iris::Vector3 texture_coords{};

        // get texture coordinates if they exist
        if (mesh->HasTextureCoords(0))
        {
            texture_coords.x = mesh->mTextureCoords[0][i].x;
            texture_coords.y = mesh->mTextureCoords[0][i].y;
        }

        // only support diffuse colour
        ::aiColor3D c(0.f, 0.f, 0.f);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, c);
        colour.x = c.r;
        colour.y = c.g;
        colour.z = c.b;

        vertices.emplace_back(
            iris::Vector3(vertex.x, vertex.y, vertex.z),
            iris::Vector3(normal.x, normal.y, normal.z),
            colour,
            texture_coords);
    }

    return vertices;
}
}

namespace iris
{

void load_mesh(
    const std::string &mesh_name,
    std::vector<std::vector<vertex_data>> *vertices,
    std::vector<std::vector<std::uint32_t>> *indices,
    std::vector<Texture *> *textures,
    Skeleton *skeleton)
{
    const auto file_data = ResourceLoader::instance().load(mesh_name);

    // parse file using assimp
    ::Assimp::Importer importer{};
    const auto *scene = importer.ReadFileFromMemory(
        file_data.data(), file_data.size(), ::aiProcess_Triangulate);

    if ((scene == nullptr) || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) ||
        (scene->mRootNode == nullptr))
    {
        throw Exception(
            std::string{"could not load mesh: "} + importer.GetErrorString());
    }

    const auto *root = scene->mRootNode;

    std::vector<Bone> bones;

    std::stack<const ::aiNode *> to_process;
    to_process.emplace(root);

    // walk the assimp scene and write the requested data
    do
    {
        const auto *node = to_process.top();
        to_process.pop();

        // iterate each mesh in the current node
        for (auto i = 0u; i < node->mNumMeshes; ++i)
        {
            const auto *mesh = scene->mMeshes[node->mMeshes[i]];
            const auto *material = scene->mMaterials[mesh->mMaterialIndex];

            // process and write requested data

            if (vertices != nullptr)
            {
                vertices->emplace_back(process_vertices(mesh, material));
            }

            if (indices != nullptr)
            {
                indices->emplace_back(process_indices(mesh));
            }

            if (textures != nullptr)
            {
                textures->emplace_back(process_texture(material));
            }

            if (skeleton != nullptr)
            {
                bones = process_bones(mesh, root);
            }
        }

        // add child nodes
        for (auto i = 0u; i < node->mNumChildren; ++i)
        {
            to_process.emplace(node->mChildren[i]);
        }
    } while (!to_process.empty());

    // if we want a skeleton then build it
    if (skeleton != nullptr)
    {
        const auto animations = process_animations(scene);

        *skeleton = {bones, animations};

        // if vertices were requested then go through them all and stamp in bone
        // data
        if (vertices != nullptr)
        {
            // each vertex supports four bones, so keep a track of the next
            // index for each vertex
            std::vector<std::uint32_t> bone_indices(vertices->back().size());

            for (const auto &bone : bones)
            {
                for (const auto &[id, weight] : bone.weights())
                {
                    if (weight == 0.0f)
                    {
                        continue;
                    }

                    // only support four bones per vertex
                    if (bone_indices[id] >= 4)
                    {
                        LOG_ENGINE_WARN(
                            "mf", "too many weights {} {}", id, weight);
                        continue;
                    }

                    const auto bone_index = skeleton->bone_index(bone.name());

                    // update vertex data with bone data
                    vertices->back()[id].bone_ids[bone_indices[id]] =
                        bone_index;
                    vertices->back()[id].bone_weights[bone_indices[id]] =
                        weight;

                    ++bone_indices[id];
                }
            }
        }
    }
}

}