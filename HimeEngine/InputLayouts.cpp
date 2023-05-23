#include "InputLayouts.h"
#include "Effects.h"
#include "HimeDevice.h"

#pragma region InputLayoutDesc

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosColor[2] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosNormal[2] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosNorTex[3] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosNorTexSkin[9] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, 
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}, 
	{"WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	{"WEIGHTSS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	{"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT,0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	{"BONEINDICESS", 0, DXGI_FORMAT_R32G32B32A32_UINT,0, 96, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	{"BONEINDICESSS", 0, DXGI_FORMAT_R32G32B32A32_UINT,0, 112, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Particle[5] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
};
#pragma endregion

#pragma region InputLayouts

ID3D11InputLayout* InputLayouts::PosColor = 0;
ID3D11InputLayout* InputLayouts::PosNormal = 0;
ID3D11InputLayout* InputLayouts::PosNorTex = 0;
ID3D11InputLayout* InputLayouts::PosNorTexSkin = 0;
ID3D11InputLayout* InputLayouts::Particle = 0;

void InputLayouts::InitAll()
{
	ID3D11Device* _device = HimeDevice::GetInst()->GetD3dDevice();
	
	/// <summary>
	/// 구조체는 같지만 안에 어떤 내용이 들어가는지가 달라지는 것.
	/// 그렇기 때문에 구조체는 한개만 갖고 있으면 됨. 
	/// 내가 쉐이더로 넘길때 어떤 이펙트를 사용할건지 지정해 주기만 하면 됨. 
	/// </summary>
	D3DX11_PASS_DESC passDesc;

	// PosColor
	Effects::PosColorFX->Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(_device->CreateInputLayout(InputLayoutDesc::PosColor, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &PosColor));
	
	// PosNormal
	Effects::PosNormalFX->Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(_device->CreateInputLayout(InputLayoutDesc::PosNormal, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &PosNormal));

	// PosNorTex
	Effects::PosNorTexFX->Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(_device->CreateInputLayout(InputLayoutDesc::PosNorTex, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &PosNorTex));

	// PosNorTexSkin
	Effects::PosNorTexSkinFX->Light1TexSkinTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(_device->CreateInputLayout(InputLayoutDesc::PosNorTexSkin, 9, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &PosNorTexSkin));

	// Particle
	Effects::FireFX->StreamOutTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(_device->CreateInputLayout(InputLayoutDesc::Particle, 5, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &Particle));
}

void InputLayouts::DestroyAll()
{
	ReleaseCOM(PosColor);
	ReleaseCOM(PosNormal);
	ReleaseCOM(PosNorTex);
	ReleaseCOM(PosNorTexSkin);
	ReleaseCOM(Particle);
}

#pragma endregion