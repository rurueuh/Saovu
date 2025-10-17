#pragma once
#include <windows.h>
#include <cassert>
#include <cstdint>


inline void DXThrow(HRESULT hr)
{
	if (FAILED(hr)) { assert(false); ::ExitProcess(static_cast<UINT>(hr)); }
}


inline UINT Align256(UINT size)
{
	return (size + 255u) & ~255u;
}