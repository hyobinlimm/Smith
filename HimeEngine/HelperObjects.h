#pragma once

#include "DXObjectBase.h"
#include "HimeDevice.h"


/// <summary>
/// ���� �׸��� ������Ʈ
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

	// �� ������Ʈ�� ������� �ʴ´�. 
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
	/// TODO: -�ֳ׵��� comPtr�� ���θ�, ���� ����� ���� �ʿ䰡 ����. ���ĺ����� ����. 
	ID3D11Device* m_pD3dDevice;						// D3D11 ����̽�
	ID3D11DeviceContext* m_pD3dImmediateContext;		// ����̽� ���ؽ�Ʈ

	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;

	//���ҽ� ���� ����
	ResourceManager* m_pResourceManager;

	// ��ȯ ����
	//XMFLOAT4X4 m_World;	// ���� ��ȯ ��� (����->����)
	XMFLOAT4X4 m_View;		// �þ� ��ȯ ��� (ī�޶� ��)
	XMFLOAT4X4 m_Proj;		// ���� ��ȯ ��� (����/����)

	int m_IndexSize;

	// ���̾�� �׸��� ���� RenderState. ���̴����� �ص� �ȴ�.
	// �������� On/Off�� �ٲٴ� ���� ���� �ϴ� ��� �̰��� �ٲ��൵ �� �� ����.
	// ���̴����� �ϴ� ��쵵 ����Ī ��뵵 ���� �� �ִ� ����� ����.
	ID3D11RasterizerState* m_pRenderstate;

};

