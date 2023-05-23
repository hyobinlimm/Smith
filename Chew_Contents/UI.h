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

/// UIManager���� UIData�� ����
/// ������ UIData�� �������� ������Ʈ�� ����
/// 
/// UIComponent���� ������ ������Ʈ���� ã�Ƽ� ���õ� ������Ʈ���� ���ͷ� ���� ������ ����
/// 
/// ������� ���ڱ� �� ����
///
/// ������ �ȿ� UIdata
/// 
///
/// UIRenderer�� �ִ� �����Ͷ�
/// �Ŵ����� �ִ� �����Ͷ� �����ƴ�?
/// �Ŵ������� �����Ѵٰ� ������ ���� �ȵ��ݾ�.
/// ������ �����ؾ� �Ǵ� �ֵ��� �־
/// 
/// �� ������ ������ �����ؾߵǰ�����
/// 
/// 

