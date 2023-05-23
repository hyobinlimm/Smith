#pragma once

#include "HimeRendererStructs.h"



class HimeDevice;
class HimeCamera;
class HimeFont;
class DXObjectBase;
class DXTKUIObject;
class FireEffect;

/// <summary>
/// ���� �׸��⸸ �ϴ� Ŭ����
/// 
/// 2022.07.15 HyobinLogic
/// </summary>
class HimeRenderer
{
public:
	HimeRenderer();
	~HimeRenderer();

public:
	// GetSet �Լ�
	void SetClientWidth(int val) { m_ClientWidth = val; }
	void SetClientHeight(int val) { m_ClientHeight = val; }

	std::vector<DXObjectBase*> GetRenderingQueue() const { return m_RenderingQueue; }
	void SetRenderingQueue(std::vector<DXObjectBase*> val) { m_RenderingQueue = val; }

public:
	void Initialize();

	void Update(HimeCamera* pCamera, float deltaTime); // test ��
	void Update(HimeCamera* pCamera, float deltaTime, MeshBuildInfo* pMeshInfo);

	// ���翵�� �߰��� �ؽ�Ʈ �����ͱ��� �Ѱܹ޴� ������Ʈ
	void TextUpdate(TextData* pTextData);
	void TextureUpdate(UIData* pTextureData);

	// �� �׷ȴٸ� ��Ҵ� ���۸� ����ش�. 
	void TextBufferClear();
	void TextureBufferClaer();
	
	void UpdateBoneWorldTM(std::vector<DXObjectBase*> objects);

	void BeginRender();
	void Render();
	void EndRender();

	void SetFireEffect(bool checkDraw);

public:
	// �ܺο� ���� �� �������̽�
	void AddDXObjectToRenderQueue(DXObjectBase* dxObject);

	// �Է¹��� ���ڸ� textdata ����ü�� ��� �Լ� 
	void CreateStirngData(int x, int y, float size, DirectX::XMFLOAT4 color, const char* text);


	void CreateStirngData(TextData* pTextData);
	
	// �Է¹��� TextureData ����ü�� ��� �Լ� 
	void CreateTextureData(UIData* textureData);

	
private:
	void RenderText(); // vector�� ��� text�� ����ϴ� �Լ�. 

private:
	int m_ClientWidth;
	int m_ClientHeight;

	bool m_isFireDraw;

private:
	// for DX11
	HimeDevice* m_pDirect3D;
	
	HimeFont* m_pFont;

	DXTKUIObject* m_pTexture2D;

	// �ִϸ��̼� �������� ������ ���� ����
	UINT m_LastFrame;

	// �׸� �� �ʿ��� ��ü�� �׸� ��ü
	/// ������ ������ ���ؼ� �������� �� ����� DX������Ʈ��
	std::vector<DXObjectBase*> m_RenderingQueue;

	/// text������ ��Ƶ� ��.
	std::vector<TextData*> m_RenderText;

	/// UI ������ ��Ƶ� ��
	std::vector<UIData*> m_RenderUI;

	///TODE:TEST - �ӽ� ��
	FireEffect* m_Fire;
	HimeCamera* m_Cam;
};


