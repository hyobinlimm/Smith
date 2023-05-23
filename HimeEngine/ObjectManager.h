#pragma once

#include <vector>
#include <string>

/// <summary>
/// 모든 오브젝트의 생성, 삭제 담당
/// </summary>

class HimeCamera;
class DXObjectBase;
class DXObjectFactory;	// 나중에는 바깥쪽으로 빠져서 Renderer와 형제 구조가 되어야 한다.

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

public:
	void Init();
	void Update(HimeCamera* pCamera, float deltaTime);
	void Render();
	void Delete();

	// 시간 관련
private:
	float m_DeltaTime;
	float m_PrevTime;
	float m_TotalTime;

	// 애니메이션의 어떤 프레임을 돌릴 것인지에 대한 변수
	int m_FrameCount;

private:
	/// 다형적 동작을 통해서 렌더링이 될 대상인 DX오브젝트들
	//std::vector<IDXObjectBase*> m_RenderingQueue;

	//ASEParser* m_pASEParser;
	// ani Frame를 결정하기 위한 변수
	int m_AniTick;

	// 부모를 찾았는지 확인하는 벡터
	//std::vector<bool> m_check;

	// parser에서 읽어온 mesh오브젝트를 들고있는 벡터
	//std::vector<ASEObjects*> m_Cylinder;

private:

	//// 애니메이션에 필요한 함수
	//void IsAnimation(std::vector<ASEObjects*>& meshObject);
	//
	//void MakeAnimationTM(ASEObjects* meshObject); /// 애니메이션 좌표를 받아와서 localTM에 대입하자.
	//void MakeAnimation_POS(Animation* _nowAnimtion, ASEObjects* meshObject, int& _posIndex);
	//void MakeAnimation_ROT(Animation* _nowAnimtion, ASEObjects* meshObject, int& _rotIndex);
	//
	//// 스키닝에 필요한 함수
	//// 스키닝 오브젝트인지 검사하는 함수
	//// todo:: 이거 매쉬오브젝트 안으로 넣는게 맞을 것 같아서 고민중. 
	//void IsSkinningObject(ASEParser::GeomObject* meshData, ASEObjects* meshObject);
	//void MakeSkinnedNodeTM(std::vector<ASEObjects*>& meshObject);
	//void MakeBoneWorldTM(std::vector<ASEObjects*>& meshObject);

};

