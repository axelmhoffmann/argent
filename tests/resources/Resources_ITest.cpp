#include "../TestFramework.h"
#include "resources/AssetManager.h"
#include "resources/Mesh.h"
#include "resources/Texture.h"
#include "resources/Material.h"

const std::string meshPath = "assets/cube.obj";
const std::string texPath = "assets/frog.png";
const std::string matPath = "assets/test.mat";


$ITest(resources, {});

$Cleanup(resources)
{
    ag::AssetManager::DropAll();
}

$Case(mesh_loading, resources)
{
    uint32_t meshID = ag::AssetManager::Load<ag::Mesh>(meshPath);

    std::weak_ptr<ag::Mesh> wMesh = ag::AssetManager::Fetch<ag::Mesh>(meshID);

    ag_expect(!wMesh.expired(), "Expected pointer to Mesh resource to be valid");

    std::shared_ptr<ag::Mesh> mesh = wMesh.lock();

    ag_expect(mesh->IsReady(), "Expected Mesh to be ready");

    ag_expect(mesh->vertices.size() == 36, "Expected Mesh to have 36 vertices, instead found {}", mesh->vertices.size());

    mesh->Unload();

    ag_expect(!mesh->IsReady(), "Expected Mesh to no longer be ready after unload");
}


$Case(texture_loading, resources)
{
    uint32_t texID = ag::AssetManager::Load<ag::Texture>(texPath);

    std::weak_ptr<ag::Texture> wTex = ag::AssetManager::Fetch<ag::Texture>(texID);

    ag_expect(!wTex.expired(), "Expected pointer to Texture resource to be valid");

    std::shared_ptr<ag::Texture> tex = wTex.lock();

    ag_expect(tex->IsReady(), "Expected Texture to be ready");

    ag_expect(tex->width == 10, "Expected Texture to be 10 pixels wide, instead found {}", tex->width);

    tex->Unload();

    ag_expect(!tex->IsReady(), "Expected Texture to no longer be ready after unload");
}

$Case(material_loading, resources)
{
    uint32_t matID = ag::AssetManager::Load<ag::Material>(matPath);

    std::weak_ptr<ag::Material> wMat = ag::AssetManager::Fetch<ag::Material>(matID);

    ag_expect(!wMat.expired(), "Expected pointer to Material resource to be valid");

    std::shared_ptr<ag::Material> mat = wMat.lock();

    ag_expect(mat->IsReady(), "Expected Material to be ready");

    ag_expect(mat->parameters[0] == 0.4f, "Expected Material parameter[0] to be 0.4, instead found {}", mat->parameters[0]);

    mat->Unload();
    
    ag_expect(!mat->IsReady(), "Expected Material to no longer be ready after unload");
}