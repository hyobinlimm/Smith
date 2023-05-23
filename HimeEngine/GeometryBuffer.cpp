#include "GeometryBuffer.h"
#include <DirectXColors.h>

GeometryBuffer::GeometryBuffer()
	:m_VB(nullptr), m_IB(nullptr),
	m_VertexSize(0), m_IndexSize(0),
	m_Name()
{

}

GeometryBuffer::GeometryBuffer(GeometryBuffer& other)
{
	m_VB = other.m_VB;
	m_IB = other.m_IB;
	m_VertexSize = other.m_VertexSize;
	m_IndexSize = other.m_IndexSize;

	m_Name = other.m_Name;
}



GeometryBuffer::GeometryBuffer(GeometryBuffer&& other) noexcept
	: m_VB(other.m_VB), m_IB(other.m_IB),
	m_VertexSize(other.m_VertexSize), m_IndexSize(other.m_IndexSize),
	m_Name(other.m_Name)
{
	other.m_VB = nullptr;
	other.m_IB = nullptr;
	other.m_VertexSize = 0;
	other.m_IndexSize = 0;
	
	m_Name = other.m_Name;
}

GeometryBuffer::~GeometryBuffer()
{

}


