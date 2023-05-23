#include "DXTKUIObject.h"
#include "HimeDevice.h"

#define SafeDelete(x) { delete x; x = 0; }
#define SAFE_RELEASE(x) { if(x){ x->Release(); x = 0; } }	// 예전 스타일

DXTKUIObject::DXTKUIObject()
	: m_pSpriteBatch(nullptr)
{
	m_pDirect3D = HimeDevice::GetInst();
}

DXTKUIObject::~DXTKUIObject()
{
	SafeDelete(m_pSpriteBatch);
}

void DXTKUIObject::Create()
{
	ID3D11DeviceContext* pDC = nullptr;
	m_pDirect3D->GetD3dDevice()->GetImmediateContext(&pDC);

	m_pSpriteBatch = new DirectX::SpriteBatch(pDC);

	SAFE_RELEASE(pDC);
}

void DXTKUIObject::CreateTextureList(std::wstring fileName)
{
	if (fileName == L"")
		return;

	ID3D11Resource* texResource = nullptr;

	ID3D11ShaderResourceView* _newUITexSRV;

	std::wstring _textPath(L"../Resource/Texture/UI/" + fileName + L".dds");
	//std::wstring _texturPath_wstr = std::wstring(_textPath.begin(), _textPath.end());
	//const wchar_t* _texFilePath = _texturPath_wstr.c_str();

	HR(CreateDDSTextureFromFile(m_pDirect3D->GetD3dDevice(),
		_textPath.c_str(), &texResource, &_newUITexSRV));

	// 사용할 texture 파일의 이름을 벡터에 담아서 들고 있을 예정. 
	m_TextureList.insert(make_pair(fileName, _newUITexSRV));	
}

void DXTKUIObject::SetTexture()
{
	//리소스를 넣어주자. 

	ID3D11Resource* texResource = nullptr;
	
	ID3D11ShaderResourceView* _newUITexSRV;
	
	HR(CreateDDSTextureFromFile(m_pDirect3D->GetD3dDevice(),
		L"../HimeEngine/Model/Textures/anvil_h_paint_base.dds", &texResource, &_newUITexSRV));
}

void DXTKUIObject::DrawTexture(std::wstring fileName,int left, int top, int right, int bottom)
{
	RECT _rec = { left, top, right, bottom };

	m_pSpriteBatch->Begin();

	m_pSpriteBatch->Draw(m_TextureList.at(fileName), _rec);

	m_pSpriteBatch->End();
}

void DXTKUIObject::RenderSprite(std::wstring fileName, int left, int top, int right, int bottom, int frame, float width, float height)
{
	RECT _rec1 = { left, top, right, bottom };
	RECT _rec2 = { left, top, right, bottom };
	if (width == 16128)
	{
	

		_rec2.left = (width / 7) * frame;
		_rec2.top = top;
		_rec2.right = (width / 7) * (frame + 1);
		_rec2.bottom = height;
	}
	else
	{
		_rec1.left = 970.0f / 2560.0f  * right;
		_rec1.top = 360.0f / 1440.0f * bottom;
		_rec1.right = 1590.0f / 2560.0f * right;
		_rec1.bottom = 980.0f / 1440.0f * bottom;

		_rec2.left = (width / 16) * frame;
		_rec2.top = top;
		_rec2.right = (width / 16) * (frame + 1);
		_rec2.bottom = height;
	}

	m_pSpriteBatch->Begin();

	m_pSpriteBatch->Draw(m_TextureList.at(fileName), _rec1, &_rec2);

	m_pSpriteBatch->End();
}

void DXTKUIObject::RotationSprite(std::wstring fileName, int left, int top, int right, int bottom, float rotate)
{
	RECT _rec1 = { left + (200.f / 2560.f) * right, top + (260.f / 1440.f) * bottom, right, bottom };

	RECT _rec2 = { left, top, right, bottom };

	m_pSpriteBatch->Begin();

	m_pSpriteBatch->Draw(m_TextureList.at(fileName), _rec1, &_rec2, Colors::White, rotate, {200, 260});

	m_pSpriteBatch->End();
}
