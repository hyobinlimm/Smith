#pragma once

#include "DXObjectBase.h"
#include "HimeDevice.h"


/// <summary>
/// 선을 그리는 오브젝트
/// 
/// 2022.06.22
/// </summary>

class ResourceManager;
class HimeCamera;

class HelperObjects : public DXObjectBase
{
public:
	HelperObjects(ID3D11RasterizerState* pRS);
	~HelperObjects();

public:
	void Initialize();
	void Update(HimeCamera* pCamera) override;
	void Render() override;

	// 이 오브젝트는 사용하지 않는다. 
	void DecomposeLocalTM() {};

	void SetGeometryBuffer(ID3D11Buffer* VB, ID3D11Buffer* IB);
	void SetIndexSize(int val) { m_IndexSize = val; }
	void SetTexResource(std::wstring name) {};

	DirectX::XMFLOAT4X4 GetWorld() const { return m_World; }
	virtual void SetPos(XMFLOAT4 Pos) override {};
	virtual void SetRot(XMFLOAT4 Rot) override {};

	virtual void UpdateAnimation(int m_framecount) {};
	virtual void CalcLocalTM() {};


private:
	/// TODO: -애네들을 comPtr로 감싸면, 따로 릴리즈를 해줄 필요가 없다. 고쳐보도록 하자. 
	ID3D11Device* m_pD3dDevice;						// D3D11 디바이스
	ID3D11DeviceContext* m_pD3dImmediateContext;		// 디바이스 컨텍스트

	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;

	//리소스 관련 변수
	ResourceManager* m_pResourceManager;

	// 변환 관련
	//XMFLOAT4X4 m_World;	// 월드 변환 행렬 (로컬->월드)
	XMFLOAT4X4 m_View;		// 시야 변환 행렬 (카메라 뷰)
	XMFLOAT4X4 m_Proj;		// 투영 변환 행렬 (원근/직교)

	int m_IndexSize;

	// 와이어로 그리기 위한 RenderState. 쉐이더에서 해도 된다.
	// 예제에서 On/Off로 바꾸는 등의 일을 하는 경우 이것을 바꿔줘도 될 것 같다.
	// 쉐이더에서 하는 경우도 스위칭 비용도 줄일 수 있는 방법은 많다.
	ID3D11RasterizerState* m_pRenderstate;

};

