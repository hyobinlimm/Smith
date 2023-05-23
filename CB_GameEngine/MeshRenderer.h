#pragma once

#include "Renderer.h"
#include "HimeRendererStructs.h"

/// <summary>
/// Graphics Engine에서 오브젝트 Render에 필요한 구조체를 만들어주면
/// 그 구조체를 가지고 이곳에서 정보를 담아준다음에
/// Graphics Engine에 정보들을 넘겨주어서 그릴 예정.
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

	// 버블용 타이머
	float m_Timer;
};