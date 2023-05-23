
#include "SpriteFont.h"
#include "SpriteBatch.h"
#include "HimeFont.h"
#include "HimeDevice.h"

#define SafeDelete(x) { delete x; x = 0; }
#define SAFE_RELEASE(x) { if(x){ x->Release(); x = 0; } }	// ���� ��Ÿ��

#include "SimpleMath.h"

HimeFont::HimeFont()
	: m_pSpriteBatch(nullptr), m_pSpriteFont(m_pSpriteFont), 
	m_RasterizerState(nullptr), m_DepthStencilState(nullptr)
{
	m_pDirect3D = HimeDevice::GetInst();
}

HimeFont::~HimeFont()
{
	SafeDelete(m_pSpriteFont);
	SafeDelete(m_pSpriteBatch);
}

void HimeFont::Create(/*ID3D11Device* pDevice, ID3D11RasterizerState* rs, ID3D11DepthStencilState* ds*/)
{
	ID3D11DeviceContext* pDC = nullptr;
	m_pDirect3D->GetD3dDevice()->GetImmediateContext(&pDC);

	m_pSpriteBatch = new DirectX::SpriteBatch(pDC);

	//TCHAR* _fileName = (TCHAR*)L"../HimeEngine/Font/gulim9k.spritefont";
	TCHAR* _fileName = (TCHAR*)L"../Resource/Font/gulim9k.spritefont";

	m_pSpriteFont = new DirectX::SpriteFont(m_pDirect3D->GetD3dDevice(), _fileName);
	m_pSpriteFont->SetLineSpacing(14.0f); // �ٰ���
	//m_pSpriteFont->SetDefaultCharacter(' ');

	SAFE_RELEASE(pDC);

	m_RasterizerState = m_pDirect3D->GetSolidRS();
	m_DepthStencilState = m_pDirect3D->GetNormalDSS();
}

void HimeFont::DrawTest()
{
	m_pSpriteBatch->Begin();
	m_pSpriteFont->DrawString(m_pSpriteBatch, L"Hello, world! �ѱ��� �ȵǳ�? �c�氢�� ���� �ݶ� �z�z", DirectX::XMFLOAT2(10.f, 10.f));
	m_pSpriteBatch->End();
}

//void HimeFont::DrawTextColor(int x, int y, DirectX::XMFLOAT4 color, TCHAR* text, ...)
//{
//	TCHAR _buffer[1024] = L"";
//	va_list vl;
//	va_start(vl, text);
//	_vstprintf(_buffer, 1024, text, vl);
//	va_end(vl);
//
//	// SpriteBatch�� ���� ���� ������Ʈ�� ������ �� �ִ�.
//	// ������, �׳� Begin�� �ϸ� �������Ľǹ��� �ɼ��� D3D11_DEPTH_WRITE_MASK_ZERO�� �Ǵ� ��. DSS�� �ٷ��� �ʴ� ���ݿ��� ������ �� �� �ִ�.
//	// �Ʒ�ó�� ���⿡ �ɼ��� ALL�� �־��༭ ZERO�� �Ǵ� ���� ���� ���� �ְ�, �ٸ� �� ������Ʈ�� �׸� �� ����������Ʈ�� ���� �� �� ���� �ִ�.
//	// DX12���� ����������Ʈ���� �׷��� �������� �͵� ���ش� ����. ���� ���� �ȵ������..
//	///m_pSpriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred, nullptr, nullptr, m_DepthStencilState/*, m_RasterizerState*/);
//	m_pSpriteBatch->Begin();
//	m_pSpriteFont->DrawString(m_pSpriteBatch, _buffer, DirectX::XMFLOAT2((float)x, (float)y), DirectX::SimpleMath::Vector4(color));
//	m_pSpriteBatch->End();
//}

/// <summary>
/// ��ũ ũ�� ���� ������ ��Ʈ ��� �Լ�
/// </summary>
/// <param name="x">postion.x</param>
/// <param name="y">postion.y</param>
/// <param name="size">Font Size</param>
/// <param name="color">Font Color</param>
/// <param name="text">����� ���� ����</param>
/// <param name="">���� ����</param>
void HimeFont::DrawTextColor(int x, int y, float size, DirectX::XMFLOAT4 color, bool isDraw, TCHAR* text, ...)
{
	if (isDraw == false)
		return;

	///TODO: ���⼭ _buffer�� �޸𸮸� �ʹ� ũ�� ��Ƽ� �� �޸� ħ�� ��. ū��
	TCHAR _buffer[1024] = L"";
	va_list vl;
	va_start(vl, text);
	_vstprintf(_buffer, 1024, text, vl);
	va_end(vl);

	// SpriteBatch�� ���� ���� ������Ʈ�� ������ �� �ִ�.
	// ������, �׳� Begin�� �ϸ� �������Ľǹ��� �ɼ��� D3D11_DEPTH_WRITE_MASK_ZERO�� �Ǵ� ��. DSS�� �ٷ��� �ʴ� ���ݿ��� ������ �� �� �ִ�.
	// �Ʒ�ó�� ���⿡ �ɼ��� ALL�� �־��༭ ZERO�� �Ǵ� ���� ���� ���� �ְ�, �ٸ� �� ������Ʈ�� �׸� �� ����������Ʈ�� ���� �� �� ���� �ִ�.
	// DX12���� ����������Ʈ���� �׷��� �������� �͵� ���ش� ����. ���� ���� �ȵ������..
	m_pSpriteBatch->Begin();
	m_pSpriteFont->DrawString(m_pSpriteBatch, _buffer, DirectX::XMFLOAT2((float)x, (float)y), DirectX::SimpleMath::Vector4(color), 0.f, {0.f, 0.f}, size);
	m_pSpriteBatch->End();
}
