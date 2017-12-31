#include "entity.hpp"

#include <iostream>
#include <cstdint>
#include <experimental/filesystem>
#include <queue>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "matrix.hpp"
#include "vector3.hpp"

namespace
{

/**
 * Helper function to convert an assimp aiMesh object into an eng::mesh object.
 *
 * @param mesh
 *   Assimp mesh object.
 *
 * @param path
 *   Path to texture file for mesh.
 *
 * @param colour
 *   Colour of meshes.
 *
 * @param position
 *   Position of mesh in world space.
 *
 * @param scale
 *   Scale of entity.
 *
 * @returns
 *   An eng::mesh object representing the supplied asismp aiMesh.
 */
eng::mesh create_mesh(
    const aiMesh * const mesh,
    const std::experimental::filesystem::path &path,
    const std::uint32_t colour,
    const eng::vector3 &position,
    const eng::vector3 &scale)
{
    if(mesh == nullptr)
    {
        throw std::runtime_error("mesh pointer is null");
    }

    std::vector<eng::vertex_data> vertices{ };
    std::vector<std::uint32_t> indices{ };

    for(auto j = 0u; j < mesh->mNumVertices; ++j)
    {
        // parse each assimp vertex data
        const auto ai_vertex = mesh->mVertices[j];

        eng::vector3 position = {
            ai_vertex.x,
            ai_vertex.y,
            ai_vertex.z };

        eng::vector3 texture_coords{ };

        // if mesh has vertex coords then extract them
        if(mesh->mTextureCoords[0] != nullptr)
        {
            texture_coords.x = mesh->mTextureCoords[0][j].x;
            texture_coords.y = mesh->mTextureCoords[0][j].y;
        }

        vertices.emplace_back(position, texture_coords);
    }

    // parse each assimp index
    for(auto j = 0u; j < mesh->mNumFaces; ++j)
    {
        const auto face = mesh->mFaces[j];

        for(auto k = 0u; k < face.mNumIndices; ++k)
        {
            indices.emplace_back(face.mIndices[k]);
        }
    }

    return{ vertices, indices, eng::texture{ path }, colour, position, scale };
}

/**
 * Helper function to parse a 3d model file and create an internal engine
 * representation from it.
 *
 * Internally this method uses the assimp library so will load any format
 * it supports.
 * @param path
 *   Path to model file to load.
 *
 * @param colour
 *   Colour of meshes.
 *
 * @param position
 *   Position of mesh in world space.
 *
 * @param scale
 *   Scale of entity.
 *
 * @returns
 *   Collection of parsed meshes.
 */
std::vector<eng::mesh> load_file(
    const std::experimental::filesystem::path &path,
    const std::uint32_t colour,
    const eng::vector3 &position,
    const eng::vector3 &scale)
{
    if(!std::experimental::filesystem::exists(path))
    {
        throw std::runtime_error(path.string() + " does not exist");
    }

    ::Assimp::Importer importer{ };

    // parse the mesh using assimp and check it was successful
    const auto *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if((scene == nullptr) ||
       (scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) ||
       (scene->mRootNode == nullptr))
    {
        throw std::runtime_error("could not load file: " + std::string{ importer.GetErrorString() });
    }

    std::vector<eng::mesh> meshes{ };

    std::queue<const aiNode * const> queue;
    queue.push(scene->mRootNode);

    // iterate through the assimp mesh hierarchy and convert each mesh into
    // our own representation
    do
    {
        const auto *node = queue.front();
        queue.pop();

        // convert each mesh in the current node
        for(int i = 0u; i < node->mNumMeshes; ++i)
        {
            const auto *mesh = scene->mMeshes[node->mMeshes[i]];
            const auto *material = scene->mMaterials[mesh->mMaterialIndex];
            std::experimental::filesystem::path image_path{ };

            // get diffuse texture information
            for(int j = 0u; j < material->GetTextureCount(aiTextureType_DIFFUSE); ++j)
            {
                // get name of texture file
                ::aiString ai_name{ };
                material->GetTexture(aiTextureType_DIFFUSE, j, &ai_name);

                // we assume texture files are located in the same directory
                // as the model file, so construct a new path
                std::string filename{ ai_name.C_Str() };
                image_path = std::experimental::filesystem::path(path).replace_filename(filename);

                // only currently support loading a single texture so break here
                break;
            }

            meshes.emplace_back(create_mesh(
                mesh,
                image_path,
                colour,
                position,
                scale));
        }

        // queue up the child nodes for processing
        for(int i = 0u; i < node->mNumChildren; ++i)
        {
            queue.push(node->mChildren[i]);
        }
    } while(!queue.empty());

    return meshes;
}

}
namespace eng
{

entity::entity(
    const std::experimental::filesystem::path &path,
    const std::uint32_t colour,
    const vector3 &position,
    const vector3 &scale)
    : meshes_(load_file(path, colour, position, scale))
{ }

const std::vector<mesh>& entity::meshes() const noexcept
{
    return meshes_;
}

}
