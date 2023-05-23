#pragma once
#include "DirectXDefine.h"
#include "InputLayouts.h"

/// <summary>
/// 객체가 생성되면, 내가 뭔진 모르겠지만 
/// 리소스 매니저에서 받아 온
/// indexBuffer, vertexBuffer를 들고 있는 객체
/// </summary>

class GeometryBuffer
{
public:
	GeometryBuffer();
	GeometryBuffer(GeometryBuffer& other); // 복사 생성자
	GeometryBuffer(GeometryBuffer&& other) noexcept;  // 이동생성자.
	//todo:: noexcept 키워드 공부하기

	~GeometryBuffer();

public: // Get, Set 함수들 
	void SetVertexSize(int val) { m_VertexSize = val; }
	void SetIndexSize(int val) { m_IndexSize = val; }
	int GetIndexSize() const { return m_IndexSize; }

	ID3D11Buffer* GetVertexBuffer() const { return m_VB; }
	void SetVertexBuffer(ID3D11Buffer* val) { m_VB = val; }
	ID3D11Buffer* GetIndexBuffer() const { return m_IB; }
	void SetIndexBuffer(ID3D11Buffer* val) { m_IB = val; }

private:
	ID3D11Buffer* m_VB;
	ID3D11Buffer* m_IB;

	int m_VertexSize;
	int m_IndexSize;

public:
	// 현재 자신이 어떤 오브젝트인지 구별하는 변수
	std::wstring m_Name;
};

