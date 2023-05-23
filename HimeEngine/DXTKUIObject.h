#pragma once

#include "DirectXDefine.h"
#include "SpriteBatch.h"
#include <map>

/// <summary>
/// DXTK를 이용한 2D Texture 출력
///
/// https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=neo_seven&logNo=40114350639#:~:text=SpriteBatch%ED%81%B4%EB%9E%98%EC%8A%A4%EB%8A%94%20XNA%20Framework,%EC%A3%BC%EC%9A%94%ED%95%9C%20%EB%AA%A9%EC%A0%81%EC%9C%BC%EB%A1%9C%20%EB%91%90%EC%97%88%EC%8A%B5%EB%8B%88%EB%8B%A4
/// https://docs.microsoft.com/en-us/previous-versions/windows/xna/bb203893(v=xnagamestudio.40)
/// https://github.com/microsoft/DirectXTK/wiki/SpriteBatch - effect를 costom 하는 방법이 나와있음. 
/// https://m.blog.naver.com/sorkelf/40169749877 - DirectX11 에서 텍스처 다루는 방법? ? 제대로 이해 못함. 
/// 위의 블로그 와 msdn 참고
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

	// 스프라이트 애니메이션 재생할 부분.
	void RenderSprite(std::wstring fileName, int left, int top, int right, int bottom, int frame, float width, float height);

	void RotationSprite(std::wstring fileName, int left, int top, int right, int bottom, float rotate);

private:
	DirectX::SpriteBatch* m_pSpriteBatch;

	HimeDevice* m_pDirect3D;
	
	// texture가 담긴 ID3D11ShaderResourceView 포인터를 관리하는 vector
	std::map<std::wstring, ID3D11ShaderResourceView*> m_TextureList;
	
	//const XMFLOAT2& m_Origin;
	float m_Rotation;
	float m_Scale;
	float m_Depth;
};

