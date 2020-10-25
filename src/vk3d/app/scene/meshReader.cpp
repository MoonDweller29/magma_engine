#include "vk3d/app/scene/meshReader.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <iostream>
#include <string>
#include <vector>

static glm::vec3 aiVector3D_to_vec3(aiVector3D &orig)
{
    return glm::vec3(orig.x, orig.z, orig.y);
}

static glm::vec2 aiVector3D_to_vec2(aiVector3D &orig)
{
    return glm::vec2(orig.x, 1 - orig.y);
}

static void print_vector(std::vector<glm::vec3> vec)
{
    for (glm::vec3 &v : vec)
    {
        std::cout << v.x <<"_"<< v.y <<"_"<< v.z << std::endl;
    }
}

static Mesh convert_mesh(aiMesh *mesh)
{
    std::cout << mesh->mNumVertices << std::endl;
    std::cout << mesh->mNumFaces << std::endl;

//    std::vector<glm::vec3> vertices;
//    std::vector<glm::vec3> normals;
//    std::vector<glm::vec2> texCoords;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(mesh->mNumVertices);
//    normals.reserve(mesh->mNumVertices);
//    texCoords.reserve(mesh->mNumVertices);

    for (int i = 0; i < mesh->mNumVertices; ++i)
    {
//        vertices.push_back(
//                aiVector3D_to_vec3(mesh->mVertices[i])
//        );
//        normals.push_back(
//                aiVector3D_to_vec3(mesh->mNormals[i])
//        );
//        texCoords.push_back(
//                aiVector3D_to_vec2(mesh->mTextureCoords[0][i])
//        );
        Vertex vert{
                aiVector3D_to_vec3(mesh->mVertices[i]),
                aiVector3D_to_vec3(mesh->mNormals[i]),
                aiVector3D_to_vec2(mesh->mTextureCoords[0][i])
        };
        vertices.push_back(vert);
    }



    indices.reserve(mesh->mNumFaces*3);
    for (int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace *currFace = &(mesh->mFaces[i]);
        for (int j = 0; j < currFace->mNumIndices; ++j)
        {
            indices.push_back(currFace->mIndices[j]);
        }
    }

    Mesh res_mesh(
            mesh->mName.C_Str(),
            vertices,
            indices
    );

    // std::cout<< res_mesh.vertices.size() << "__"
    // 		<< res_mesh.normals.size() << "__"
    // 		<< res_mesh.indices.size() << std::endl;

    // print_vector(res_mesh.vertices);

    return res_mesh;
}

std::vector<Mesh> &MeshReader::load_scene(const std::string& filename)
{
    // Create an instance of the Importer class
    Assimp::Importer importer;
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile( filename,
                                              aiProcess_CalcTangentSpace       |
                                              aiProcess_Triangulate            |
                                              aiProcess_JoinIdenticalVertices  |
                                              aiProcess_SortByPType);
    // If the import failed, report it
    if( !scene)
    {
        std::cout << "Can't load scene from " << filename << std::endl;
        return meshes;
    }
    meshes.clear();
    meshes.reserve(scene->mNumMeshes);

    std::cout << "mNumMeshes = " << scene->mNumMeshes << std::endl;
    for (int i = 0; i < scene->mNumMeshes; ++i)
    {
        std::cout << scene->mMeshes[i]->mName.C_Str() << std::endl;
        meshes.push_back(convert_mesh(scene->mMeshes[i]));
    }

    return meshes;
}
