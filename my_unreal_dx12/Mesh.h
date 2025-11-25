#pragma once
#include <memory>
#include <DirectXMath.h>
#include "MeshAsset.h"
#include "ResourceCache.h"
#include "Utils.h"
#include <cmath>

constexpr auto M_PI = 3.14159265358979323846f;

/**
 * @class Mesh
 * @brief Represents a 3D mesh with its own transformation.
 * This class handles the creation, transformation, and rendering of a mesh.
 */
class Mesh {
public:
    /**
     * @brief Constructs a new Mesh object from vertices and indices.
     * @param vertices The vertices of the mesh.
     * @param indices The indices of the mesh.
     */
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    /**
     * @brief Constructs a new Mesh object from a file.
     * @param filename The path to the mesh file.
     */
    Mesh(const std::string& filename);

    Mesh(const Mesh&) = default;
    Mesh& operator=(const Mesh&) = default;
    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(Mesh&&) noexcept = default;

	/**
	 * @brief Creates a plane mesh.
	 * @param width The width of the plane.
	 * @param depth The depth of the plane.
	 * @param m The number of vertices along the width.
	 * @param n The number of vertices along the depth.
	 * @return A new Mesh object representing a plane.
	 */
	static Mesh CreatePlane(float width, float depth, uint32_t m = 2, uint32_t n = 2);

	/**
	 * @brief Creates a cube mesh.
	 * @param size The size of the cube.
	 * @return A new Mesh object representing a cube.
	 */
	static Mesh CreateCube(float size = 1);

	/**
	 * @brief Creates a sphere mesh.
	 * @param diameter The diameter of the sphere.
	 * @param sliceCount The number of slices.
	 * @param stackCount The number of stacks.
	 * @return A new Mesh object representing a sphere.
	 */
	static Mesh CreateSphere(float diameter = 1, uint16_t sliceCount = 16, uint16_t stackCount = 16);

    /**
     * @brief Creates a cylinder mesh.
     * @param radius The radius of the cylinder.
     * @param height The height of the cylinder.
     * @param slices The number of slices.
     * @param withCaps Whether to include caps on the cylinder.
     * @return A new Mesh object representing a cylinder.
     */
    static Mesh CreateCylinder(float radius = 1, float height = 1, uint32_t slices = 16, bool withCaps = true);

	/**
	 * @brief Creates a cone mesh.
	 * @param radius The radius of the cone.
	 * @param height The height of the cone.
	 * @param slices The number of slices.
	 * @param withBase Whether to include a base on the cone.
	 * @return A new Mesh object representing a cone.
	 */
	static Mesh CreateCone(    float radius = 1, float height = 1, uint32_t slices = 32, bool withBase = true);

    /**
     * @brief Sets the position of the mesh.
     * @param x The x-coordinate.
     * @param y The y-coordinate.
     * @param z The z-coordinate.
     */
    void SetPosition(float x, float y, float z);

	/**
	 * @brief Sets the x-coordinate of the mesh's position.
	 * @param x The new x-coordinate.
	 */
	void SetPositionX(float x);

	/**
	 * @brief Sets the y-coordinate of the mesh's position.
	 * @param y The new y-coordinate.
	 */
	void SetPositionY(float y);

	/**
	 * @brief Sets the z-coordinate of the mesh's position.
	 * @param z The new z-coordinate.
	 */
	void SetPositionZ(float z);

    /**
     * @brief Adds to the position of the mesh.
     * @param dx The change in the x-coordinate.
     * @param dy The change in the y-coordinate.
     * @param dz The change in the z-coordinate.
     */
    void AddPosition(float dx, float dy, float dz);

	/**
	 * @brief Adds to the x-coordinate of the mesh's position.
	 * @param dx The change in the x-coordinate.
	 */
	void AddPositionX(float dx);

	/**
	 * @brief Adds to the y-coordinate of the mesh's position.
	 * @param dy The change in the y-coordinate.
	 */
	void AddPositionY(float dy);

	/**
	 * @brief Adds to the z-coordinate of the mesh's position.
	 * @param dz The change in the z-coordinate.
	 */
	void AddPositionZ(float dz);

    /**
     * @brief Sets the rotation of the mesh using yaw, pitch, and roll.
     * @param yawDeg The yaw in degrees.
     * @param pitchDeg The pitch in degrees.
     * @param rollDeg The roll in degrees.
     */
    void SetRotationYawPitchRoll(float yawDeg, float pitchDeg, float rollDeg);

	/**
	 * @brief Sets the yaw of the mesh.
	 * @param yawDeg The yaw in degrees.
	 */
	void SetRotationYaw(float yawDeg);

	/**
	 * @brief Sets the pitch of the mesh.
	 * @param pitchDeg The pitch in degrees.
	 */
	void SetRotationPitch(float pitchDeg);

	/**
	 * @brief Sets the roll of the mesh.
	 * @param rollDeg The roll in degrees.
	 */
	void SetRotationRoll(float rollDeg);

    /**
     * @brief Adds to the rotation of the mesh using yaw, pitch, and roll.
     * @param dyawDeg The change in yaw in degrees.
     * @param dpitchDeg The change in pitch in degrees.
     * @param drollDeg The change in roll in degrees.
     */
    void AddRotationYawPitchRoll(float dyawDeg, float dpitchDeg, float drollDeg);

	/**
	 * @brief Adds to the yaw of the mesh.
	 * @param dyawDeg The change in yaw in degrees.
	 */
	void AddRotationYaw(float dyawDeg);

