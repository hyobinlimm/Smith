#pragma once

#include <vector>
#include <string>

/// <summary>
/// ��� ������Ʈ�� ����, ���� ���
/// </summary>

class HimeCamera;
class DXObjectBase;
class DXObjectFactory;	// ���߿��� �ٱ������� ������ Renderer�� ���� ������ �Ǿ�� �Ѵ�.

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

	// �ð� ����
private:
	float m_DeltaTime;
	float m_PrevTime;
	float m_TotalTime;

	// �ִϸ��̼��� � �������� ���� �������� ���� ����
	int m_FrameCount;

private:
	/// ������ ������ ���ؼ� �������� �� ����� DX������Ʈ��
	//std::vector<IDXObjectBase*> m_RenderingQueue;

	//ASEParser* m_pASEParser;
	// ani Frame�� �����ϱ� ���� ����
	int m_AniTick;

	// �θ� ã�Ҵ��� Ȯ���ϴ� ����
	//std::vector<bool> m_check;

	// parser���� �о�� mesh������Ʈ�� ����ִ� ����
	//std::vector<ASEObjects*> m_Cylinder;

private:

	//// �ִϸ��̼ǿ� �ʿ��� �Լ�
	//void IsAnimation(std::vector<ASEObjects*>& meshObject);
	//
	//void MakeAnimationTM(ASEObjects* meshObject); /// �ִϸ��̼� ��ǥ�� �޾ƿͼ� localTM�� ��������.
	//void MakeAnimation_POS(Animation* _nowAnimtion, ASEObjects* meshObject, int& _posIndex);
	//void MakeAnimation_ROT(Animation* _nowAnimtion, ASEObjects* meshObject, int& _rotIndex);
	//
	//// ��Ű�׿� �ʿ��� �Լ�
	//// ��Ű�� ������Ʈ���� �˻��ϴ� �Լ�
	//// todo:: �̰� �Ž�������Ʈ ������ �ִ°� ���� �� ���Ƽ� �����. 
	//void IsSkinningObject(ASEParser::GeomObject* meshData, ASEObjects* meshObject);
	//void MakeSkinnedNodeTM(std::vector<ASEObjects*>& meshObject);
	//void MakeBoneWorldTM(std::vector<ASEObjects*>& meshObject);

};

