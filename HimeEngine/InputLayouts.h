#pragma once

#include "DirectXDefine.h"
#include "d3dx11Effect.h"

namespace VertexStruct
{
	struct PosColor
	{
		PosColor()
		{
			// �ڱ� �ҽ��� ��� 0���� �ʱ�ȭ �Ѵ�. 
			ZeroMemory(this, sizeof(PosColor));
		}

		PosColor(XMFLOAT3 pos, XMFLOAT4 color)
		{
			ZeroMemory(this, sizeof(PosColor));
			//Pos = pos;
			//Color = color;
		}

		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	struct PosNormal
	{
		PosNormal()
		{
			// �ڱ� �ҽ��� ��� 0���� �ʱ�ȭ �Ѵ�. 
			ZeroMemory(this, sizeof(PosNormal));
		}

		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
	};

	struct PosNorTex
	{
		PosNorTex()
		{
			// �ڱ� �ҽ��� ��� 0���� �ʱ�ȭ �Ѵ�. 
			ZeroMemory(this, sizeof(PosNorTex));
		}

		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};

	struct PosNorTexSkin
	{
		PosNorTexSkin()
		{
			// �ڱ� �ҽ��� ��� 0���� �ʱ�ȭ �Ѵ�. 
			ZeroMemory(this, sizeof(PosNorTexSkin));
		}

		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;

		// ==================== Skinnig �߰� �ڵ� ====================
		XMFLOAT4 Weights1;
		XMFLOAT4 Weights2;
		XMFLOAT4 Weights3;
		UINT BoneIndices[9];
	};

	struct Particle
	{
		Particle()
		{
			// �ڱ� �ҽ��� ��� 0���� �ʱ�ȭ �Ѵ�. 
			ZeroMemory(this, sizeof(Particle));
		}

		XMFLOAT3 InitialPos;
		XMFLOAT3 InitialVel;
		XMFLOAT2 Size;
		float Age;
		unsigned int Type;
	};

	/// normalmap ������ / �����ϱ�� ��. 
	struct NormalMap
	{
		NormalMap()
		{
			// �ڱ� �ҽ��� ��� 0���� �ʱ�ȭ �Ѵ�. 
			ZeroMemory(this, sizeof(NormalMap));
		}

		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
		XMFLOAT3 TangentU;
	};
}

class InputLayoutDesc
{
public:
	static const D3D11_INPUT_ELEMENT_DESC PosColor[2];

	static const D3D11_INPUT_ELEMENT_DESC PosNormal[2];

	static const D3D11_INPUT_ELEMENT_DESC PosNorTex[3];

	static const D3D11_INPUT_ELEMENT_DESC PosNorTexSkin[9];

	static const D3D11_INPUT_ELEMENT_DESC Particle[5];
};

class InputLayouts
{
public:
	static void InitAll();
	static void DestroyAll();

	static ID3D11InputLayout* PosColor;
	static ID3D11InputLayout* PosNormal;
	static ID3D11InputLayout* PosNorTex;
	static ID3D11InputLayout* PosNorTexSkin;
	static ID3D11InputLayout* Particle;
};
