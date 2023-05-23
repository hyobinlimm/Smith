#pragma once

#include "DirectXDefine.h"
#include "d3dx11Effect.h"
#include "AlignedAllocationPolicy.h"
#include "string"

/// <summary>
/// ��� ������Ʈ�� �������ü��� ������ ����.
/// 
/// 22.07.07 MK1 ������ ���������� ������Ʈ���̶� �����ǰ� ����. 
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
	ID3D11Device* m_D3dDevice;						// D3D11 ����̽�
	ID3D11DeviceContext* m_D3dImmediateContext;	// ����̽� ���ؽ�Ʈ

	ID3D11Buffer* m_pVB;
	ID3D11Buffer* m_pIB;

	//���ҽ� ���� ����
	ResourceManager* m_pResourceManager;

	// ��ȯ ����
	XMMATRIX m_World;	// ���� ��ȯ ��� (����->����)
	XMMATRIX m_View;		// �þ� ��ȯ ��� (ī�޶� ��)
	XMMATRIX m_Proj;		// ���� ��ȯ ��� (����/����)

	int m_IndexSize;

	// ���̾�� �׸��� ���� RenderState. ���̴����� �ص� �ȴ�.
	// �������� On/Off�� �ٲٴ� ���� ���� �ϴ� ��� �̰��� �ٲ��൵ �� �� ����.
	// ���̴����� �ϴ� ��쵵 ����Ī ��뵵 ���� �� �ִ� ����� ����.
	ID3D11RasterizerState* m_pRenderstate;
};

