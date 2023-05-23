#include "CCPch.h"
#include "UIRenderer.h"
#include "SceneManager.h"

UIRenderer::UIRenderer()
{

}

void UIRenderer::Awake()
{

}

void UIRenderer::Start()
{

}

void UIRenderer::Update()
{

}

void UIRenderer::Render()
{
	
}

void UIRenderer::SetUIData(wstring UIName,wstring UIType,  wstring FileName,wstring Data, wstring Type, bool isDraw, bool isMovable)
{
	if (m_UIData == nullptr)
	{
		m_UIData = make_shared<UIData>();
	}
	m_UIData->Name = UIName;
	m_UIData->UIType = UIType;
	m_UIData->FileName = FileName;
	m_UIData->Data = Data;
	m_UIData->Type = Type;
	//m_UIData->left = left;
	//m_UIData->top = top;
	//m_UIData->right = right;
	//m_UIData->bottom = bottom;
	m_UIData->isDraw = isDraw;
	m_UIData->isMovable = isMovable;
}

