#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>
#include <string>
#include "MeshAsset.h"

/**
 * @struct Material
 * @brief Represents a material.
 */
struct Material {
    DirectX::XMFLOAT3 Kd{ 1,1,1 };
    DirectX::XMFLOAT3 Ks{ 1,1,1 };
    DirectX::XMFLOAT3 Ke{ 0,0,0 };
    std::string map_Kd;
    float Ns{ 128.f };
    float d{ 1.f };
    std::string map_normal;
    std::string map_metalRough;
};

/**
 * @class ResourceCache
 * @brief Manages the caching of resources.
 * This class is a singleton that provides access to cached resources like meshes and textures.
 */
class ResourceCache {
public:
    /**
     * @brief Gets the singleton instance of the ResourceCache.
     * @return A reference to the singleton instance.
     */
    static ResourceCache& I() { static ResourceCache s; return s; }

    /**
     * @brief Gets a mesh from an OBJ file.
     * This method will first check the cache for the mesh. If it is not found, it will load it from the file.
     * @param path The path to the OBJ file.
     * @return A shared pointer to the mesh asset.
     */
    std::shared_ptr<MeshAsset> getMeshFromOBJ(const std::string& path);

    /**
     * @brief Sets the default white texture.
     * @param t A shared pointer to the new default white texture.
     */
    void setDefaultWhiteTexture(std::shared_ptr<Texture> t) {
        std::lock_guard<std::mutex> lk(mu_);
        defaultWhite_ = std::move(t);
    }

    /**
     * @brief Gets the default white texture.
     * @return A shared pointer to the default white texture.
     */
    std::shared_ptr<Texture> defaultWhite() {
        std::lock_guard<std::mutex> lk(mu_);
        return defaultWhite_;
    }

private:
    ResourceCache() = default;
    std::mutex mu_;
    std::unordered_map<std::string, std::weak_ptr<MeshAsset>> meshCache_;
    std::shared_ptr<Texture> defaultWhite_;
};
