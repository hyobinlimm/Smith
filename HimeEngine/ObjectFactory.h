#pragma once

#include "DirectXDefine.h"
#include "d3dx11Effect.h"
#include "AlignedAllocationPolicy.h"
#include "string"

/// <summary>
/// 모든 오브젝트를 생성지시서를 보내는 공장.
/// 
/// 22.07.07 MK1 간단한 라인정도의 오브젝트만이라도 생성되게 하자. 
/// </summary>

class ResourceManager;

class ObjectFactory : public AlignedAllocationPolicy<16>
{
public:
	ObjectFactory(ID3D11RasterizerState* pRS);
	~ObjectFactory();

public:
	void Initialize(std::string name);
	void Update(const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& proj);
	void Render();

private:
	void BuildGeometryBuffers(std::string name);

private:
	ID3D11Device* m_D3dDevice;						// D3D11 디바이스
	ID3D11DeviceContext* m_D3dImmediateContext;	// 디바이스 컨텍스트

	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;

	//리소스 관련 변수
	ResourceManager* m_pResourceManager;

	// 변환 관련
	XMMATRIX m_World;	// 월드 변환 행렬 (로컬->월드)
	XMMATRIX m_View;		// 시야 변환 행렬 (카메라 뷰)
	XMMATRIX m_Proj;		// 투영 변환 행렬 (원근/직교)

	int m_IndexSize;

	// 와이어로 그리기 위한 RenderState. 쉐이더에서 해도 된다.
	// 예제에서 On/Off로 바꾸는 등의 일을 하는 경우 이것을 바꿔줘도 될 것 같다.
	// 쉐이더에서 하는 경우도 스위칭 비용도 줄일 수 있는 방법은 많다.
	ID3D11RasterizerState* m_pRenderstate;
};

