#pragma once

#include "IDXObjectBase.h"
#include "NodeInfo.h"
#include "HimeDevice.h"

/// <summary>
/// DX오브젝트들의 베이스 
/// 
/// 인터페이스를 상속 받고, 
/// 이곳에서는 멤버 변수를 들고 있게 한다. 
/// </summary>

class HimeCamera;

class DXObjectBase : public IDXObjectBase, public NodeInfo
{
public:
	DXObjectBase();
	virtual ~DXObjectBase();

	void Update(HimeCamera* pCamera) override;
	void Render() override;

	// object구조를 만드는데 필요한 함수
	virtual void DecomposeLocalTM() abstract;

	// 값 변경을 위하여 임의로 추가(최재영)
	virtual void SetPos(XMFLOAT4 Pos) abstract;
	virtual void SetRot(XMFLOAT4 Rot) abstract;
	
	// 텍스처를 세팅하기 위한 함수
	virtual void SetTexResource(std::wstring name) abstract;

	DirectX::XMFLOAT4X4 GetWorld() const { return m_World; }

	// Animation를 선형보관하는 함수
	virtual void UpdateAnimation(int m_framecount) abstract;

	virtual void CalcLocalTM() abstract;

	// 기본적으로 Action을 하기 위해서 필요한 것
	ID3D11DeviceContext* m_pDeviceContext;

	/// 텍스쳐에서 추가된 부분
	ID3D11ShaderResourceView* m_pDiffuseMapSRV;

	XMFLOAT4X4 m_World;	// 월드 변환 행렬 (로컬->월드)

	// 애니메이션의 상태를 바꿔주기 위한 변수 
	void SetAniType(int aniType) { m_anitype = aniType; };
	int m_anitype;

	// 이동 행렬
	XMVECTOR m_MovePos;
	XMVECTOR m_MoveRot;

	// 애니메이션을 위한 행렬
	XMVECTOR m_Position;
	XMVECTOR m_Rotate;
	XMVECTOR m_Scale;

	XMMATRIX m_MovMX;

	UINT m_LightCount;
};

