#pragma once

/// <summary>
/// DX오브젝트들의 부모 (인터페이스)
/// 
/// 
/// DX오브젝트?
/// DirectX11에서 렌더링의 대상이 되는 오브젝트.
/// 내부적으로 리소스들을 가지고 있다.
/// 리소스 : VB, IB, Texture.... constant Buffer
/// </summary>

class HimeCamera;

class IDXObjectBase
{
public:
	IDXObjectBase();
	virtual ~IDXObjectBase();
	
	virtual void Update(HimeCamera* pCamera) abstract;
	virtual void Render() abstract;

};

