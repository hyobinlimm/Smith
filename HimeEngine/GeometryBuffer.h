#pragma once
#include "DirectXDefine.h"
#include "InputLayouts.h"

/// <summary>
/// ��ü�� �����Ǹ�, ���� ���� �𸣰����� 
/// ���ҽ� �Ŵ������� �޾� ��
/// indexBuffer, vertexBuffer�� ��� �ִ� ��ü
/// </summary>

class GeometryBuffer
{
public:
	GeometryBuffer();
	GeometryBuffer(GeometryBuffer& other); // ���� ������
	GeometryBuffer(GeometryBuffer&& other) noexcept;  // �̵�������.
	//todo:: noexcept Ű���� �����ϱ�

	~GeometryBuffer();

public: // Get, Set �Լ��� 
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
	// ���� �ڽ��� � ������Ʈ���� �����ϴ� ����
	std::wstring m_Name;
};

