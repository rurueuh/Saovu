#pragma once
#include <windows.h>
#include <cassert>
#include <cstdint>
#include <DirectXMath.h>
#include <algorithm>
#include <math.h>
#include <cstdlib>
#include <iostream>


inline constexpr uint32_t kSwapBufferCount = 2;

/**
 * @brief Throws an exception if the HRESULT is a failure.
 * In debug builds, this will trigger an assertion. In release builds, it will exit the process.
 * @param hr The HRESULT to check.
 */
static inline void DXThrow(HRESULT hr)
{
	if (FAILED(hr)) {
		assert(false);
		::ExitProcess(static_cast<UINT>(hr));
	}
}

/**
 * @brief Aligns a size to 256 bytes.
 * @param size The size to align.
 * @return The aligned size.
 */
inline UINT Align256(UINT size)
{
	return (size + 255u) & ~255u;
}

using namespace DirectX;
/**
 * @brief Converts a quaternion to Euler angles in radians.
 * @param q The quaternion to convert.
 * @return An XMFLOAT3 containing the roll, pitch, and yaw in radians.
 */
inline XMFLOAT3 XMQuaternionToEulerRad(FXMVECTOR q)
{
    XMFLOAT4 v; XMStoreFloat4(&v, q);
    const float ww = v.w, xx = v.x, yy = v.y, zz = v.z;

    float sinr_cosp = 2.f * (ww * xx + yy * zz);
    float cosr_cosp = 1.f - 2.f * (xx * xx + yy * yy);
    float roll = std::atan2(sinr_cosp, cosr_cosp);

    float sinp = 2.f * (ww * yy - zz * xx);
    sinp = std::clamp(sinp, -1.f, 1.f);
    float pitch = std::asin(sinp);

    float siny_cosp = 2.f * (ww * zz + xx * yy);
    float cosy_cosp = 1.f - 2.f * (yy * yy + zz * zz);
    float yaw = std::atan2(siny_cosp, cosy_cosp);

    return XMFLOAT3(roll, pitch, yaw);
}