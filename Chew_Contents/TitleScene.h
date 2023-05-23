#pragma once
#include <Scene.h>


class GameObject;
struct UIData;
struct TextData;
class SceneManager;
class TitleScene :
	public Scene
{

public:
	TitleScene(wstring sceneName);
	virtual ~TitleScene() {};
public:
	virtual void Awake() override;

	virtual void Start() override;

	virtual void Update() override;

	void KeyUpdate();

	virtual void Render() override;

	virtual void LoadObject(vector<MeshBuildInfo*> MeshInfoList, shared_ptr<ObjectDataTable> ObjData) override;


private:
	bool m_IsCreditOn= false;
	shared_ptr<SceneManager> m_Scene;
protected:
	virtual void CreateUI() override;


};

