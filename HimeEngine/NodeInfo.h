#pragma once

#include <string>
#include <SimpleMath.h>
#include "CParsingDataClass.h"
#include "Animation.h"

/// <summary>
/// node�� ���� �������� ��� �ִ´�. 
/// 
/// �̰� �׳� struct�� �ٲ���. 
/// </summary>

/// ��� ���� ������ ��Ƽ� ��� - TODO:HIME
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
	
	/// nodeTM, localTM�� ���� ����ü
	NodeTransform m_NodeData;
	
	// �θ��� ���
	NodeInfo* m_pParentNodeInfo;

	// �ڽ��� ���
	NodeInfo* m_pChildNodeInfo;

	// �ִϸ��̼� ����
	Animation* m_pAnimation;
	vector<Animation> m_Animation_List;

	// �ִϸ��̼��� �ӽ÷� ����. 
	int m_LastFrame;
	int m_ticksperFrame;

	int m_type;

	bool m_IsSkinningObj;

	bool m_IsBone;

	// ���� ���� �׷��� �� �ΰ�? �ƴѰ�? 
	bool m_isDraw; 

	// bone ���� 
	vector<ASEParser::Bone*> m_BoneList;

};

