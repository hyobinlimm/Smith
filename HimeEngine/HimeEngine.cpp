#include "HimeDevice.h"
#include "HimeRenderer.h"
#include "HimeEngine.h"

#include "MathHelper.h"

#include "ResourceManager.h"
#include "ObjectManager.h"

#include "HelperObjects.h"
#include "MeshObject.h"
#include "SkinningObject.h"

#include "HimeCamera.h"

HimeEngine::HimeEngine()
	: m_pDirect(nullptr), m_pRenderer(nullptr),
	m_pCamera(nullptr),
	m_pFactory(nullptr), m_pResourceManager(nullptr),
	m_pObjManager(nullptr),
	mhMainWnd(0)
{

}

HimeEngine::~HimeEngine()
{
	SafeDelete(m_pObjManager);
	SafeDelete(m_pFactory);
	m_pResourceManager->DestroyInst();

	SafeDelete(m_pCamera);
	SafeDelete(m_pRenderer);
	m_pDirect->DestroyInst();
}

void HimeEngine::Initialize(HWND hWnd, int screenWidth, int screenHeight)
{
	mhMainWnd = hWnd;

	/// 로딩씬을 할떄 어떻게 할 것인가? 
	m_pDirect = HimeDevice::GetInst();
	m_pDirect->Initialize(hWnd, screenWidth, screenHeight);
	m_pDirect->CreateRenderState();

	m_pRenderer = new HimeRenderer();
	m_pRenderer->Initialize();

	m_pCamera = new HimeCamera();
	m_pCamera->LookAt(XMFLOAT3(8.0f, 8.0f, -8.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1.0f, 0));

	/// 원래 m_pResourceManager init할때 build vector를 받았는데, 
	/// 이제는 SetBuildInfoAll(pFilePath)를 사용하여 정보를 넣어줘야 한다. 
	m_pResourceManager = ResourceManager::GetInst();
	m_pResourceManager->Initialize();

	m_pFactory = new DXObjectFactory();

	OnResize(screenWidth, screenHeight);
	//	OnResize();

	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;

	m_pObjManager = new ObjectManager();
	m_pObjManager->Init();
}

void HimeEngine::Initialize(HWND hWnd, int screenWidth, int screenHeight, CameraInfo* pCameraInfo)
{
	m_pDirect = HimeDevice::GetInst();
	m_pDirect->Initialize(hWnd, screenWidth, screenHeight);

	m_pRenderer = new HimeRenderer();
	m_pRenderer->Initialize();

	m_pCamera = new HimeCamera();
	//m_pCamera->LookAt(XMFLOAT3(200.0f, 200.0f, -200.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1.0f, 0));
	m_pCamera->LookAt(pCameraInfo->Pos, pCameraInfo->Target, pCameraInfo->Up);

	/// 원래 m_pResourceManager init할때 build vector를 받았는데, 
	/// 이제는 SetBuildInfoAll(pFilePath)를 사용하여 정보를 넣어줘야 한다. 
	m_pResourceManager = ResourceManager::GetInst();
	m_pResourceManager->Initialize();

	m_pFactory = new DXObjectFactory();

	OnResize(screenWidth, screenHeight);

	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;
	m_pDirect->CreateRenderState();

	m_pObjManager = new ObjectManager();
	m_pObjManager->Init();
}

void HimeEngine::OnResize(int screenWidth, int screenHeight)
{
	m_pDirect->OnResize(screenWidth, screenHeight);

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	/// 창의 크기가 변했으므로, 종횡비를 업데이트하고 투영 행렬을 재계산한다.
	m_pCamera->SetLens(0.25f * MathHelper::Pi, GetAspectRatio(screenWidth, screenHeight), 1.0f, 1000.0f);
}

void HimeEngine::OnResize()
{
	m_pDirect->OnResize(m_ScreenWidth, m_ScreenHeight);
	m_pCamera->SetLens(0.25f * MathHelper::Pi, GetAspectRatio(m_ScreenWidth, m_ScreenHeight), 1.0f, 1000.0f);
}

void HimeEngine::SetScreenSize(int screenWidth, int screenHeight)
{
	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;
}

void HimeEngine::Update(float deltaTime)
{
	m_pRenderer->SetClientWidth(m_ScreenWidth);
	m_pRenderer->SetClientHeight(m_ScreenHeight);
	m_pCamera->Update(deltaTime);
	m_pRenderer->Update(m_pCamera, deltaTime);
}

void HimeEngine::Update(float deltaTime, MeshBuildInfo* pMeshInfo)
{
	//m_pCamera->Update(deltaTime);

	//현 모니터의 최대화면크기를 가져옴
	m_pRenderer->SetClientWidth(m_ScreenWidth);
	m_pRenderer->SetClientHeight(m_ScreenHeight);
	m_pRenderer->Update(m_pCamera, deltaTime, pMeshInfo);
}

