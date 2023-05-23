#pragma once

#include "IDXObjectBase.h"
#include "NodeInfo.h"
#include "HimeDevice.h"

/// <summary>
/// DX������Ʈ���� ���̽� 
/// 
/// �������̽��� ��� �ް�, 
/// �̰������� ��� ������ ��� �ְ� �Ѵ�. 
/// </summary>

class HimeCamera;

class DXObjectBase : public IDXObjectBase, public NodeInfo
{
public:
	DXObjectBase();
	virtual ~DXObjectBase();

	void Update(HimeCamera* pCamera) override;
	void Render() override;

	// object������ ����µ� �ʿ��� �Լ�
	virtual void DecomposeLocalTM() abstract;

	// �� ������ ���Ͽ� ���Ƿ� �߰�(���翵)
	virtual void SetPos(XMFLOAT4 Pos) abstract;
	virtual void SetRot(XMFLOAT4 Rot) abstract;
	
	// �ؽ�ó�� �����ϱ� ���� �Լ�
	virtual void SetTexResource(std::wstring name) abstract;

	DirectX::XMFLOAT4X4 GetWorld() const { return m_World; }

	// Animation�� ���������ϴ� �Լ�
	virtual void UpdateAnimation(int m_framecount) abstract;

	virtual void CalcLocalTM() abstract;

	// �⺻������ Action�� �ϱ� ���ؼ� �ʿ��� ��
	ID3D11DeviceContext* m_pDeviceContext;

	/// �ؽ��Ŀ��� �߰��� �κ�
	ID3D11ShaderResourceView* m_pDiffuseMapSRV;

	XMFLOAT4X4 m_World;	// ���� ��ȯ ��� (����->����)

	// �ִϸ��̼��� ���¸� �ٲ��ֱ� ���� ���� 
	void SetAniType(int aniType) { m_anitype = aniType; };
	int m_anitype;

	// �̵� ���
	XMVECTOR m_MovePos;
	XMVECTOR m_MoveRot;

	// �ִϸ��̼��� ���� ���
	XMVECTOR m_Position;
	XMVECTOR m_Rotate;
	XMVECTOR m_Scale;

	XMMATRIX m_MovMX;

	UINT m_LightCount;
};

