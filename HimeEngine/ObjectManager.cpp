#include "ObjectManager.h"
#include "HimeCamera.h"

/// 여기 둘 필요가 없을 것 같은데 우선 화면에 띄우기 위해 임의로 넣자. 
#include "HimeDevice.h"
#include "IDXObjectBase.h"
#include "DXObjectFactory.h"


ObjectManager::ObjectManager()
	: m_DeltaTime(0.0f), m_PrevTime(0.0f), m_TotalTime(0.0f),
	m_FrameCount(0), 
	m_AniTick(0)
{
	
}

ObjectManager::~ObjectManager()
{
	//SafeDelete(m_pASEParser);
}

void ObjectManager::Init()
{
	HimeDevice* _direct3D = HimeDevice::GetInst();

	/* // m_IKJoe가 들고있는 GeomObject의 부모-자식 관계를 만들어주자. 
	MakeHierarchy(m_Cylinder);

	// m_IKJoe가 자신의 애니메이션을 들고 있게 하자. 
	IsAnimation(m_Cylinder);

	// LocalTM의 초기값을 NodeTM들로부터 만들어 준다.
	MakeLocalTM(m_Cylinder);

	// 오브젝트 들에서 본의 nodeTM과 worldTM를 받아서 넣어주자. 
	MakeSkinnedNodeTM(m_Cylinder);*/

}

void ObjectManager::Update(HimeCamera* pCamera, float deltaTime)
{
	m_DeltaTime = deltaTime;
	m_TotalTime += m_DeltaTime;

/*
	for (size_t i = 0; i < m_Cylinder.size(); i++)
	{
		MakeAnimationTM(m_Cylinder[i]);
	}

	MakeBoneWorldTM(m_Cylinder);

	for (size_t i = 0; i < m_Cylinder.size(); i++)
	{
		m_Cylinder[i]->Update(pCamera);
	}

	// animation의 size만큼 한번 돌았다면 framecout++ 해주자.
	float tmepTime = m_TotalTime - m_PrevTime;
	if (0.01f <= tmepTime) // dtime이 0.03 커졌다면
	{
		m_FrameCount++;

		m_PrevTime = m_TotalTime;
	}

	/// ASE의 의도는 맞는데, 데이터가 정확히 이렇게 맞는다는 보장이 없다.
	if (m_FrameCount >= (m_pASEParser->m_scenedata.m_lastframe - m_pASEParser->m_scenedata.m_firstframe))
	{
		m_FrameCount = 0;
	}*/
}

void ObjectManager::Render()
{
	//m_GridObject->Render();
	//for (auto DXObjects : m_RenderingQueue)
	//{
	//	DXObjects->Render();
	//}

	/*for (size_t i = 0; i < m_Cylinder.size(); i++)
	{
		// SkinningObject 오브젝트만 그리도록 하자. 
		if (m_Cylinder[i]->m_IsSkinningObject == true)
		{
			m_Cylinder[i]->Render();
		}
	}*/
}

void ObjectManager::Delete()
{
/*
	for (size_t i = 0; i < m_Cylinder.size(); i++)
	{
		delete	m_Cylinder[i];
	}
	m_Cylinder.clear();*/
}

