#pragma once
#include "DXObjectBase.h"
#include "CParsingDataClass.h"
#include "Effects.h"

/// <summary>
/// Bounding Volume 만들기 위한 클래스
/// </summary>

class ResourceManager;
class HimeCamera;

class VolumeObject : public DXObjectBase
{
public:
	VolumeObject(ID3D11RasterizerState* pRS);
	VolumeObject(ID3D11RasterizerState* pRS, XMVECTOR pos);
	~VolumeObject();

public:
	void Initialize();

	void Update(HimeCamera* pCamera) override;
	void Render() override;

	void SetGeometryBuffer(ID3D11Buffer* VB, ID3D11Buffer* IB);
	void SetIndexSize(int val) { m_IndexSize = val; }

	DirectX::XMFLOAT4X4 GetWorld() const { return m_World; }
	virtual void SetPos(XMFLOAT4 Pos) override;
	virtual void SetRot(XMFLOAT4 Rot) override;

	// LocatTM를 pos, rot, scale로 분해하는 함수
	void DecomposeLocalTM();

	void SetTexResource(std::wstring name) override {};
	virtual void UpdateAnimation(int m_framecount) override {};
	virtual void CalcLocalTM() {};


private:
	ID3D11Device* m_pD3dDevice;						// D3D11 디바이스
	ID3D11DeviceContext* m_pDeviceContext;		// 디바이스 컨텍스트
	ID3D11RasterizerState* m_pRenderstate;

	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;

	XMFLOAT3 m_EyePosW;

	//리소스 관련 변수
	ResourceManager* m_pResourceManager;

	// 변환 관련
	//XMFLOAT4X4 m_World;	// 월드 변환 행렬 (로컬->월드)
	XMFLOAT4X4 m_View;	// 시야 변환 행렬 (카메라 뷰)
	XMFLOAT4X4 m_Proj;	// 투영 변환 행렬 (원근/직교)

	int m_IndexSize;

private:
	UINT m_IndexCount;
};

