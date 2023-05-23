#pragma once

#include "HimeRendererStructs.h"
#include "DXObjectFactory.h"

/// <summary>
/// device�� render�� �ʱ�ȭ ���ִ� Ŭ���� 
/// game�� �����ϰ� ������ ��ü? 
/// </summary>

class HimeDevice;
class HimeRenderer;

class HimeCamera;

class ResourceManager;
class ObjectManager;

class HimeEngine
{
public:
	HimeEngine();
	~HimeEngine();

public:
	void Initialize(HWND hWnd, int screenWidth, int screenHeight); // test ��
	void Initialize(HWND hWnd, int screenWidth, int screenHeight, CameraInfo* pCameraInfo);
	
	void OnResize(int screenWidth, int screenHeight);

	// ȭ�� ũ������ �ɶ� �ʿ��ؼ� ���翵�� ����.
	void OnResize();
	void SetScreenSize(int screenWidth, int screenHeight);
	
	void Update(float deltaTime); //test ��
	void Update(float deltaTime, MeshBuildInfo* pMeshInfo);
	
	void TextBufferClear();
	void UIBufferClaer();

	void Render();

	void Destroy();

	// �ӽ÷� ���� ī�޶� ���� ������Ʈ ����
	void CameraUpdate(CameraInfo* pCameraInfo);

private:
	float GetAspectRatio(int screenWidth, int screenHeight) const; // ��Ⱦ��

	/// �� �׷��Ƚ� ������ ����ϴ� ������ ���� �� �������̽�
	/// ���丮�� �̿��ؼ�, DX������Ʈ�� �ϳ��� �����, �װ��� ������ ť�� �����ؼ� �׸� �� �ֵ��� �Ѵ�.

public:
	// buildInfo ����ü 
	void SetBuildInfoAll(std::vector<FilePath*> pFilePath);

	// text�� �Է¹޾� �����ϴ� �Լ�? 
	void CreateStringBuffer(int x, int y, float size, DirectX::XMFLOAT4 color, const char* text);

	// test ���翵 test
	void CreateStringBuffer(TextData* pTextData);

	// UI Texture ���� ��θ� ������ ���� �Լ� 
	void SetUITextureData(std::vector<UIData*> textureData);

	void SetAniStateType(std::wstring name, int aniStateType);

	// �ϵ��ڵ����� ����� ������ ����
	/// TODO:REMOVE - CreateDXObject �ȿ��� � mesh Ÿ�������� ���� ������ ���ش�.   
	void CreateDXObject_Axis();
	void CreateDXObject_Grid();

private:
	// �׸��� �ʿ��� �͵�
	HimeDevice* m_pDirect;
	HimeRenderer* m_pRenderer;
	
	HimeCamera* m_pCamera;

	// ����� ���ؼ� �ʿ�
	ResourceManager* m_pResourceManager;	// ���ҽ� �Ŵ����� ���丮 ������ ���� �Ѵ�.
	DXObjectFactory* m_pFactory;

	// �����ϱ� (���� ��) ���� �͵�
	ObjectManager* m_pObjManager;	// ���� ������ �ȵȴ�.

	int m_ScreenWidth;
	int m_ScreenHeight;

public:
	// Asset�� �ε��ϴ� �Լ�. (���� ������ ����ؼ�)
	// �ʱ�ȭ �ҋ� �� ��� �ְ� �ϱ� ���ؼ�.
	void LoadAsset_ByBuildInfo(std::wstring IDName);

	// node�� �̸�, ������ �ְ� �޴� �Լ�. 
	std::vector<MeshBuildInfo*> GetMeshInfoList() const { return m_pFactory->m_MeshInfoList; }

	// ���� �������� ���� ���� Get�Լ��� ������. 
	int FindDXObjectUniqueIndex(std::wstring Name);

	///TODO: draw�� false�� �ٲ��ִ� �κ��� ����. ū��. 
	void SetRenderable(int UniqueIndex);
	
	void SetFireEffect(bool checkDraw);


	/// ���� �� �׽�Ʈ�� �ӽ� �Լ�
	
	// ASE���� �̸� �ϳ������� DXObject���� ������ش�.
public:
	// Asset�� �ε��ϴ� �Լ�. // �ʱ�ȭ �ҋ� �� ��� �ִ� ��. 
	void LoadDXObject_FromASEFile(std::wstring fileName);


	/// ���콺 �Է� ����
private:
	HWND mhMainWnd;				/// ���� �ڵ�
	POINT mLastMousePos;

public:
	void OnMouseDown(int x, int y);
	void OnMouseUp(int x, int y);
	void OnMouseMove(int btnState, int x, int y);
};