void HimeEngine::TextBufferClear()
{
	// 다음 텍스트를 넘어갈 때 버퍼를 비워주자. 
	m_pRenderer->TextBufferClear();
}

void HimeEngine::UIBufferClaer()
{
	// 다음 UI를 넘어갈 때 버퍼를 비워주자. 
	m_pRenderer->TextureBufferClaer();
}

void HimeEngine::CameraUpdate(CameraInfo* pCameraInfo)
{
	m_pCamera->UpdateViewMatrix(pCameraInfo);
}

void HimeEngine::Render()
{
	m_pRenderer->BeginRender();

	m_pRenderer->Render();

	m_pRenderer->EndRender();
}

void HimeEngine::Destroy()
{

}

float HimeEngine::GetAspectRatio(int screenWidth, int screenHeight) const
{
	return static_cast<float>(screenWidth) / screenHeight;
}

void HimeEngine::SetBuildInfoAll(std::vector<FilePath*> pFilePath)
{
	m_pResourceManager->SetBuildInfoAll(pFilePath);

	// 게임엔진이 건내준 파일경로를 보고, 미리 데이터를 읽어서 저장해 놓는다. 
	for (auto _pathInfo : pFilePath)
	{
		LoadAsset_ByBuildInfo(_pathInfo->ID_name);
	}
}

void HimeEngine::CreateStringBuffer(int x, int y, float size, DirectX::XMFLOAT4 color, const char* text)
{
	m_pRenderer->CreateStirngData(x, y, size, color, text);
}

void HimeEngine::CreateStringBuffer(TextData* pTextData)
{
	m_pRenderer->CreateStirngData(pTextData);
}

void HimeEngine::SetUITextureData(std::vector<UIData*> textureData)
{
	for (auto _nowTex : textureData)
	{
		m_pRenderer->CreateTextureData(_nowTex);
	}
}

void HimeEngine::SetAniStateType(std::wstring name, int aniStateType)
{
	// 게임엔진에서 animation의 상태를 바꿔준다. 
	for (auto obj : m_pRenderer->GetRenderingQueue())
	{
		if (obj->m_ID == name)
		{
			obj->SetAniType(aniStateType);
		}
	}
}

void HimeEngine::CreateDXObject_Axis()
{
	// 하드 코딩 로드 된 Axis를 받아올 지역 변수
	DXObjectBase* _newAxis = nullptr;
	_newAxis = m_pFactory->CreateDXObject_Axis();

	m_pRenderer->AddDXObjectToRenderQueue(_newAxis);
}

void HimeEngine::CreateDXObject_Grid()
{
	// 하드 코딩 로드 된 Grid를 받아올 지역 변수
	DXObjectBase* _newGrid = nullptr;
	_newGrid = m_pFactory->CreateDXObject_Grid();

	m_pRenderer->AddDXObjectToRenderQueue(_newGrid);
}

void HimeEngine::LoadAsset_ByBuildInfo(std::wstring IDName)
{
	/// 게임 엔진에서 사용할 오브젝트의 데이터를 모두 읽어서 담아두자.
	std::vector<DXObjectBase*> _Objects;

	/// 이미 intialize할 때, buildInfo(파일 이름, 파일경로)를 다 가지고 있다. 
	m_pFactory->CreateDXObject(IDName, _Objects);

	for (DXObjectBase* _nowDXObject : _Objects)
	{
		m_pRenderer->AddDXObjectToRenderQueue(_nowDXObject);
	}

	// 위에 push한 객체 메모리 해제 
	for (DXObjectBase* _nowDXObject : _Objects)
	{
		_nowDXObject = nullptr;
	}

	_Objects.clear();
}

int HimeEngine::FindDXObjectUniqueIndex(std::wstring Name)
{
	/// index를 엔진에 넘겨준다. 

	for (UINT i = 0; i < m_pRenderer->GetRenderingQueue().size(); i++)
	{
		if (Name == m_pRenderer->GetRenderingQueue().at(i)->m_NodeName)
		{
			return i;
		}
	}

	return NULL; // 흠... 못찾으면 오또케 하지. 
}

void HimeEngine::SetRenderable(int UniqueIndex)
{
	m_pRenderer->GetRenderingQueue().at(UniqueIndex)->m_isDraw = true;
}

void HimeEngine::SetFireEffect(bool checkDraw)
{
	m_pRenderer->SetFireEffect(checkDraw);
}

void HimeEngine::LoadDXObject_FromASEFile(std::wstring fileName)
{

}

void HimeEngine::OnMouseDown(int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void HimeEngine::OnMouseUp(int x, int y)
{
	ReleaseCapture();
}

void HimeEngine::OnMouseMove(int btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		m_pCamera->Pitch(dy);
		m_pCamera->RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

