#pragma once
#include <Component.h>

class UIRenderer;
class SceneManager;
class UI :
	public Component
{
public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;


public:
	void OpenUI();
	void CloseUI();
	void SetUIList(wstring name);

	void SwitchUI();

	bool IsOpen();
	vector<shared_ptr<UIRenderer>> GetUIList() { return m_UIList; };



private:
	shared_ptr<SceneManager> m_Scene;

	bool m_IsOpen = false;
	vector<shared_ptr<UIRenderer>> m_UIList;
};

/// UIManager에서 UIData를 생성
/// 씬에서 UIData를 바탕으로 오브젝트를 생성
/// 
/// UIComponent에서 씬에서 오브젝트들을 찾아서 관련된 오브젝트들을 벡터로 만들어서 가지고 있음
/// 
/// 여기까지 갑자기 왜 왔지
///
/// 렌더러 안에 UIdata
/// 
///
/// UIRenderer에 있는 데이터랑
/// 매니저에 있는 데이터랑 별개아님?
/// 매니저에서 수정한다고 렌더러 수정 안되잖아.
/// 개별로 수정해야 되는 애들이 있어서
/// 
/// 바 같은거 개별로 수정해야되가지고
/// 
/// 

