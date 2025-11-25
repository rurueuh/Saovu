#pragma once
#include <DirectXMath.h>

/**
 * @class Camera
 * @brief Represents a camera in the 3D scene.
 * This class manages the view and projection matrices.
 */
class Camera
{
public:
	/**
	 * @brief Sets the perspective projection matrix.
	 * @param fovRadians The field of view in radians.
	 * @param aspect The aspect ratio.
	 * @param zNear The near clipping plane.
	 * @param zFar The far clipping plane.
	 */
	void SetPerspective(float fovRadians, float aspect, float zNear, float zFar)
	{
		using namespace DirectX;
		m_proj = XMMatrixPerspectiveFovLH(fovRadians, aspect, zNear, zFar);
	}

	/**
	 * @brief Sets the view matrix.
	 * @param eye The position of the camera.
	 * @param at The point the camera is looking at.
	 * @param up The up vector.
	 */
	void LookAt(DirectX::XMVECTOR eye, DirectX::XMVECTOR at, DirectX::XMVECTOR up)
	{
		using namespace DirectX;
		m_view = XMMatrixLookAtLH(eye, at, up);
	}

	/**
	 * @brief Gets the view matrix.
	 * @return The view matrix.
	 */
	DirectX::XMMATRIX View() const { return m_view; }

	/**
	 * @brief Gets the projection matrix.
	 * @return The projection matrix.
	 */
	DirectX::XMMATRIX Proj() const { return m_proj; }

	/**
	 * @brief Gets the position of the camera.
	 * @return The position of the camera.
	 */
	DirectX::XMFLOAT3 getPosition() const
	{
		using namespace DirectX;
		XMVECTOR det;
		XMMATRIX invView = XMMatrixInverse(&det, m_view);
		XMVECTOR pos = invView.r[3];
		XMFLOAT3 position;
		XMStoreFloat3(&position, pos);
		return position;
	}


private:
	DirectX::XMMATRIX m_view = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX m_proj = DirectX::XMMatrixIdentity();
};