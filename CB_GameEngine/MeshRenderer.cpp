#include "CBpch.h"
#include "MeshRenderer.h"
#include "IComponent.h"
#include "Transform.h"
#include "SceneManager.h"
#include "GameTimer.h"

MeshRenderer::MeshRenderer()
	:m_ObjName(), m_Timer(0.f)
{
	//m_MeshInfo = make_shared<MeshBuildInfo>();
	
}

void MeshRenderer::Awake()
{
	if (m_pGameObject->GetName() == L"Smith")
	{
		m_Transform->Translate({5, 0, -5});
	}
}

void MeshRenderer::Start()
{

}

void MeshRenderer::Update()
{
	/*m_MeshInfo->Rot = m_pGameObject->GetComponent<Transform>()->GetWorldRotate();
	m_MeshInfo->Scale = m_pGameObject->GetComponent<Transform>()->GetWorldScale();*/

	m_MeshInfo->Pos = { m_Transform->GetWorldPosition().x,m_Transform->GetWorldPosition().y, m_Transform->GetWorldPosition().z, 0 };
	m_MeshInfo->Rot = { m_Transform->GetWorldRotate().x,m_Transform->GetWorldRotate().y, m_Transform->GetWorldRotate().z, 0 };
	m_MeshInfo->Scale = { m_Transform->GetWorldScale().x,m_Transform->GetWorldScale().y, m_Transform->GetWorldScale().z, 0 };

	if (m_pGameObject->GetName().find(L"bubble")!=wstring::npos)
	{
		if (m_MeshInfo->isDraw == true)
		{
			m_Timer += GameTimer::GetInstance()->DeltaTime();
			if (m_Timer >= 3.f)
			{
				m_Timer = 0.f;
				m_MeshInfo->isDraw = false;
			}
		}
	}
}

void MeshRenderer::Render()
{
	int _ID = SceneManager::GetInstance()->m_HimeEngine->FindDXObjectUniqueIndex(m_MeshInfo->NodeName);
	SceneManager::GetInstance()->m_HimeEngine->SetRenderable(_ID);
}

void MeshRenderer::SetName(wstring name)
{
	this->m_ObjName = name;
}

wstring MeshRenderer::GetName()
{
	return m_ObjName;
}
