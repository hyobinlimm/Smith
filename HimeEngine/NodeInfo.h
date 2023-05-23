#pragma once

#include <string>
#include <SimpleMath.h>
#include "CParsingDataClass.h"
#include "Animation.h"

/// <summary>
/// node에 대한 정보들을 들고 있는다. 
/// 
/// 이거 그냥 struct로 바꾸자. 
/// </summary>

/// 노드 관련 정보를 담아서 운반 - TODO:HIME
struct NodeTransform
{
	XMFLOAT4X4 m_NodeTM;
	XMFLOAT4X4 m_LocalTM;
	XMFLOAT4X4 m_DirectionTM;
};

using namespace DirectX;
using namespace SimpleMath;

class NodeInfo
{
public:
	virtual ~NodeInfo() {};
public:
	std::wstring m_ID;
	std::wstring m_NodeName;
	std::wstring m_ParentName;
	std::string m_stringName; 
	
	/// nodeTM, localTM를 담은 구조체
	NodeTransform m_NodeData;
	
	// 부모의 노드
	NodeInfo* m_pParentNodeInfo;

	// 자식의 노드
	NodeInfo* m_pChildNodeInfo;

	// 애니메이션 정보
	Animation* m_pAnimation;
	vector<Animation> m_Animation_List;

	// 애니메이션의 임시로 넣음. 
	int m_LastFrame;
	int m_ticksperFrame;

	int m_type;

	bool m_IsSkinningObj;

	bool m_IsBone;

	// 현재 내가 그려질 것 인가? 아닌가? 
	bool m_isDraw; 

	// bone 정보 
	vector<ASEParser::Bone*> m_BoneList;

};

