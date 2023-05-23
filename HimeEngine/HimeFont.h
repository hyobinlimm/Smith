#pragma once

#include "SpriteFont.h"
#include "SpriteBatch.h"

/// <summary>
/// DXTK를 이용한 텍스트 출력
/// 
/// 교수님 클래스 복사해옴. 
/// </summary>

class HimeDevice;

class HimeFont
{
public:

	HimeFont();
	~HimeFont();

public:
	void Create(/*ID3D11Device* pDevice, ID3D11RasterizerState* rs, ID3D11DepthStencilState* ds*/);

	void DrawTest();
	//void DrawTextColor(int x, int y, DirectX::XMFLOAT4 color, TCHAR* text, ...); // 교수님 버전
	void DrawTextColor(int x, int y, float size, DirectX::XMFLOAT4 color, bool isDraw, TCHAR* text,...); // 폰트 크기조절 가능한 효빈 버전

private:
	DirectX::SpriteBatch* m_pSpriteBatch;
	DirectX::SpriteFont* m_pSpriteFont;

	// 그냥 사용하면 뎁스 문제가 생긴다.
	HimeDevice* m_pDirect3D; 

	ID3D11RasterizerState* m_RasterizerState;
	ID3D11DepthStencilState* m_DepthStencilState;

	/// WriteFactory를 이용한 창 변환에도 깔끔한 폰트가 나옴. 구현해야 함. 
};

