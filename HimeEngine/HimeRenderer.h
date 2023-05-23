#pragma once

#include "HimeRendererStructs.h"



class HimeDevice;
class HimeCamera;
class HimeFont;
class DXObjectBase;
class DXTKUIObject;
class FireEffect;

/// <summary>
/// 오직 그리기만 하는 클래스
/// 
/// 2022.07.15 HyobinLogic
/// </summary>
class HimeRenderer
{
public:
	HimeRenderer();
	~HimeRenderer();

public:
	// GetSet 함수
	void SetClientWidth(int val) { m_ClientWidth = val; }
	void SetClientHeight(int val) { m_ClientHeight = val; }

	std::vector<DXObjectBase*> GetRenderingQueue() const { return m_RenderingQueue; }
	void SetRenderingQueue(std::vector<DXObjectBase*> val) { m_RenderingQueue = val; }

public:
	void Initialize();

	void Update(HimeCamera* pCamera, float deltaTime); // test 용
	void Update(HimeCamera* pCamera, float deltaTime, MeshBuildInfo* pMeshInfo);

	// 최재영이 추가한 텍스트 데이터까지 넘겨받는 업데이트
	void TextUpdate(TextData* pTextData);
	void TextureUpdate(UIData* pTextureData);

	// 다 그렸다면 담았던 버퍼를 비워준다. 
	void TextBufferClear();
	void TextureBufferClaer();
	
	void UpdateBoneWorldTM(std::vector<DXObjectBase*> objects);

	void BeginRender();
	void Render();
	void EndRender();

	void SetFireEffect(bool checkDraw);

public:
	// 외부에 노출 될 인터페이스
	void AddDXObjectToRenderQueue(DXObjectBase* dxObject);

	// 입력받은 문자를 textdata 구조체에 담는 함수 
	void CreateStirngData(int x, int y, float size, DirectX::XMFLOAT4 color, const char* text);


	void CreateStirngData(TextData* pTextData);
	
	// 입력받은 TextureData 구조체에 담는 함수 
	void CreateTextureData(UIData* textureData);

	
private:
	void RenderText(); // vector에 담긴 text를 출력하는 함수. 

private:
	int m_ClientWidth;
	int m_ClientHeight;

	bool m_isFireDraw;

private:
	// for DX11
	HimeDevice* m_pDirect3D;
	
	HimeFont* m_pFont;

	DXTKUIObject* m_pTexture2D;

	// 애니메이션 프레임을 돌리기 위한 변수
	UINT m_LastFrame;

	// 그릴 때 필요한 객체와 그릴 객체
	/// 다형적 동작을 통해서 렌더링이 될 대상인 DX오브젝트들
	std::vector<DXObjectBase*> m_RenderingQueue;

	/// text정보를 담아둔 곳.
	std::vector<TextData*> m_RenderText;

	/// UI 정보를 담아둔 곳
	std::vector<UIData*> m_RenderUI;

	///TODE:TEST - 임시 불
	FireEffect* m_Fire;
	HimeCamera* m_Cam;
};


