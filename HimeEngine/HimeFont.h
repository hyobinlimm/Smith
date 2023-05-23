#pragma once

#include "SpriteFont.h"
#include "SpriteBatch.h"

/// <summary>
/// DXTK�� �̿��� �ؽ�Ʈ ���
/// 
/// ������ Ŭ���� �����ؿ�. 
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
	//void DrawTextColor(int x, int y, DirectX::XMFLOAT4 color, TCHAR* text, ...); // ������ ����
	void DrawTextColor(int x, int y, float size, DirectX::XMFLOAT4 color, bool isDraw, TCHAR* text,...); // ��Ʈ ũ������ ������ ȿ�� ����

private:
	DirectX::SpriteBatch* m_pSpriteBatch;
	DirectX::SpriteFont* m_pSpriteFont;

	// �׳� ����ϸ� ���� ������ �����.
	HimeDevice* m_pDirect3D; 

	ID3D11RasterizerState* m_RasterizerState;
	ID3D11DepthStencilState* m_DepthStencilState;

	/// WriteFactory�� �̿��� â ��ȯ���� ����� ��Ʈ�� ����. �����ؾ� ��. 
};

