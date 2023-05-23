#pragma once

// 2008년의 조상현 교수님이 제공해주신 Lexer
#include "ASEFile.h"

// 파서 내부에서 쓰는 데이터형
#include "CParsingDataClass.h"
#include "Animation.h"

#include <list>

using namespace std;
using namespace ASEParser;


/// <summary>
/// 2022년에 새로 만든 ASEParser
/// 
/// 2022.06.14 LeHideKGITPPR
/// </summary>
namespace ASE
{
	/// <summary>
	/// 기존의 재귀형태의 큰 함수 하나로 하는 방식에서
	/// 구조적인 여러 개의 단위별 함수로 처리하는 방식으로 바꿨다.
	/// 좀 더 알기 쉬운 코드를 목적으로 했다.
	/// 
	/// 2022.06.26 LeHideHome
	/// </summary>
	class ASEParser
	{
	public:
		ASEParser();
		~ASEParser();
		

		///--------------------------------------------------
		/// Load 관련
		/// 
		// ASE파일을 하나 읽어서, 대응하는 객체로 만든다.
		bool Load(LPSTR p_File);

		///--------------------------------------------------
		/// Parsing 관련
		/// 
	private:
		// 모든 내용을 파싱한다. ASE파일 스펙에 맞게 각 level별로 파싱한다.
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
		
		// 오브젝트의 Node 관련
		void Parsing_NodeTM(GeometryObject* targetObject);	// NodeTM을 넣도록 바뀌어야 한다.

		// TM Animation
		void Parsing_TMAnimation(GeometryObject* targetObject);
		void Parsing_TMAnimationData(Animation* animation);

		// 오브젝트별 개별 속성
		void Parsing_Mesh(GeometryObject* targetObject);
		void Parsing_Shape(GeometryObject* targetObject);
		void Parsing_CameraData(GeometryObject* targetObject);
		void Parsing_LightData(GeometryObject* targetObject);
		void Parsing_Misc(GeometryObject* targetObject);

		// 매트리얼 내용
		void Parsing_MaterialData(ASEMaterial* material);

		// 제작 중 더미
		void Parsing_Dummy();

		// 파싱에 사용되는 토큰을 읽어서 파싱한다.
	private:
		int Parsing_NumberLong();			// long을 읽어서 리턴해준다.
		float Parsing_NumberFloat();		// float
		LPSTR Parsing_String();				// string
		int Parsing_NumberInt();			// int
		Vector3 Parsing_NumberVector3();	// 3개의 Float를 벡터 하나로 읽어서 리턴해준다.(그대로)
		Vector3 Parsing_NumberVector3_YZ();	// 3개의 Float를 벡터 하나로 읽어서 리턴해준다.(YZ변환)



		///--------------------------------------------------
		/// 변환 관련
		
		// 씬 정보
		bool LoadSceneData(Scenedata* pscene);

	public:
		/// 1차 변환
		bool CopyMesh(GeometryObject* pMesh);				// 모두 그냥 변환
		bool OptimizeMesh(GeometryObject* pMesh);			// 버텍스(노말), 텍스쳐 값으로 최적화를 해 준다.
		bool OptimizeMesh2(GeometryObject* pMesh);

		void CalcTangentSpace(GeometryObject* pMesh);		// 탄젠트 스페이스를 계산, tangent값을 채워준다.

	private:
		char m_TokenString[256];				// 토큰을 구별해내기 위한 스트링
		int m_Token;

	private:
		/// 기본 정보
		int m_ASCIIExportSign;					// *3DSMAX_ASCIIEXPORT	200 - 파일의 시그니처
		std::string m_Comment;					// 버전과 만들어진 날짜 등의 코멘트

		/// 장면 정보
		Scenedata m_scenedata;					// 현재 Scene의 데이터가 들어갈 구조체

		/// 매트리얼 정보
		int	m_materialcount;					// Material이 총 몇 개인가?
		ASEMaterial* m_materialdata;			/// Material의 데이터가 들어갈 구조체
		list<ASEMaterial*> m_list_materialdata;	// Material들의 리스트

		/// 한 씬에 N개의 메시가 존재할 수 있다.
		GeometryObject* m_NowObject;			/// 오브젝트 하나가 들어갈 구조체
		vector<GeometryObject*> m_ASEObjectList;// ASE파일 내에 있는 렌더링 대상 오브젝트

		/// 쉐이프 오브젝트를 파싱할 때 필요한 중간 변수
		///( 파서 내부에서 쉐이프 오브젝트는 GeomObject와 동일하게 취급된다)
		ShapeLine* m_nowshapeline;				// 현재 읽고 있는 Line
		ShapeVertex* m_nowshapevertex;			// 현재 읽고 있는 ShapeVertex

		/// 애니메이션
		Animation* m_animation;					/// 한 오브젝트의 애니메이션 데이터(파싱용)
		list<Animation*> m_list_animation;		// 그 데이터의 리스트


		///--------------------------------------------------
		/// 내보내기 관련
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



		// 이제 새로 교체해야 할 렉서. 일단 이전 구현을 복구하고, 리팩토링하자.
	private:
		// 존경의 의미로 2008년의 조상현교수님의 lexer를 사용했다. 단, 오타 정도는 수정했다.
		ASE::CASELexer* m_lexer;

	};

}
