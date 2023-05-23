#pragma once

#include "HimeRendererStructs.h"
#include "DXObjectFactory.h"

/// <summary>
/// device와 render를 초기화 해주는 클래스 
/// game과 유일하게 소통할 객체? 
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
	void Initialize(HWND hWnd, int screenWidth, int screenHeight); // test 용
	void Initialize(HWND hWnd, int screenWidth, int screenHeight, CameraInfo* pCameraInfo);
	
	void OnResize(int screenWidth, int screenHeight);

	// 화면 크기조정 될때 필요해서 최재영이 변경.
	void OnResize();
	void SetScreenSize(int screenWidth, int screenHeight);
	
	void Update(float deltaTime); //test 용
	void Update(float deltaTime, MeshBuildInfo* pMeshInfo);
	
	void TextBufferClear();
	void UIBufferClaer();

	void Render();

	void Destroy();

	// 임시로 만든 카메라 정보 업데이트 공간
	void CameraUpdate(CameraInfo* pCameraInfo);

private:
	float GetAspectRatio(int screenWidth, int screenHeight) const; // 종횡비

	/// 이 그래픽스 엔진을 사용하는 측에서 쓰게 될 인터페이스
	/// 팩토리를 이용해서, DX오브젝트를 하나를 만들고, 그것을 렌더링 큐에 보관해서 그릴 수 있도록 한다.

public:
	// buildInfo 구조체 
	void SetBuildInfoAll(std::vector<FilePath*> pFilePath);

	// text를 입력받아 저장하는 함수? 
	void CreateStringBuffer(int x, int y, float size, DirectX::XMFLOAT4 color, const char* text);

	// test 최재영 test
	void CreateStringBuffer(TextData* pTextData);

	// UI Texture 파일 경로를 저장해 놓는 함수 
	void SetUITextureData(std::vector<UIData*> textureData);

	void SetAniStateType(std::wstring name, int aniStateType);

	// 하드코딩으로 만들어 버리는 버전
	/// TODO:REMOVE - CreateDXObject 안에서 어떤 mesh 타입인지에 따라 생성을 해준다.   
	void CreateDXObject_Axis();
	void CreateDXObject_Grid();

private:
	// 그릴때 필요한 것들
	HimeDevice* m_pDirect;
	HimeRenderer* m_pRenderer;
	
	HimeCamera* m_pCamera;

	// 만들기 위해서 필요
	ResourceManager* m_pResourceManager;	// 리소스 매니저는 팩토리 등으로 들어가야 한다.
	DXObjectFactory* m_pFactory;

	// 관리하기 (조작 등) 위한 것들
	ObjectManager* m_pObjManager;	// 여기 있으면 안된다.

	int m_ScreenWidth;
	int m_ScreenHeight;

public:
	// Asset를 로드하는 함수. (빌드 정보를 사용해서)
	// 초기화 할떄 다 들고 있게 하기 위해서.
	void LoadAsset_ByBuildInfo(std::wstring IDName);

	// node의 이름, 정보를 주고 받는 함수. 
	std::vector<MeshBuildInfo*> GetMeshInfoList() const { return m_pFactory->m_MeshInfoList; }

	// 게임 엔진에게 전달 해줄 Get함수를 만들자. 
	int FindDXObjectUniqueIndex(std::wstring Name);

	///TODO: draw를 false로 바꿔주는 부분이 없다. 큰일. 
	void SetRenderable(int UniqueIndex);
	
	void SetFireEffect(bool checkDraw);


	/// 개발 중 테스트용 임시 함수
	
	// ASE파일 이름 하나만으로 DXObject들을 만들어준다.
public:
	// Asset를 로드하는 함수. // 초기화 할떄 다 들고 있는 것. 
	void LoadDXObject_FromASEFile(std::wstring fileName);


	/// 마우스 입력 관련
private:
	HWND mhMainWnd;				/// 윈도 핸들
	POINT mLastMousePos;

public:
	void OnMouseDown(int x, int y);
	void OnMouseUp(int x, int y);
	void OnMouseMove(int btnState, int x, int y);
};