/*
void ObjectManager::MakeHierarchy(std::vector<ASEObjects*>& meshObject)
{
	/// <summary>
	/// 벡터에 들어있는 GeomObject들의 부모-자식 관계를 만들자.
	/// </summary>
	/// <param name="meshObject">GeomObject</param>

	bool _isParentFounded = false;

	//	1. for each를 사용해 이중포문을 돌면서,
	//		나의 m_ParentNodeName과 같은 targetmesh의 mNodeName 있는지 찾는다.
	for (size_t i = 0; i < meshObject.size(); i++)
	{
		_isParentFounded = false;

		for (size_t j = 0; j < meshObject.size(); j++)
		{
			//	1-1. 나의 mNodeName과 targetmesh의 mNodeName 같다면 무시한다. 
			if (meshObject[i]->mNodeName == meshObject[j]->mNodeName)
			{
				meshObject[i]->m_ParentNode = nullptr;
				continue;
			}
			//	2. 찾았다면 나의  m_ParentNode 에 GeomObject의 주소를 넣어준다. 
			if (meshObject[i]->mParentNodeName == meshObject[j]->mNodeName)
			{
				meshObject[i]->m_ParentNode = meshObject[j];
				m_check.push_back(true);

				_isParentFounded = true;
				break;
			}
		}

		/// 부모의 이름이 있는데, 위에서 결국 부모를 못찾은 경우
		// 부모의 이름이 있는데
		if (meshObject[i]->mParentNodeName.size() != 0)
		{
			// 이러면 뭐가 문제가 있어서 결국 부모를 못찾은 것이다.
			if (_isParentFounded == false)
			{
				int kk = 0;
			}
		}
	}
}

/// <summary>
/// LocalTM의 초기값을 NodeTM들로부터 만들어 준다.
/// (나의 NodeTM과 부모의 NodeTM의 역행렬을 이용해서)
/// </summary>
void ObjectManager::MakeLocalTM(std::vector<ASEObjects*>& meshObject)
{
	for (size_t i = 0; i < meshObject.size(); i++)
	{
		// 부모가 없으면 나의 nodeTM를 localTM에 넣어주자. 
		if (meshObject[i]->m_ParentNode == nullptr)
		{
			meshObject[i]->mLocalTM = meshObject[i]->mNodeTM;

			// decompose LocalTM 
			meshObject[i]->DecomposeLocalTM();

			continue;
		}

		// _nowMesh의 부모 NodeTM의 역행렬을 얻어오자. 
		XMMATRIX _ParentNodeTM = XMLoadFloat4x4(&meshObject[i]->m_ParentNode->mNodeTM);
		XMMATRIX _parentInverseNodeTM = XMMatrixInverse(nullptr, _ParentNodeTM);

		// _nowMesh->LocalTM 를 구하자. 
		// LocalTM = NodeTM * Parent’sNodeTM −1
		meshObject[i]->mLocalTM = meshObject[i]->mNodeTM * _parentInverseNodeTM;

		//meshObject[i]->mNodeTM = meshObject[i]->mLocalTM * _ParentNodeTM;

		// decompose LocalTM 
		meshObject[i]->DecomposeLocalTM();
	}
}

void ObjectManager::IsAnimation(std::vector<ASEObjects*>& meshObject)
{
	for (auto _animation : m_pASEParser->m_list_animation)
	{
		for (auto _object : meshObject)
		{
			if (_object->mNodeName == _animation->m_nodename)
			{
				_object->m_Animation = _animation;
			}
		}
	}
}

void ObjectManager::MakeAnimationTM(ASEObjects* meshObject)
{
	/// 1. 일단 내 애니메이션을 찾는다.
	// 내가 애니메이션을 들고 있지 않다면 함수를 나오자. 
	if (meshObject->m_Animation == nullptr) return;

	Animation* _nowAnimtion = meshObject->m_Animation;

	/// 2. 애니메이션의 모든 기준은 Tick이다.
	// Animation의 Tickcount를 나눈값(parser가 들고있음)
	int _AniTick = m_FrameCount * m_pASEParser->m_scenedata.m_ticksperframe;

	// 애니메이션이 있지만, 포지션 데이터가 없는 경우도 그냥 나온다.
	if (_nowAnimtion->m_position.size() != 0)
	{
		/// 3. Tick을 기준으로 좌/우 값을 찾는다.
		int _leftIndex = 0;
		int _rightIndex = 0;

		for (size_t i = 0; i < _nowAnimtion->m_position.size(); i++)
		{
			if (_AniTick < _nowAnimtion->m_position[i]->m_time)
			{
				if (i == 0 && _nowAnimtion->m_position[0]->m_time != 0) // 애니메이션의 time이 0에서 부터 시작하는게 아닐 때 예외처리
				{
					_leftIndex = _nowAnimtion->m_position.size() - 1;
					_rightIndex = 0;
				}
				else
				{
					_leftIndex = i - 1;
					_rightIndex = i;
				}
				break;
			}

			if (i == _nowAnimtion->m_position.size() - 1)
			{
				_leftIndex = i;
				_rightIndex = 0;
			}
		}

		/// 4. 좌/우 인덱스를 기준으로 보간한다.
		// 현재와 다음 pos 구조체를 담는 변수
		auto _leftPos = _nowAnimtion->m_position[_leftIndex];
		auto _rightPos = _nowAnimtion->m_position[_rightIndex];

		// 현재와 다음 pos 사이의 시간이 얼마나 흘렀나를 담는 변수(0 ~ 1 사이 값으로 나타냄)
		float _NowTime = (float)(_leftPos->m_time);
		float _NextTime = (float)(_rightPos->m_time);

		float _floatTime = (_AniTick - _NowTime) / (_NextTime - _NowTime);

		meshObject->m_Position = XMVectorLerp(_leftPos->m_pos, _rightPos->m_pos, _floatTime);
	}

	// 애니메이션이 있지만, 회전 데이터가 없는 경우도 그냥 나온다.
	if (_nowAnimtion->m_rotation.size() != 0)
	{
		/// 3. Tick을 기준으로 좌/우 값을 찾는다.
		int _leftIndex = 0;
		int _rightIndex = 0;

		for (size_t i = 0; i < _nowAnimtion->m_rotation.size(); i++)
		{
			if (_AniTick < _nowAnimtion->m_rotation[i]->m_time)
			{
				if (i == 0 && _nowAnimtion->m_rotation[0]->m_time != 0) // 애니메이션의 time이 0에서 부터 시작하는게 아닐 때 예외처리
				{
					_leftIndex = _nowAnimtion->m_rotation.size() - 1;
					_rightIndex = 0;
				}
				else
				{
					int _num0 = i - 1;
					_leftIndex = max(0, _num0);
					_rightIndex = i;
				}
				break;
			}

			if (i == _nowAnimtion->m_rotation.size() - 1)
			{
				_leftIndex = i;
				_rightIndex = 0;
			}
		}

		/// 4. 좌/우 인덱스를 기준으로 보간한다.
		// 현재와 다음 rot 구조체를 담는 변수
		auto _leftRot = _nowAnimtion->m_rotation[_leftIndex];
		auto _rightRot = _nowAnimtion->m_rotation[_rightIndex];

		// 현재와 다음 rot 사이의 시간이 얼마나 흘렀나를 담는 변수(0 ~ 1 사이 값으로 나타냄)
		float _NowTime = (float)(_leftRot->m_time);
		float _NextTime = (float)(_rightRot->m_time);

		float _floatTime = (_AniTick - _NowTime) / (_NextTime - _NowTime);

		meshObject->m_Rotate = XMQuaternionSlerp(_leftRot->m_rotQT_accumulation, _rightRot->m_rotQT_accumulation, _floatTime);
	}
}

void ObjectManager::MakeAnimation_POS(Animation* _nowAnimtion, ASEObjects* meshObject, int& _posIndex)
{
	if (_nowAnimtion->m_position.size() != 0)
	{
		// keyframe를 담은 변수 
		int _nowPoskeyFrame = _nowAnimtion->m_position[_posIndex]->m_time;

		// ani_pos를 mesh에 담는다. 
		meshObject->m_Position = _nowAnimtion->m_position[_posIndex]->m_pos;

		/// 시간의 크기를 비교하여 어떤 _pos, _rot를 대입해야 하는지 알아낸다.
		if (m_AniTick > _nowPoskeyFrame)
		{
			_posIndex++;

			// index값이 사이즈보다 커지면 애니메이션을 다 돌았다고 생각함. 
			if (_nowAnimtion->m_position.size() - 1 == _posIndex)
			{
				// 그래서 index를 처음으로 다시 돌림. 
				_posIndex = 0;
			}
			// idnex가 변했다면 mesh에 바뀐 정보를 넣어주자. 
			meshObject->m_Position = _nowAnimtion->m_position[_posIndex]->m_pos;
		}
	}
}

void ObjectManager::MakeAnimation_ROT(Animation* _nowAnimtion, ASEObjects* meshObject, int& _rotIndex)
{
	if (_nowAnimtion->m_rotation.size() != 0)
	{
		// keyframe를 담은 변수 
		int _nowRotkeyFrame = _nowAnimtion->m_rotation[_rotIndex]->m_time;

		// ani_pos를 mesh에 담는다. 
		meshObject->m_Rotate = _nowAnimtion->m_rotation[_rotIndex]->m_rotQT_accumulation;

		/// 시간의 크기를 비교하여 어떤 _pos, _rot를 대입해야 하는지 알아낸다.
		if (_nowRotkeyFrame < m_AniTick)
		{
			_rotIndex++;

			// index값이 사이즈보다 커지면 애니메이션을 다 돌았다고 생각함. 
			if (_nowAnimtion->m_rotation.size() - 1 == _rotIndex)
			{
				// 그래서 index를 처음으로 다시 돌림. 
				_rotIndex = 0;
			}
			// idnex가 변했다면 mesh에 바뀐 정보를 넣어주자. 
			meshObject->m_Rotate = _nowAnimtion->m_rotation[_rotIndex]->m_rotQT_accumulation;
		}
	}
}

void ObjectManager::IsSkinningObject(ASEParser::GeomObject* meshData, ASEObjects* meshObject)
{
	// 이중포문을 돌면서 bone의 이름과 같은 geomObject가 있다면 
	// skinning object라고 판단한다.
	auto _boneList = meshData->m_vector_bone_list;

	if (_boneList.size() != 0)
	{
		if (meshData->m_nodename == meshObject->mNodeName)
		{
			meshObject->m_IsSkinningObject = true;
			meshObject->m_BoneList = _boneList;
		}
	}
}

void ObjectManager::MakeSkinnedNodeTM(std::vector<ASEObjects*>& meshObject)
{
	std::vector<ASEParser::Bone*> _Bones;

	// skin오브젝트의 본 리스트를 저장
	for (ASEObjects* _object : meshObject)
	{
		if (_object->m_IsSkinningObject == true)
		{
			_Bones = _object->m_BoneList;

			break;
		}
	}

	// 본 리스트의 이름과 오브젝트의 이름을 찾아서 nodeTM를 넣어주자. 
	for (Bone* _bone : _Bones)
	{
		for (ASEObjects* _object : meshObject)
		{
			if (_bone->m_bone_name == _object->mNodeName)
			{
				// 첫 nodeTM를 넣어줌. 
				Matrix* _temp1 = new Matrix;
				*_temp1 = _object->mNodeTM;
				_bone->m_boneTM_NodeTM = _temp1;
				_bone->m_boneTM_NodeTM = reinterpret_cast<Matrix*>(&_object->mNodeTM);
			}
		}
	}
}

void ObjectManager::MakeBoneWorldTM(vector<ASEObjects*>& meshObject)
{
	std::vector<ASEParser::Bone*> _Bones;

	// skin오브젝트의 본 리스트를 저장
	for (ASEObjects* _object : meshObject)
	{
		if (_object->m_IsSkinningObject == true)
		{
			_Bones = _object->m_BoneList;

			break;
		}
	}

	// 본 리스트의 이름과 오브젝트의 이름을 찾아서 nodeTM를 넣어주자. 
	for (Bone* _bone : _Bones)
	{
		for (ASEObjects* _object : meshObject)
		{
			if (_bone->m_bone_name == _object->mNodeName)
			{
				// boneObject가 애니메이션 되면서 TM이 바뀌기 때문에 업데이트 될때마다 받아와야함.
				Matrix* _temp2 = new Matrix;
				*_temp2 = _object->GetWorld();
				_bone->m_boneTM_WorldTM = _temp2;

			}
		}
	}
}
*/
//void ObjectManager::CreateDXObject(std::string name)
//{
//	IDXObjectBase* _newObject = m_Factory->CreateDXObject(name, 0);
//
//	m_RenderingQueue.emplace_back(_newObject);
//}
//
//void ObjectManager::CreateDXObjects(std::string name)
//{
//	std::vector<IDXObjectBase*> _DXObjects = m_Factory->CreateDXObjects(name, 0);
//
//	//m_RenderingQueue.emplace_back(_newObject);
//}