	/**
	 * @brief Adds to the pitch of the mesh.
	 * @param dpitchDeg The change in pitch in degrees.
	 */
	void AddRotationPitch(float dpitchDeg);

	/**
	 * @brief Adds to the roll of the mesh.
	 * @param drollDeg The change in roll in degrees.
	 */
	void AddRotationRoll(float drollDeg);

    /**
     * @brief Sets the scale of the mesh.
     * @param sx The scale in the x-direction.
     * @param sy The scale in the y-direction.
     * @param sz The scale in the z-direction.
     */
    void SetScale(float sx, float sy, float sz);

	/**
	 * @brief Sets the scale in the x-direction.
	 * @param sx The new scale in the x-direction.
	 */
	void SetScaleX(float sx);

	/**
	 * @brief Sets the scale in the y-direction.
	 * @param sy The new scale in the y-direction.
	 */
	void SetScaleY(float sy);

	/**
	 * @brief Sets the scale in the z-direction.
	 * @param sz The new scale in the z-direction.
	 */
	void SetScaleZ(float sz);

    /**
     * @brief Adds to the scale of the mesh.
     * @param dsx The change in scale in the x-direction.
     * @param dsy The change in scale in the y-direction.
     * @param dsz The change in scale in the z-direction.
     */
    void AddScale(float dsx, float dsy, float dsz);

	/**
	 * @brief Adds to the scale in the x-direction.
	 * @param dsx The change in scale in the x-direction.
	 */
	void AddScaleX(float dsx);

	/**
	 * @brief Adds to the scale in the y-direction.
	 * @param dsy The change in scale in the y-direction.
	 */
	void AddScaleY(float dsy);

	/**
	 * @brief Adds to the scale in the z-direction.
	 * @param dsz The change in scale in the z-direction.
	 */
	void AddScaleZ(float dsz);

    /**
     * @brief Gets the underlying mesh asset.
     * @return A pointer to the MeshAsset.
     */
    const MeshAsset* GetAsset() const { return m_asset.get(); }

    /**
     * @brief Gets the transformation matrix of the mesh.
     * @return The transformation matrix.
     */
    const DirectX::XMMATRIX& Transform() const { return m_transform; }

    /**
     * @brief Sets the texture of the mesh.
     * @param t A shared pointer to the new texture.
     */
    void SetTexture(std::shared_ptr<Texture> t) { if (m_asset) m_asset->texture = std::move(t); }

    /**
     * @brief Gets the texture of the mesh.
     * @return A pointer to the texture, or the default white texture if none is set.
     */
    Texture* GetTexture() const {
        if (!m_asset) return nullptr;
        auto t = m_asset->texture ? m_asset->texture : ResourceCache::I().defaultWhite();
        return t ? t.get() : nullptr;
    }

    /**
     * @brief Gets a shared pointer to the texture of the mesh.
     * @return A shared pointer to the texture, or the default white texture if none is set.
     */
    std::shared_ptr<Texture> GetTextureShared() const {
        if (!m_asset) return {};
        return m_asset->texture ? m_asset->texture : ResourceCache::I().defaultWhite();
    }

    /**
     * @brief Sets the color of the mesh.
     * @param r The red component.
     * @param g The green component.
     * @param b The blue component.
     */
    void SetColor(float r, float g, float b);

    /**
     * @brief Gets the color of the mesh.
     * @return A tuple containing the red, green, and blue components of the color.
     */
    std::tuple<float, float, float> getColor() const;

    /**
     * @brief Binds the texture of the mesh to the pipeline.
     * @param cmdList The command list.
     * @param rootParamIndex The root parameter index for the texture.
     */
    void BindTexture(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndex) const;

    /**
     * @brief Gets the vertex buffer view.
     * @return The D3D12_VERTEX_BUFFER_VIEW.
     */
    const D3D12_VERTEX_BUFFER_VIEW& VBV() const { return m_asset->vbv; }

    /**
     * @brief Gets the index buffer view.
     * @return The D3D12_INDEX_BUFFER_VIEW.
     */
    const D3D12_INDEX_BUFFER_VIEW& IBV() const { return m_asset->ibv; }

    /**
     * @brief Gets the number of indices in the mesh.
     * @return The index count.
     */
    UINT IndexCount() const { return m_asset->indexCount; }

	/**
	 * @brief Sets the shininess of the mesh.
	 * @param s The new shininess value.
	 */
	void setShininess(float s) {
        if (s < 16.f) {
            std::cout << "[Warning]: shininess too low, Object may appear too dull." << std::endl;
		}
		else if (s > 256.f) {
            std::cout << "[Warning]: shininess too high, Object may appear too shiny." << std::endl;
		}
        for (auto & sm : m_asset->submeshes) {
            sm.shininess = s;
		}
    }


private:
    void UpdateMatrix();
    std::shared_ptr<MeshAsset> m_asset;

    DirectX::XMVECTOR m_position{ DirectX::XMVectorZero() };
    DirectX::XMVECTOR m_scale{ DirectX::XMVectorSet(1,1,1,0) };
    DirectX::XMVECTOR m_rotQ{ DirectX::XMQuaternionIdentity() };

    DirectX::XMMATRIX m_transform{ DirectX::XMMatrixIdentity() };

    float m_yawDeg = 0.f;
    float m_pitchDeg = 0.f;
    float m_rollDeg = 0.f;

    void RecomputeRotationFromAbsoluteEuler();
};
