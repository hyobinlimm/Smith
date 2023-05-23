#pragma once

/// <summary>
/// DX������Ʈ���� �θ� (�������̽�)
/// 
/// 
/// DX������Ʈ?
/// DirectX11���� �������� ����� �Ǵ� ������Ʈ.
/// ���������� ���ҽ����� ������ �ִ�.
/// ���ҽ� : VB, IB, Texture.... constant Buffer
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

