#pragma once

#include "DirectXDefine.h"
#include "SpriteBatch.h"
#include <map>

/// <summary>
/// DXTK�� �̿��� 2D Texture ���
///
/// https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=neo_seven&logNo=40114350639#:~:text=SpriteBatch%ED%81%B4%EB%9E%98%EC%8A%A4%EB%8A%94%20XNA%20Framework,%EC%A3%BC%EC%9A%94%ED%95%9C%20%EB%AA%A9%EC%A0%81%EC%9C%BC%EB%A1%9C%20%EB%91%90%EC%97%88%EC%8A%B5%EB%8B%88%EB%8B%A4
/// https://docs.microsoft.com/en-us/previous-versions/windows/xna/bb203893(v=xnagamestudio.40)
/// https://github.com/microsoft/DirectXTK/wiki/SpriteBatch - effect�� costom �ϴ� ����� ��������. 
/// https://m.blog.naver.com/sorkelf/40169749877 - DirectX11 ���� �ؽ�ó �ٷ�� ���? ? ����� ���� ����. 
/// ���� ��α� �� msdn ����
/// </summary>

class HimeDevice;

class DXTKUIObject
{
public:
	DXTKUIObject();
	~DXTKUIObject();

public:
	void Create();

	void CreateTextureList(std::wstring fileName);
	//void SetTexture(std::string textureName);
	void SetTexture();

	void DrawTexture(std::wstring fileName,  int left, int top, int right, int bottom);

	// ��������Ʈ �ִϸ��̼� ����� �κ�.
	void RenderSprite(std::wstring fileName, int left, int top, int right, int bottom, int frame, float width, float height);

	void RotationSprite(std::wstring fileName, int left, int top, int right, int bottom, float rotate);

private:
	DirectX::SpriteBatch* m_pSpriteBatch;

	HimeDevice* m_pDirect3D;
	
	// texture�� ��� ID3D11ShaderResourceView �����͸� �����ϴ� vector
	std::map<std::wstring, ID3D11ShaderResourceView*> m_TextureList;
	
	//const XMFLOAT2& m_Origin;
	float m_Rotation;
	float m_Scale;
	float m_Depth;
};

