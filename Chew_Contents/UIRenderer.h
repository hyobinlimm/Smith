#pragma once

#include "Renderer.h"
#include "HimeRendererStructs.h"


class UIRenderer : public Renderer
{
public:
	UIRenderer();
	virtual ~UIRenderer() {};
public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void Render() override;

	void SetUIData(wstring UIName, wstring UIType, wstring FileName, wstring Data, wstring Type, bool isDraw, bool isMovable);

	shared_ptr<UIData> GetUIData() const { return m_UIData; }
	void SetUIData(shared_ptr<UIData> val) { m_UIData = val; }


private:
	shared_ptr<UIData> m_UIData;
};

