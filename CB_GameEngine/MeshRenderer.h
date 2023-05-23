#pragma once

#include "Renderer.h"
#include "HimeRendererStructs.h"

/// <summary>
/// Graphics Engine���� ������Ʈ Render�� �ʿ��� ����ü�� ������ָ�
/// �� ����ü�� ������ �̰����� ������ ����ش�����
/// Graphics Engine�� �������� �Ѱ��־ �׸� ����.
/// </summary>

class MeshRenderer : public Renderer
{
public:
	MeshRenderer();
	virtual ~MeshRenderer() {};

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void Render() override;
	
	void SetName(wstring name);
	wstring GetName();
	MeshBuildInfo* GetMeshInfo() { return m_MeshInfo; }
	void SetMeshInfo(MeshBuildInfo* meshInfo) { m_MeshInfo = meshInfo; }

private:
	wstring m_ObjName;
	MeshBuildInfo* m_MeshInfo;

	// ����� Ÿ�̸�
	float m_Timer;
};