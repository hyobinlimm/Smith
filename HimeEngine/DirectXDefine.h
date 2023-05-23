#pragma once

#include <vector>
#include <tchar.h>
#include <string>
#include "MathHelper.h"

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <directxmath.h>
#include "DDSTextureLoader.h"

#pragma comment(lib, "d3d11.lib")

using namespace DirectX; // DirectXMath.h 때문에 필요

//---------------------------------------------------------------------------------------
// Convenience macro for releasing COM objects.
//---------------------------------------------------------------------------------------
#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }


//---------------------------------------------------------------------------------------
// Convenience macro for deleting objects.
//---------------------------------------------------------------------------------------
#define SafeDelete(x) { delete x; x = 0; }

// LEHIDE
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)									\
	{									\
		HRESULT hr = (x);						\
		if(FAILED(hr))							\
		{								\
			LPWSTR output;                                    	\
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |		\
				FORMAT_MESSAGE_IGNORE_INSERTS 	 |		\
				FORMAT_MESSAGE_ALLOCATE_BUFFER,			\
				NULL,						\
				hr,						\
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	\
				(LPTSTR) &output,				\
				0,						\
				NULL);					        \
			MessageBox(NULL, output, L"Error", MB_OK);		\
		}								\
	}
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

// particle 시스템을 주기 위해 
// 복사해온 루나할배 코드
class d3dHelper
{
public:
	static ID3D11ShaderResourceView* CreateTexture2DArraySRV(
		ID3D11Device* device, ID3D11DeviceContext* context,
		std::vector<std::wstring>& filenames);
	static ID3D11ShaderResourceView* CreateRandomTexture1DSRV(ID3D11Device* device);
};

