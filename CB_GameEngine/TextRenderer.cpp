#include "CBpch.h"
#include "SceneManager.h"
#include "TextRenderer.h"
#include "HimeEngine.h"
#include "Transform.h"

TextRenderer::TextRenderer()
{
}

TextRenderer::~TextRenderer()
{

}

void TextRenderer::Awake()
{
	m_SceneManager = SceneManager::GetInstance();
	m_Transform = m_pGameObject->GetComponent<Transform>();
}

void TextRenderer::Start()
{

}

void TextRenderer::Update()
{
	if (m_Transform != nullptr && m_pTextData !=nullptr)
	{
		m_pTextData->value = m_Transform->GetLocalPosition().x;
		m_pTextData->value2 = m_Transform->GetLocalPosition().y;
		m_pTextData->value3 = m_Transform->GetLocalPosition().z;
	}
}

void TextRenderer::Render()
{

}


void TextRenderer::SetTexData(std::wstring name, int x, int y, float size, XMFLOAT4 color, bool isDraw, std::wstring text, float value, float value2, float value3)
{
	m_pTextData = make_shared<TextData>();

	m_pTextData->IDName = name;
	m_pTextData->x = x;
	m_pTextData->y = y;
	m_pTextData->color = color;
	m_pTextData->FontSize = size;

	m_pTextData->text = text;

	m_pTextData->value = value;
	m_pTextData->value2 = value2;
	m_pTextData->value3 = value3;

	m_pTextData->isDraw = isDraw;
}