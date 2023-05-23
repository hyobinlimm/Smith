#pragma once

// 2008���� ������ �������� �������ֽ� Lexer
#include "ASEFile.h"

// �ļ� ���ο��� ���� ��������
#include "CParsingDataClass.h"
#include "Animation.h"

#include <list>

using namespace std;
using namespace ASEParser;


/// <summary>
/// 2022�⿡ ���� ���� ASEParser
/// 
/// 2022.06.14 LeHideKGITPPR
/// </summary>
namespace ASE
{
	/// <summary>
	/// ������ ��������� ū �Լ� �ϳ��� �ϴ� ��Ŀ���
	/// �������� ���� ���� ������ �Լ��� ó���ϴ� ������� �ٲ��.
	/// �� �� �˱� ���� �ڵ带 �������� �ߴ�.
	/// 
	/// 2022.06.26 LeHideHome
	/// </summary>
	class ASEParser
	{
	public:
		ASEParser();
		~ASEParser();
		

		///--------------------------------------------------
		/// Load ����
		/// 
		// ASE������ �ϳ� �о, �����ϴ� ��ü�� �����.
		bool Load(LPSTR p_File);

		///--------------------------------------------------
		/// Parsing ����
		/// 
	private:
		// ��� ������ �Ľ��Ѵ�. ASE���� ���忡 �°� �� level���� �Ľ��Ѵ�.
		void ParsingAll();

		///------------------------------
		/// Top level category

		// Scene
		void Parsing_Scene();

		// Objects
		void Parsing_GeometryObject();
		void Parsing_ShapeObject();
		void Parsing_CameraObject();
		void Parsing_LightObject();
		void Parsing_HelperObject();

		// Material
		void Parsing_Material(int materialIndex);

		///------------------------------
		/// Depth 1 category
		
		// ������Ʈ�� Node ����
		void Parsing_NodeTM(GeometryObject* targetObject);	// NodeTM�� �ֵ��� �ٲ��� �Ѵ�.

		// TM Animation
		void Parsing_TMAnimation(GeometryObject* targetObject);
		void Parsing_TMAnimationData(Animation* animation);

		// ������Ʈ�� ���� �Ӽ�
		void Parsing_Mesh(GeometryObject* targetObject);
		void Parsing_Shape(GeometryObject* targetObject);
		void Parsing_CameraData(GeometryObject* targetObject);
		void Parsing_LightData(GeometryObject* targetObject);
		void Parsing_Misc(GeometryObject* targetObject);

		// ��Ʈ���� ����
		void Parsing_MaterialData(ASEMaterial* material);

		// ���� �� ����
		void Parsing_Dummy();

		// �Ľ̿� ���Ǵ� ��ū�� �о �Ľ��Ѵ�.
	private:
		int Parsing_NumberLong();			// long�� �о �������ش�.
		float Parsing_NumberFloat();		// float
		LPSTR Parsing_String();				// string
		int Parsing_NumberInt();			// int
		Vector3 Parsing_NumberVector3();	// 3���� Float�� ���� �ϳ��� �о �������ش�.(�״��)
		Vector3 Parsing_NumberVector3_YZ();	// 3���� Float�� ���� �ϳ��� �о �������ش�.(YZ��ȯ)



		///--------------------------------------------------
		/// ��ȯ ����
		
		// �� ����
		bool LoadSceneData(Scenedata* pscene);

	public:
		/// 1�� ��ȯ
		bool CopyMesh(GeometryObject* pMesh);				// ��� �׳� ��ȯ
		bool OptimizeMesh(GeometryObject* pMesh);			// ���ؽ�(�븻), �ؽ��� ������ ����ȭ�� �� �ش�.
		bool OptimizeMesh2(GeometryObject* pMesh);

		void CalcTangentSpace(GeometryObject* pMesh);		// ź��Ʈ �����̽��� ���, tangent���� ä���ش�.

	private:
		char m_TokenString[256];				// ��ū�� �����س��� ���� ��Ʈ��
		int m_Token;

	private:
		/// �⺻ ����
		int m_ASCIIExportSign;					// *3DSMAX_ASCIIEXPORT	200 - ������ �ñ״�ó
		std::string m_Comment;					// ������ ������� ��¥ ���� �ڸ�Ʈ

		/// ��� ����
		Scenedata m_scenedata;					// ���� Scene�� �����Ͱ� �� ����ü

		/// ��Ʈ���� ����
		int	m_materialcount;					// Material�� �� �� ���ΰ�?
		ASEMaterial* m_materialdata;			/// Material�� �����Ͱ� �� ����ü
		list<ASEMaterial*> m_list_materialdata;	// Material���� ����Ʈ

		/// �� ���� N���� �޽ð� ������ �� �ִ�.
		GeometryObject* m_NowObject;			/// ������Ʈ �ϳ��� �� ����ü
		vector<GeometryObject*> m_ASEObjectList;// ASE���� ���� �ִ� ������ ��� ������Ʈ

		/// ������ ������Ʈ�� �Ľ��� �� �ʿ��� �߰� ����
		///( �ļ� ���ο��� ������ ������Ʈ�� GeomObject�� �����ϰ� ��޵ȴ�)
		ShapeLine* m_nowshapeline;				// ���� �а� �ִ� Line
		ShapeVertex* m_nowshapevertex;			// ���� �а� �ִ� ShapeVertex

		/// �ִϸ��̼�
		Animation* m_animation;					/// �� ������Ʈ�� �ִϸ��̼� ������(�Ľ̿�)
		list<Animation*> m_list_animation;		// �� �������� ����Ʈ


		///--------------------------------------------------
		/// �������� ����
	public:
		GeometryObject* GetMesh(int index);
		std::vector<GeometryObject*>& GetMeshList()  { return m_ASEObjectList; }
		std::list<Animation*> GetList_animation() const { return m_list_animation; }
		ASEMaterial* GetMaterial() { return m_list_materialdata.front(); }
		list<ASEMaterial*> GetMaterialList() { return m_list_materialdata; }
		ASEParser::Scenedata GetScenedata() const { return m_scenedata; }
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////



		// ���� ���� ��ü�ؾ� �� ����. �ϴ� ���� ������ �����ϰ�, �����丵����.
	private:
		// ������ �ǹ̷� 2008���� �������������� lexer�� ����ߴ�. ��, ��Ÿ ������ �����ߴ�.
		ASE::CASELexer* m_lexer;

	};

}
