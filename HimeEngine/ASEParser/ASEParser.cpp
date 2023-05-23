#include "ASEParser.h"

// DX Object
///#include "../DRDX11Renderer/DXObject/IDXObjectBase.h"
///#include "../DRDX11Renderer/DXObject/MeshObject.h"


ASE::ASEParser::ASEParser()
	: m_lexer(nullptr)
{
	m_lexer = new ASE::CASELexer();
}

ASE::ASEParser::~ASEParser()
{
	delete m_lexer;
}

bool ASE::ASEParser::Load(LPSTR p_File)
{
	// 0) 파일을 로드한다.
	if (!m_lexer->Open(p_File))
	{
		TRACE("파일을 여는 중에 문제가 발생했습니다!");
		return false;
	}

	// 1) 개별 요소를 모두 파싱한다.
	ParsingAll();

	return true;
}

bool ASE::ASEParser::LoadSceneData(Scenedata* pScene)
{
	// 값 복사
	*pScene = m_scenedata;
	return true;
}

/// <summary>
/// 메시를 모두 그대로 복사한다.
/// 
/// Face기준으로 모든 버텍스를 각자 다르다고 가정하고 모두(!) 추가한다.
/// 버텍스가 엄청 많아질 것이다.
/// </summary>
bool ASE::ASEParser::CopyMesh(GeometryObject* pMesh)
{
	// 버텍스들은 일단 모두 복사
	for (unsigned int i = 0; i < pMesh->m_meshvertex.size(); i++)
	{
		Vertex* _pVertex = new Vertex;
		_pVertex->m_pos = pMesh->m_meshvertex[i]->m_pos;

		pMesh->m_opt_vertex.push_back(_pVertex);
	}

	// 나머지는 face를 기준으로 한 인덱스로 찾아야 함
	unsigned int _faceCount = pMesh->m_meshface.size();
	TRACE("FaceCount : %d\n", _faceCount);

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		TRACE("FaceNum : %d / %d\n", i, _faceCount);

		for (int j = 0; j < 3; j++)
		{
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// face기준으로 버텍스의 노말을 넣어준다.
			_nowVertex->m_normal = pMesh->m_meshface[i]->m_normalvertex[j];

			if (pMesh->m_mesh_tvertex.size() != 0)
			{
				_nowVertex->u = pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_u;
				_nowVertex->v = pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_v;
			}
		}
	}

	// 인덱스는 그냥 복사
	pMesh->m_opt_index = new IndexList[pMesh->m_meshface.size()];

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pMesh->m_opt_index[i].index[j] = pMesh->m_meshface[i]->m_vertexindex[j];
		}
	}

	return false;
}

/// <summary>
/// 메시의 최적화를 해 준다.
/// 노말 값, 텍스쳐 좌표에 따라 버텍스를 늘리고, 중첩되는것은 제거하고..
///
/// Face기준으로 모든 버텍스를 각자 다르다고 가정하고 만들고, 이전의 버텍스와 중복되는 것이 있는지 체크 후 추가한다.
/// 최악의 경우 O(N^2)이다.
/// </summary>
bool ASE::ASEParser::OptimizeMesh(GeometryObject* pMesh)
{
	// 이것이 메시가 아니라면 리턴
	if (!(pMesh->m_type == eASEObjectType::Geometry || pMesh->m_type == eASEObjectType::SkinnedMesh))
	{
		return false;
	}

	unsigned int _faceCount = pMesh->m_meshface.size();
	//TRACE("FaceCount : %d", _faceCount);

	pMesh->m_opt_index = new IndexList[pMesh->m_meshface.size()];

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		for (int j = 0; j < 3; j++)
		{
			// 1. Vertex를 조합한다.
			// 단, 데이터가 없으면 조합하지 않는다.
			Vertex* _pVertex;
			_pVertex = new Vertex;

			if (pMesh->m_meshvertex.size() != 0)
			{
				_pVertex->m_pos = pMesh->m_meshvertex[pMesh->m_meshface[i]->m_vertexindex[j]]->m_pos;

				if (pMesh->m_type == eASEObjectType::SkinnedMesh)
				{
					_pVertex->m_bw1 = pMesh->m_meshvertex[pMesh->m_meshface[i]->m_vertexindex[j]]->m_bw1;
					_pVertex->m_bw2 = pMesh->m_meshvertex[pMesh->m_meshface[i]->m_vertexindex[j]]->m_bw2;
					_pVertex->m_bw3 = pMesh->m_meshvertex[pMesh->m_meshface[i]->m_vertexindex[j]]->m_bw3;
					_pVertex->m_weightIndices[0] = pMesh->m_meshvertex[pMesh->m_meshface[i]->m_vertexindex[j]]->m_weightIndices[0];
					_pVertex->m_weightIndices[1] = pMesh->m_meshvertex[pMesh->m_meshface[i]->m_vertexindex[j]]->m_weightIndices[1];
					_pVertex->m_weightIndices[2] = pMesh->m_meshvertex[pMesh->m_meshface[i]->m_vertexindex[j]]->m_weightIndices[2];
					_pVertex->m_weightIndices[3] = pMesh->m_meshvertex[pMesh->m_meshface[i]->m_vertexindex[j]]->m_weightIndices[3];
				}
			}

			_pVertex->m_normal = pMesh->m_meshface[i]->m_normalvertex[j];

			if (pMesh->m_mesh_tvertex.size() != 0)
			{
				_pVertex->u = pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_u;
				_pVertex->v = pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_v;
			}

			// 2. 지금까지 조합된 Vertex중에 같은 Vertex가 있는지 검색한다.
			int index = 0;
			for (unsigned int vlistindex = 0; vlistindex < pMesh->m_opt_vertex.size(); vlistindex++, index++)
			{
				// 같은 버텍스가 있으면 지금 조합된것을 삭제하고 루프를 빠져나감.
				if (pMesh->m_opt_vertex[index]->m_pos == (_pVertex)->m_pos &&
					pMesh->m_opt_vertex[index]->m_normal == (_pVertex)->m_normal &&
					pMesh->m_opt_vertex[index]->u == (_pVertex)->u &&
					pMesh->m_opt_vertex[index]->v == (_pVertex)->v)
				{
					TRACE("같은 버텍스 삭제 : %d\n", index);
					delete _pVertex;
					_pVertex = NULL;
					break;
				}
			}

			// vertex가 삭제되지 않았다면 같은 vertex가 없다는 것이므로
			// 리스트에 추가한 뒤, 인덱스를 넣어준다 번호는 index (0부터 시작하기 때문, 위의 for문에서 ++이 되기 때문)
			if (_pVertex != NULL)
			{
				pMesh->m_opt_vertex.push_back(_pVertex);
				pMesh->m_opt_index[i].index[j] = index;
			}
			// vertex가 삭제되었을 때 -> 같은 Vertex가 있다는 것. 그에 해당하는 인덱스도 index다.
			else
			{
				pMesh->m_opt_index[i].index[j] = index;

			}
		}
	}

	pMesh->m_mesh_numfaces = pMesh->m_meshface.size();

	return false;
}

bool ASE::ASEParser::OptimizeMesh2(GeometryObject* pMesh)
{
	// 2021.04.12
	// 버텍스들 복사 (노말값은 아직 복사하지 않음.)
	for (unsigned int i = 0; i < pMesh->m_meshvertex.size(); i++)
	{
		Vertex* _pVertex = new Vertex;
		_pVertex->m_pos = pMesh->m_meshvertex[i]->m_pos;
		_pVertex->m_indices = pMesh->m_meshvertex[i]->m_indices;

		pMesh->m_opt_vertex.push_back(_pVertex);
	}

	// 나머지는 face를 기준으로 한 인덱스로 찾아야 함
	unsigned int _faceCount = pMesh->m_meshface.size();
	TRACE("FaceCount : %d", _faceCount);

	/// 스플릿된 버텍스의 번호 부여 및 개수를 측정하기 위함.
	unsigned int split_Vertex_Num = 0 + pMesh->m_meshvertex.size();
	/// 각 페이스에 할당되어 있는 버텍스 노말값들을 비교하며 버텍스에 노말값을 채워준다. 
	/// 만약 버텍스에 노말값이 두개이상 들어가는 경우라면 스플릿을 해준다.
	for (unsigned int i = 0; i < _faceCount; i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		for (int j = 0; j < 3; j++)
		{
			int Vertex_num = pMesh->m_meshface[i]->m_vertexindex[j];

			Vertex* _nowVertex = pMesh->m_opt_vertex[Vertex_num];

			if (_nowVertex->m_isnormalset)
			{
				if (pMesh->m_opt_vertex[Vertex_num]->m_normal != pMesh->m_meshface[i]->m_normalvertex[j])
				{
					bool Is_Data_Exist = false;

					if (split_Vertex_Num > pMesh->m_meshvertex.size())
					{
						for (unsigned int k = pMesh->m_meshvertex.size(); k < split_Vertex_Num; k++)
						{
							/// 만약 서로다른 버텍스 노말값으로 인해 생성된 데이터들 중에서도 또 같은값이 중복하여 있다면 생성을 해줄 필요가 없다.
							if ((pMesh->m_opt_vertex[k]->m_indices == pMesh->m_meshface[i]->m_vertexindex[j]))
							{
								/// 노말값이 다를 때 새로 생성
								if ((pMesh->m_opt_vertex[k]->m_normal == pMesh->m_meshface[i]->m_normalvertex[j]))
								{
									pMesh->m_meshface[i]->m_vertexindex[j] = k;
									Is_Data_Exist = true;
									break;
								}

							}
						}
					}

					/// 만약 이전에 생성되었던 버텍스가 없다면
					/// 현재 버텍스는 새로운 버텍스로 지정한다.
					if (Is_Data_Exist == false)
					{

						Vertex* _pVertex = new Vertex;
						_pVertex->m_pos = _nowVertex->m_pos;
						_pVertex->m_indices = _nowVertex->m_indices;
						_pVertex->m_normal = pMesh->m_meshface[i]->m_normalvertex[j];
						_pVertex->m_isnormalset = true;

						if (pMesh->m_mesh_numtvertex)
						{
							_pVertex->u = pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_u;
							_pVertex->v = pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_v;
							_pVertex->m_istextureset = true;
						}

						pMesh->m_opt_vertex.push_back(_pVertex);
						// 매시에 맵핑되는 버텍스 번호도 바꿔줘야 하겠지?!
						pMesh->m_meshface[i]->m_vertexindex[j] = split_Vertex_Num;
						split_Vertex_Num++;

						continue;
					}
				}
			}

			/// 만약 텍스쳐가 셋팅 되어있더라도 uv값이 다를 때 새로 생성
			if (_nowVertex->m_istextureset)
			{
				if ((_nowVertex->u != pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_u)
					|| (_nowVertex->v != pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_v))
				{
					Vertex* _pVertex = new Vertex;
					_pVertex->m_pos = _nowVertex->m_pos;
					_pVertex->m_indices = _nowVertex->m_indices;
					_pVertex->m_normal = pMesh->m_meshface[i]->m_normalvertex[j];
					_pVertex->m_isnormalset = true;
					_pVertex->u = pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_u;
					_pVertex->v = pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_v;
					_pVertex->m_istextureset = true;
					pMesh->m_opt_vertex.push_back(_pVertex);

					// 매시에 맵핑되는 버텍스 번호도 바꿔줘야 하겠지?!
					pMesh->m_meshface[i]->m_vertexindex[j] = split_Vertex_Num;
					split_Vertex_Num++;

					continue;
				}
			}

			if (!_nowVertex->m_isnormalset)
			{
				_nowVertex->m_normal = pMesh->m_meshface[i]->m_normalvertex[j];
				_nowVertex->m_isnormalset = true;
			}
			if (pMesh->m_mesh_numtvertex && !_nowVertex->m_istextureset)
			{
				_nowVertex->u = pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_u;
				_nowVertex->v = pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_v;
				_nowVertex->m_istextureset = true;
			}
		}
	}

	// 인덱스는 그냥 복사
	pMesh->m_opt_index = new IndexList[pMesh->m_meshface.size()];

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pMesh->m_opt_index[i].index[j] = pMesh->m_meshface[i]->m_vertexindex[j];
		}
	}

	return FALSE;
}

void ASE::ASEParser::CalcTangentSpace(GeometryObject* pMesh)
{
	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[0]]->m_tangent = Vector3::Zero;
		pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[1]]->m_tangent = Vector3::Zero;
		pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[2]]->m_tangent = Vector3::Zero;
	}

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		const Vector3& p0 = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[0]]->m_pos;
		const Vector3& p1 = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[1]]->m_pos;
		const Vector3& p2 = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[2]]->m_pos;

		Vector2 w0(pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[0]]->u,
			pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[0]]->v);

		Vector2 w1(pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[1]]->u,
			pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[1]]->v);

		Vector2 w2(pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[2]]->u,
			pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[2]]->v);

		Vector3 e1 = p1 - p0;
		Vector3 e2 = p2 - p0;
		float x1 = w1.x - w0.x;
		float x2 = w2.x - w0.x;
		float y1 = w1.y - w0.y;
		float y2 = w2.y - w0.y;

		float r = 1.0f / (x1 * y2 - x2 * y1);
		Vector3 t = (e1 * y2 - e2 * y1) * r;
		Vector3 b = (e2 * x1 - e1 * x2) * r;

		pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[0]]->m_tangent += t;
		pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[1]]->m_tangent += t;
		pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[2]]->m_tangent += t;
	}

	for (unsigned int i = 0; i < pMesh->m_opt_vertex.size(); i++)
	{
		pMesh->m_opt_vertex[i]->m_tangent.Normalize();
	}
}

ASEParser::GeometryObject* ASE::ASEParser::GetMesh(int index)
{
	return m_ASEObjectList[index];
}

void ASE::ASEParser::ParsingAll()
{
	// Lexer에서 한 개씩 토큰을 가져온다.
	LONG _nowToken;

	// 토큰을 한 개씩 읽으면서, 적절한 처리를 해 준다.
	while (_nowToken = m_lexer->GetToken(m_TokenString), _nowToken != TOKEND_END)
	{
		// 최상위의 토큰들을 읽었다면, 하부를 읽는 함수를 호출 해 준다.
		// 파일의 끝이 나올 때 까지 반복한다.
		// 
		// Top level category의 ID 중 앞의 몇 개를 빼고는 모두 중괄호를 열며 시작한다.
		// 내부에서 중괄호를 카운팅하며 제대로 끝날 때까지 읽는다.
		switch (_nowToken)
		{
			/// Top level category ID's

			// 3DSMAX_ASCIIEXPORT
			case TOKENR_3DSMAX_ASCIIEXPORT:
			{
				m_ASCIIExportSign = Parsing_NumberLong();
			}
			break;

			// COMMENT
			case TOKENR_COMMENT:
			{
				m_Comment = Parsing_String();
			}
			break;

			// SCENE
			case TOKENR_SCENE:
			{
				Parsing_Scene();
			}
			break;

			/// 이제 Node를 가진 오브젝트들

			// Geometry Object
			case TOKENR_GEOMOBJECT:
			{
				// Geometry Object 하나를 온전히 만들어서, 그 내용을 모두 읽도록 한다.
				// 내부에서 중괄호의 열기-닫기 갯수 체크를 한다.
				Parsing_GeometryObject();
			}
			break;

			// Shape Object
			case TOKENR_SHAPEOBJECT:
			{
				Parsing_ShapeObject();
			}
			break;

			// Camera Object
			case TOKENR_CAMERAOBJECT:
			{
				Parsing_CameraObject();
			}
			break;

			// Light Object
			case TOKENR_LIGHTOBJECT:
			{
				Parsing_LightObject();
			}
			break;

			// Helper Object
			case TOKENR_HELPEROBJECT:
			{
				Parsing_HelperObject();
			}
			break;

			// Materials
			case TOKENR_MATERIAL_LIST:
			{
				// 리스트가 있다는 것을 의미함.
				// 아래에 갯수와 개별 리스트가 나온다.
			}
			break;

			case TOKENR_MATERIAL_COUNT:
			{
				m_materialcount = Parsing_NumberLong();
			}
			break;

			case TOKENR_MATERIAL:
			{
				// 새 Material을 생성하고 번호 매겨주자
				int _newMaterialNumber = Parsing_NumberLong();
				Parsing_Material(_newMaterialNumber);
			}
			break;

			// 한 개의 그룹 시작
			case TOKENR_GROUP:
			{
				// 이 다음에 이름이 스트링으로 나오기는 하는데.
				/// 이것은 특별 취급 할 것
				/// Depth 1 category이며, Hierarchy 관련으로 분류 되어 있다.
			}
			break;

			case TOKEND_END:
			{
				// 파일의 끝이 나타났을 때. while을 탈출해야 한다.
				TRACE("TRACE: 파싱중: 파일의 끝을 봤습니다!\n");
				return;
			}
			break;

			default:
			{
				// 뭔가 문제가 있거나
				// 처리하지 않은 토큰이다.

				// 아무것도 하지 않는다.
			}
			break;

		} // switch()

	} // while()


	// 정상적으로 여기에 왔다면, 한 개의 ASE파일을 잘 읽었다는 뜻이다.
	return;
}


// 장면(Scene) 정보를 파싱한다.
void ASE::ASEParser::Parsing_Scene()
{
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_SCENE_FILENAME:
				m_scenedata.m_filename = Parsing_String();
				break;

			case TOKENR_SCENE_FIRSTFRAME:
				m_scenedata.m_firstframe = Parsing_NumberLong();
				break;

			case TOKENR_SCENE_LASTFRAME:
				m_scenedata.m_lastframe = Parsing_NumberLong();
				break;

			case TOKENR_SCENE_FRAMESPEED:
				m_scenedata.m_framespeed = Parsing_NumberLong();
				break;

			case TOKENR_SCENE_TICKSPERFRAME:
				m_scenedata.m_ticksperframe = Parsing_NumberLong();
				break;

			case TOKENR_SCENE_MESHFRAMESTEP:
				m_scenedata.m_meshframestep = Parsing_NumberLong();
				break;

			case TOKENR_SCENE_KEYFRAMESTEP:
				m_scenedata.m_keyframestep = Parsing_NumberLong();
				break;

			case TOKENR_SCENE_BACKGROUND_STATIC:
				m_scenedata.m_scene_background_static = Parsing_NumberVector3();
				break;

			case TOKENR_SCENE_AMBIENT_STATIC:
				m_scenedata.m_scene_ambient_static = Parsing_NumberVector3();
				break;

			default:
			{
				// 처리하지 않은 토큰이나, 다른 무엇인가가 들어왔을 때
				///case TOKENR_SCENE_ENVMAP:
			}
			break;

		} // switch()

	} // while()
}

// 새로 Geometry Object를 만들어서, 리스트에 추가 해 준다.
void ASE::ASEParser::Parsing_GeometryObject()
{
	GeometryObject* _newObject = new GeometryObject();
	_newObject->m_scenedata = m_scenedata;		// 값 복사. 애니메이션 등에서 각 오브젝트가 알아야 하기 때문이다.

	_newObject->m_type = eASEObjectType::Geometry;

	/// GEOMOBJECT 토큰 뒤에 바로 나오는 내용이 정해져 있다.
	/// 1. 이것을 확정적으로 (읽어 보고) 있으면 처리 할 것인지
	/// 2. while문을 돌려서 특정 토큰이 나올 때 대응 할지는 선택이다.
	/// 무엇이 나올지 확정인 경우는 while을 돌려서 케이스별 처리를 안 해도 될 것 같다.

	// 완전한 동작이 보장되어야 한다. {} 짝의 수를 카운팅한다.
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_NODE_NAME:
			{
				// 노드(ASE오브젝트)들을 구별 할 수 있는 유일한 값이다.
				// 오브젝트의 종류에 따라 넣어야 할 곳이 다르다.
				_newObject->m_nodename = Parsing_String();
			}
			break;

			case TOKENR_NODE_PARENT:
			{
				// 현 노드의 부모 노드의 정보
				_newObject->m_isparentexist = true;
				_newObject->m_nodeparent = Parsing_String();
			}
			break;

			// NodeTM을 파싱한다.
			case TOKENR_NODE_TM:
			{
				Parsing_NodeTM(_newObject);
			}
			break;

			// Geometry Object는 Mesh 정보를 가지고 있다.
			case TOKENR_MESH:
			{
				Parsing_Mesh(_newObject);
			}
			break;

			// 애니메이션 정보가 있다면 읽는다.
			case TOKENR_TM_ANIMATION:
			{
				Parsing_TMAnimation(_newObject);
			}
			break;

			/// Bone

			case TOKENR_MESH_NUMBONE:
			{
				// 본의 갯수. 이게 있다면 이것은 Skinned Mesh라고 판단한다.
				_newObject->m_is_skinningobject = true;
				_newObject->m_type = eASEObjectType::SkinnedMesh;
				_newObject->m_numbone = Parsing_NumberLong();
			}
			break;

			// 이 (스키닝) 오브젝트에 연동된 뼈대(Bone)의 정보
			case TOKENR_BONE_LIST:
			{
			}
			break;

			case TOKENR_SKIN_INITTM:
			{
			}
			break;

			case TOKENR_BONE:
			{
				/// 모드 체인지 해 주고, Bone을 소유하고 있다는 것은 이것은 스키닝 오브젝트라는 것이다.
				_newObject->m_is_skinningobject = true;
				_newObject->m_type = eASEObjectType::SkinnedMesh;

				// 본 하나를 만들어서 임시 포인터 보관, 벡터에 넣고
				Bone* _tempbone = new Bone;
				_newObject->m_bone = _tempbone;
				_newObject->m_vector_bone_list.push_back(_tempbone);

				// Bone의 넘버를 읽어 주자
				_newObject->m_bone->m_bone_number = Parsing_NumberLong();
			}
			break;

			// 본의 이름을 넣어야 한다.
			case TOKENR_BONE_NAME:
			{
				_newObject->m_bone->m_bone_name = Parsing_String();
			}
			break;

			case TOKENR_BONE_PROPERTY:
			{
				// 이 다음 ABSOLUTE가 나오기는 하는데, 쓸 일이 없다.
				// 다음에는 TM_ROW0~3이 나오는데 역시 무시됨..
			}
			break;

			case TOKENR_MESH_WVERTEXS:
			{

			}
			break;

			case TOKENR_MESH_WEIGHT:
			{
				// 버텍스 하나의 정보를 만들어서 리스트에 넣음
				VertexWeight* _wVertex = new VertexWeight;
				_newObject->m_wvertex = _wVertex;
				_newObject->m_vector_wvertexs.push_back(_wVertex);
				_newObject->m_wvertex->m_wvertex_number = Parsing_NumberLong();
			}
			break;

			case TOKENR_BONE_BLENGING_WEIGHT:
			{
				// 대체 몇 단계를 들어가는거야...
				// 가중치 한개를 만들어서 리스트에 넣는다
				Weight* _weight = new Weight;
				_newObject->m_wvertex->m_temp_bone_blend_weight = _weight;
				_weight->m_bone_number = Parsing_NumberLong();
				_weight->m_bone_weight = Parsing_NumberFloat();
				_newObject->m_wvertex->m_bone_blending_weight.push_back(_weight);
				/// 헥 헥....
			}
			break;

			default:
			{
				// 처리하지 않은 토큰이나, 다른 무엇인가가 들어왔을 때
			}
			break;

		} // switch()

	} // while()

	m_ASEObjectList.push_back(_newObject);
}

// 새로 Shape Object를 만들어서, 리스트에 추가 해 준다.
void ASE::ASEParser::Parsing_ShapeObject()
{
	GeometryObject* _newObject = new GeometryObject();
	_newObject->m_scenedata = m_scenedata;		// 값 복사. 애니메이션 등에서 각 오브젝트가 알아야 하기 때문이다.

	_newObject->m_type = eASEObjectType::Shape;

	/// 순서가 정해져 있으므로 이렇게 해도 된다. 다만...
	// 내용을 계속 파싱한다. 완전한 동작이 보장되어야 한다. 왠지 {} 짝의 수를 카운팅하고 싶다..
	//Parsing_NodeTM(_newObject);
	//Parsing_Shape(_newObject);
	//Parsing_TMAnimation(_newObject);

	/// 역시 제대로 토큰을 읽은 다음, 그것을 보장받고 파싱하는 것이 좋을 것이다.


	// 완전한 동작이 보장되어야 한다. {} 짝의 수를 카운팅한다.
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_NODE_NAME:
			{
				// 노드(ASE오브젝트)들을 구별 할 수 있는 유일한 값이다.
				// 오브젝트의 종류에 따라 넣어야 할 곳이 다르다.
				_newObject->m_nodename = Parsing_String();
			}
			break;

			case TOKENR_NODE_PARENT:
			{
				// 현 노드의 부모 노드의 정보
				_newObject->m_isparentexist = true;
				_newObject->m_nodeparent = Parsing_String();
			}
			break;

			// NodeTM을 파싱한다.
			case TOKENR_NODE_TM:
			{
				Parsing_NodeTM(_newObject);
			}
			break;

			// Shape Object는 Shape 정보를 가지고 있다.
			case TOKENR_SHAPEOBJECT:
			{
				Parsing_Shape(_newObject);
			}
			break;

			// 애니메이션 정보가 있다면 읽는다.
			case TOKENR_TM_ANIMATION:
			{
				Parsing_TMAnimation(_newObject);
			}
			break;

			default:
			{
				// 처리하지 않은 토큰이나, 다른 무엇인가가 들어왔을 때
			}
			break;

		} // switch()

	} // while()

	m_ASEObjectList.push_back(_newObject);
}

void ASE::ASEParser::Parsing_CameraObject()
{
	GeometryObject* _newObject = new GeometryObject();
	_newObject->m_scenedata = m_scenedata;		// 값 복사. 애니메이션 등에서 각 오브젝트가 알아야 하기 때문이다.

	_newObject->m_type = eASEObjectType::Camera;

	// 완전한 동작이 보장되어야 한다. {} 짝의 수를 카운팅한다.
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_NODE_NAME:
			{
				// 카메라는 NodeTM이 두 번 나온다.
				// 두 번째라면 넣지 않는다.
				if (_newObject->m_camera_isloadTarget) break;

				_newObject->m_nodename = Parsing_String();
			}
			break;

			case TOKENR_NODE_PARENT:
			{
				// 현 노드의 부모 노드의 정보
				_newObject->m_isparentexist = true;
				_newObject->m_nodeparent = Parsing_String();
			}
			break;

			// NodeTM을 파싱한다.
			case TOKENR_NODE_TM:
			{
				Parsing_NodeTM(_newObject);
			}
			break;

			// 카메라 정보를 파싱한다.
			case TOKENR_CAMERAOBJECT:
			{
				Parsing_CameraData(_newObject);
			}
			break;

			// 애니메이션 정보가 있다면 읽는다.
			case TOKENR_TM_ANIMATION:
			{
				Parsing_TMAnimation(_newObject);
			}
			break;

			default:
			{
				// 처리하지 않은 토큰이나, 다른 무엇인가가 들어왔을 때
			}
			break;

		} // switch()

	} // while()

	m_ASEObjectList.push_back(_newObject);
}

void ASE::ASEParser::Parsing_LightObject()
{
	GeometryObject* _newObject = new GeometryObject();
	_newObject->m_scenedata = m_scenedata;		// 값 복사. 애니메이션 등에서 각 오브젝트가 알아야 하기 때문이다.

	_newObject->m_type = eASEObjectType::Light;

	// 완전한 동작이 보장되어야 한다. {} 짝의 수를 카운팅한다.
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_NODE_NAME:
			{
				// 마찬가지의 이유로 Light도 넣지 않는다.
				if (_newObject->m_light_isloadTarget) break;

				_newObject->m_nodename = Parsing_String();
			}
			break;

			case TOKENR_NODE_PARENT:
			{
				// 현 노드의 부모 노드의 정보
				_newObject->m_isparentexist = true;
				_newObject->m_nodeparent = Parsing_String();
			}
			break;

			// NodeTM을 파싱한다.
			case TOKENR_NODE_TM:
			{
				Parsing_NodeTM(_newObject);
			}
			break;

			// 빛 정보를 파싱한다.
			case TOKENR_LIGHTOBJECT:
			{
				Parsing_LightData(_newObject);
			}
			break;

			// 애니메이션 정보가 있다면 읽는다.
			case TOKENR_TM_ANIMATION:
			{
				Parsing_TMAnimation(_newObject);
			}
			break;

			default:
			{
				// 처리하지 않은 토큰이나, 다른 무엇인가가 들어왔을 때
			}
			break;

		} // switch()

	} // while()

	m_ASEObjectList.push_back(_newObject);
}

void ASE::ASEParser::Parsing_HelperObject()
{
	GeometryObject* _newObject = new GeometryObject();
	_newObject->m_scenedata = m_scenedata;		// 값 복사. 애니메이션 등에서 각 오브젝트가 알아야 하기 때문이다.

	_newObject->m_type = eASEObjectType::Helper;

	// 완전한 동작이 보장되어야 한다. {} 짝의 수를 카운팅한다.
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_NODE_NAME:
			{
				// 노드(ASE오브젝트)들을 구별 할 수 있는 유일한 값이다.
				// 오브젝트의 종류에 따라 넣어야 할 곳이 다르다.
				_newObject->m_nodename = Parsing_String();
			}
			break;

			case TOKENR_NODE_PARENT:
			{
				// 현 노드의 부모 노드의 정보
				_newObject->m_isparentexist = true;
				_newObject->m_nodeparent = Parsing_String();
			}
			break;

			// NodeTM을 파싱한다.
			case TOKENR_NODE_TM:
			{
				Parsing_NodeTM(_newObject);
			}
			break;

			// 애니메이션 정보가 있다면 읽는다.
			case TOKENR_TM_ANIMATION:
			{
				Parsing_TMAnimation(_newObject);
			}
			break;

			default:
			{
				// 처리하지 않은 토큰이나, 다른 무엇인가가 들어왔을 때
			}
			break;

		} // switch()

	} // while()

	m_ASEObjectList.push_back(_newObject);
}

void ASE::ASEParser::Parsing_Material(int materialIndex)
{
	ASEMaterial* _newMaterial = new ASEMaterial;
	_newMaterial->m_materialnumber = materialIndex;

	Parsing_MaterialData(_newMaterial);

	m_list_materialdata.push_back(_newMaterial);
}

/// <summary>
/// 한 개의 노드 변환 행렬을 파싱한다.
/// </summary>
void ASE::ASEParser::Parsing_NodeTM(GeometryObject* targetObject)
{
	// 완전한 동작이 보장되어야 한다. {} 짝의 수를 카운팅한다.
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	/// 토큰을 한 개씩 읽으면서, 적절한 처리를 해 준다.
	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_NODE_TM:
				//m_parsingmode	=	eGeomobject;
				// (NODE_TM으로 진입 후 NODE_NAME이 한번 더 나온다.)
				// (Animation과도 구별을 해야 하기 때문에 이렇게 모드를 적어준다)

				/// 게다가,
				// 카메라는 NodeTM이 두번 나온다. 두번째라면 넣지 않는다.
				// 아예 이 재귀에서 리턴을 시키고 싶지만. 중간에 읽는것을 무시해야 하기 때문에...
				//if (targetObject->m_camera_isloadTarget) {
				//	'}'이 나올때까지 나오는건 무조건 무시! 뭐 이런거 안되나..
				// 재귀구조의 문제점이다....

				break;

			case TOKENR_INHERIT_POS:
			{
				// 카메라는 NodeTM이 두번 나온다. 두번째라면 넣지 않는다.
				if (targetObject->m_camera_isloadTarget) break;
				if (targetObject->m_light_isloadTarget) break;

				targetObject->m_inherit_pos = Parsing_NumberVector3_YZ();
			}
			break;

			case TOKENR_INHERIT_ROT:
			{
				if (targetObject->m_camera_isloadTarget) break;
				if (targetObject->m_light_isloadTarget) break;

				targetObject->m_inherit_rot = Parsing_NumberVector3_YZ();
			}
			break;

			case TOKENR_INHERIT_SCL:
			{

				if (targetObject->m_camera_isloadTarget) break;
				if (targetObject->m_light_isloadTarget) break;
				targetObject->m_inherit_scl = Parsing_NumberVector3_YZ();
			}
			break;

			case TOKENR_TM_ROW0:
			{
				if (targetObject->m_camera_isloadTarget) break;
				if (targetObject->m_light_isloadTarget) break;

				// SKIN_INITTM일 경우 무시한다.
				///if (m_parsingmode == eSkin_InitTM) break;
				///if (m_parsingmode == eBone_List) break;
				targetObject->m_tm_row0.x = Parsing_NumberFloat();
				targetObject->m_tm_row0.y = Parsing_NumberFloat();	/// yz는 INodeTM에서 바꾼다. 특별한 이유가 없다면 여기에서 미리 바꾸자..
				targetObject->m_tm_row0.z = Parsing_NumberFloat();
			}
			break;

			case TOKENR_TM_ROW1:
			{
				if (targetObject->m_light_isloadTarget) break;
				if (targetObject->m_camera_isloadTarget) break;
				///if (m_parsingmode == eSkin_InitTM) break;
				///if (m_parsingmode == eBone_List) break;
				targetObject->m_tm_row1.x = Parsing_NumberFloat();
				targetObject->m_tm_row1.y = Parsing_NumberFloat();
				targetObject->m_tm_row1.z = Parsing_NumberFloat();
			}
			break;

			case TOKENR_TM_ROW2:
			{
				if (targetObject->m_camera_isloadTarget) break;
				if (targetObject->m_light_isloadTarget) break;
				///if (m_parsingmode == eSkin_InitTM) break;
				///if (m_parsingmode == eBone_List) break;
				targetObject->m_tm_row2.x = Parsing_NumberFloat();
				targetObject->m_tm_row2.y = Parsing_NumberFloat();
				targetObject->m_tm_row2.z = Parsing_NumberFloat();
			}
			break;

			case TOKENR_TM_ROW3:
			{
				if (targetObject->m_camera_isloadTarget) break;
				if (targetObject->m_light_isloadTarget) break;
				///if (m_parsingmode == eSkin_InitTM) break;
				///if (m_parsingmode == eBone_List) break;
				targetObject->m_tm_row3.x = Parsing_NumberFloat();
				targetObject->m_tm_row3.y = Parsing_NumberFloat();
				targetObject->m_tm_row3.z = Parsing_NumberFloat();
			}
			break;

			case TOKENR_TM_POS:
			{
				if (targetObject->m_camera_isloadTarget) break;
				if (targetObject->m_light_isloadTarget) break;
				targetObject->m_tm_pos.x = Parsing_NumberFloat();
				targetObject->m_tm_pos.z = Parsing_NumberFloat();
				targetObject->m_tm_pos.y = Parsing_NumberFloat();
			}
			break;

			case TOKENR_TM_ROTAXIS:
			{
				if (targetObject->m_camera_isloadTarget) break;
				if (targetObject->m_light_isloadTarget) break;
				targetObject->m_tm_rotaxis.x = Parsing_NumberFloat();
				targetObject->m_tm_rotaxis.z = Parsing_NumberFloat();
				targetObject->m_tm_rotaxis.y = Parsing_NumberFloat();
			}
			break;

			case TOKENR_TM_ROTANGLE:
			{
				if (targetObject->m_camera_isloadTarget) break;
				if (targetObject->m_light_isloadTarget) break;
				targetObject->m_tm_rotangle = Parsing_NumberFloat();
			}
			break;

			case TOKENR_TM_SCALE:
			{
				if (targetObject->m_camera_isloadTarget) break;
				if (targetObject->m_light_isloadTarget) break;
				targetObject->m_tm_scale.x = Parsing_NumberFloat();
				targetObject->m_tm_scale.z = Parsing_NumberFloat();
				targetObject->m_tm_scale.y = Parsing_NumberFloat();
			}
			break;

			case TOKENR_TM_SCALEAXIS:
			{
				if (targetObject->m_camera_isloadTarget) break;
				if (targetObject->m_light_isloadTarget) break;
				targetObject->m_tm_scaleaxis.x = Parsing_NumberFloat();
				targetObject->m_tm_scaleaxis.z = Parsing_NumberFloat();
				targetObject->m_tm_scaleaxis.y = Parsing_NumberFloat();
			}
			break;

			case TOKENR_TM_SCALEAXISANG:
			{
				if (targetObject->m_camera_isloadTarget) break;
				if (targetObject->m_light_isloadTarget) break;
				targetObject->m_tm_scaleaxisang = Parsing_NumberFloat();

				// 현재 카메라 상태였다면 이미 노드를 읽은 것으로 표시해준다.
				if (targetObject->m_type == eASEObjectType::Camera) targetObject->m_camera_isloadTarget = true;
				if (targetObject->m_type == eASEObjectType::Light) targetObject->m_light_isloadTarget = true;
			}
			break;

			// 아무 토큰에도 해당하지 않을 때
			default:
			{
				// 처리하지 않은 토큰이나, 다른 무엇인가가 들어왔을 때
			}
			break;

		} // switch()

	} // while()
}

void ASE::ASEParser::Parsing_TMAnimation(GeometryObject* targetObject)
{
	// 애니메이션 데이터를 생성, 리스트에 넣어준다.
	Animation* _newAnimation = new Animation;
	m_list_animation.push_back(_newAnimation);

	// 오브젝트에 애니메이션 데이터가 있음을 기록한다.
	targetObject->m_isAnimated = true;

	// 씬 정보로부터 애니메이션 틱 정보를 가져온다.
	_newAnimation->m_ticksperFrame = m_scenedata.m_ticksperframe;
	_newAnimation->m_minKeyframe = m_scenedata.m_ticksperframe;

	// 계속해서 정보를 읽는다.
	Parsing_TMAnimationData(_newAnimation);
}

void ASE::ASEParser::Parsing_TMAnimationData(Animation* animation)
{
	Matrix TM1, TM2;
	Quaternion tempQT;
	Quaternion prevQT;
	Quaternion resultQT;

	// 완전한 동작이 보장되어야 한다. {} 짝의 수를 카운팅한다.
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_NODE_NAME:
			{
				animation->m_nodename = Parsing_String();
			}
			break;

			// 이 세 가지는 그냥 괄호를 여는 것밖에는 하는 일이 없다.
			case TOKENR_CONTROL_POS_TRACK:
				break;

			case TOKENR_CONTROL_ROT_TRACK:
				break;

			case TOKENR_CONTROL_SCALE_TRACK:
				break;

			case TOKENR_CONTROL_POS_SAMPLE:
			{
				// 포지션을 하나 만들고, 읽고, 벡터에 넣는다
				CAnimation_pos* _pos = new CAnimation_pos;
				_pos->m_time = Parsing_NumberInt();
				_pos->m_pos.x = Parsing_NumberFloat();
				_pos->m_pos.z = Parsing_NumberFloat();
				_pos->m_pos.y = Parsing_NumberFloat();

				// m_position은 vector이다. 뒤에 한 개를 추가시켜 준다.
				animation->m_position.push_back(_pos);
			}
			break;

			case TOKENR_CONTROL_ROT_SAMPLE:
			{
				// 로테이션을 하나 만들고, 읽고, 벡터에 넣는다
				CAnimation_rot* _rot = new CAnimation_rot;
				_rot->m_time = Parsing_NumberInt();
				_rot->m_rot.x = Parsing_NumberFloat();
				_rot->m_rot.z = Parsing_NumberFloat();
				_rot->m_rot.y = Parsing_NumberFloat();
				_rot->m_angle = Parsing_NumberFloat();

				/// 지금까지의 내용을 읽어서 누적, 쿼터니언으로 만들어 넣는다.
				//Quaternion tempQT;	(앞쪽에 선언돼있다)
				//Quaternion prevQT;
				//Quaternion resultQT;

				// 현재 쿼터니온을 구해서,
				tempQT = XMQuaternionRotationAxis(_rot->m_rot, _rot->m_angle);

				/// 쿼터니온이 제대로 옮겨졌는지 확인했다.
				TM1 = XMMatrixRotationAxis(_rot->m_rot, _rot->m_angle);
				TM2 = XMMatrixRotationQuaternion(tempQT);

				// 벡터의 사이즈가 0이라면 이전 내용이 없는 것이므로 그냥 넣고
				if (animation->m_rotation.size() == 0)
				{
					// 현재 값을 그대로 넣어준다.
					_rot->m_rotQT_accumulation = tempQT;
					animation->m_rotation.push_back(_rot);
				}
				else
				{
					// 아니라면 이전 내용이 있는 것이므로 이전 내용과 Multiply
					// 이건 이전 내용
					prevQT = animation->m_rotation[animation->m_rotation.size() - 1]->m_rotQT_accumulation;
					resultQT = XMQuaternionMultiply(prevQT, tempQT);
					_rot->m_rotQT_accumulation = resultQT;
					animation->m_rotation.push_back(_rot);
				}
			}
			break;

			case TOKENR_CONTROL_SCALE_SAMPLE:
			{
				// 스케일을 하나 만들고 벡터에 넣는다
				CAnimation_scl* _scl = new CAnimation_scl;
				_scl->m_time = Parsing_NumberInt();
				_scl->m_scale = Parsing_NumberVector3_YZ();
				_scl->m_scaleaxis = Parsing_NumberVector3_YZ();
				_scl->m_scaleaxisang = Parsing_NumberFloat();

				// 현재 쿼터니온
				tempQT = XMQuaternionRotationAxis(_scl->m_scaleaxis, _scl->m_scaleaxisang);

				// 벡터의 사이즈가 0이라면 이전 내용이 없는 것이므로 그냥
				if (animation->m_scale.size() == 0)
				{
					// 현재 값을 그대로 넣어준다.
					_scl->m_scalerotQT_accumulation = tempQT;
				}
				else
				{
					// 아니라면 이전 내용이 있는 것이므로 이전 내용과 Multiply
					// 이건 이전 내용
					prevQT = animation->m_scale[animation->m_scale.size() - 1]->m_scalerotQT_accumulation;
					resultQT = XMQuaternionMultiply(prevQT, tempQT);
					_scl->m_scalerotQT_accumulation = resultQT;

				}

				// 벡터에 넣어준다.
				animation->m_scale.push_back(_scl);
			}
			break;

		} // switch()

	} // while()
}

void ASE::ASEParser::Parsing_Mesh(GeometryObject* targetObject)
{
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_TIMEVALUE:
				if (targetObject->m_light_isAnimate == true)
				{
					targetObject->m_lightanimation->m_timevalue = Parsing_NumberFloat();
				}
				else
				{
					targetObject->m_timevalue = Parsing_NumberFloat();
				}
				break;



			case TOKENR_MESH_NUMSKINWEIGHT:
				targetObject->m_numskinweight = Parsing_NumberLong();
				break;

			case TOKENR_MESH_NUMVERTEX:
				targetObject->m_mesh_numvertex = Parsing_NumberLong();
				break;

			case TOKENR_MESH_NUMFACES:
				targetObject->m_mesh_numfaces = Parsing_NumberLong();
				break;

				/// MESH_VERTEX_LIST

			case TOKENR_MESH_VERTEX_LIST:
				//
				// 버텍스의 값들을 집어넣어야 하는데
				// 이미 벡터로 선언이 돼 있으므로 그냥 넣으면 된다.
				break;

			case TOKENR_MESH_VERTEX:
			{
				// 데이터 입력

				int vertexnumber;
				vertexnumber = Parsing_NumberLong();		// 첫번째는 버텍스의 번호

				// 루프를 돌까도 생각해봤지만, 할당된 크기만 정확하다면
				// vertex정보 자체에 번호가 있기 때문에 카운트도, 루프도 필요없다!

				// 버텍스 벡터에 데이터를 하나 넣자
				Vertex* _newVertex = new Vertex;
				targetObject->m_meshvertex.push_back(_newVertex);
				targetObject->m_meshvertex[vertexnumber]->m_pos = Parsing_NumberVector3_YZ();
			}
			break;

			/// MESH_FACE_LIST
			case TOKENR_MESH_FACE_LIST:
				//
				break;

			case TOKENR_MESH_FACE:
			{
				Face* temp = new Face;

				int facenumber;
				facenumber = Parsing_NumberLong();			// Face의 번호인데...

				Parsing_String();		// A:를 읽고
				temp->m_vertexindex[0] = Parsing_NumberLong();
				Parsing_String();		// B:
				temp->m_vertexindex[2] = Parsing_NumberLong();	/// 021인것은 y-z좌표계 변환 때문
				Parsing_String();		// C:
				temp->m_vertexindex[1] = Parsing_NumberLong();

				/// (뒤에 정보가 더 있지만 default에 의해 스킵될 것이다.)
				/// ......

				static int count = 0;
				count++;
				// 벡터에 넣어준다.
				targetObject->m_meshface.push_back(temp);
			}
			break;

			case TOKENR_MESH_NUMTVERTEX:
				targetObject->m_mesh_numtvertex = Parsing_NumberLong();
				break;

				/// MESH_TVERTLIST

			case TOKENR_MESH_TVERTLIST:
				//
				break;

			case TOKENR_MESH_TVERT:
			{
				// 버텍스의 인덱스가 나오는데 어차피 순서와 같으므로 버린다.
				Parsing_NumberLong();

				// 새로운 TVertex를 만들어서 벡터에 넣는다
				COneTVertex* temp = new COneTVertex;
				temp->m_u = Parsing_NumberFloat();
				temp->m_v = 1.0f - Parsing_NumberFloat();		/// y-z 뒤집힘 때문에 v도 1-v을 해줘야 함
				temp->m_w = Parsing_NumberFloat();
				targetObject->m_mesh_tvertex.push_back(temp);
			}
			break;

			case TOKENR_MESH_NUMTVFACES:
				targetObject->m_mesh_tvfaces = Parsing_NumberLong();
				break;

				/// MESH_TFACELIST

			case TOKENR_MESH_TFACELIST:
				//
				break;

			case TOKENR_MESH_TFACE:
			{
				int facenumber;
				facenumber = Parsing_NumberLong();	// Face의 번호. TVFaces는 VFaces와 갯수가 같다고 가정한다. (그렇다면 바로 위의 NUMTVFACES는 왜 있을까? 하는 의문이 들기는 하지만.)
				targetObject->m_meshface[facenumber]->m_TFace[0] = Parsing_NumberLong();
				targetObject->m_meshface[facenumber]->m_TFace[2] = Parsing_NumberLong();/// 021인것은 y-z좌표계 변환 때문
				targetObject->m_meshface[facenumber]->m_TFace[1] = Parsing_NumberLong();
			}
			break;

			case TOKENR_MESH_NUMCVERTEX:
				targetObject->m_mesh_numcvertex = Parsing_NumberLong();
				break;

				/// MESH_NORMALS
			case TOKENR_MESH_NORMALS:
				//
				targetObject->m_mesh_vertexnormal = new Vector3[targetObject->m_mesh_numvertex];
				break;

			case TOKENR_MESH_FACENORMAL:
			{
				// 페이스 노말 데이터 입력
				int facenumber;
				facenumber = Parsing_NumberLong();			// Face의 번호인데...

				targetObject->m_meshface[facenumber]->m_normal.x = Parsing_NumberFloat();
				targetObject->m_meshface[facenumber]->m_normal.z = Parsing_NumberFloat();
				targetObject->m_meshface[facenumber]->m_normal.y = Parsing_NumberFloat();

				///case TOKENR_MESH_VERTEXNORMAL:
				for (int i = 0; i < 3; i++)
				{
					if (m_lexer->GetToken(m_TokenString) == TOKENR_MESH_VERTEXNORMAL)
					{
						//	일단 숫자 하나를 읽는데 이것은 버텍스의 인덱스.
						Parsing_NumberLong();

						// 021식으로 읽으려면..
						int _convertedIndex = 0;
						if (i == 0) _convertedIndex = 0;
						else if (i == 1) _convertedIndex = 2;
						else if (i == 2) _convertedIndex = 1;
						targetObject->m_meshface[facenumber]->m_normalvertex[_convertedIndex].x = Parsing_NumberFloat();
						targetObject->m_meshface[facenumber]->m_normalvertex[_convertedIndex].y = Parsing_NumberFloat();
						targetObject->m_meshface[facenumber]->m_normalvertex[_convertedIndex].z = Parsing_NumberFloat();
					}
				}
			}
			break;

		} // switch()

	} // while()
}

void ASE::ASEParser::Parsing_Shape(GeometryObject* targetObject)
{
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_SHAPE_LINECOUNT:
			{
				targetObject->m_shape_linecount = Parsing_NumberLong();
			}
			break;

			case TOKENR_SHAPE_LINE:
			{
				// 리스트에 한 개의 라인을 추가한다.
				ShapeLine* temp = new ShapeLine;
				m_nowshapeline = temp;
				m_nowshapeline->m_line_number = Parsing_NumberLong();		// 현재 무슨 라인인가?
				targetObject->m_vector_shape_line.push_back(m_nowshapeline);
			}
			break;

			case TOKENR_SHAPE_CLOSED:
				m_nowshapeline->m_shape_closed = true;
				break;

			case TOKENR_SHAPE_VERTEXCOUNT:
				m_nowshapeline->m_shape_vertexcount = Parsing_NumberLong();
				break;

			case TOKENR_SHAPE_VERTEX_KNOT:
			{
				ShapeVertex* temp = new ShapeVertex;
				temp->m_isknot = true;
				temp->m_indexnum = Parsing_NumberLong();
				// 			temp->m_pos.x		= Parsing_NumberFloat();
				// 			temp->m_pos.z		= Parsing_NumberFloat();
				// 			temp->m_pos.y		= Parsing_NumberFloat();
				temp->m_pos = Parsing_NumberVector3_YZ();		/// 만들었으니 한번 써 봤다. 나중에 다 이것으로 대체하자.

				m_nowshapeline->m_shapevertex.push_back(temp);
			}
			break;

			case TOKENR_SHAPE_VERTEX_INTERP:
			{
				ShapeVertex* temp = new ShapeVertex;
				temp->m_isknot = false;
				temp->m_indexnum = Parsing_NumberLong();
				temp->m_pos = Parsing_NumberVector3_YZ();

				m_nowshapeline->m_shapevertex.push_back(temp);
			}
			break;

		} // switch()

	} // while()
}

void ASE::ASEParser::Parsing_CameraData(GeometryObject* targetObject)
{
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_CAMERA_TYPE:
				Parsing_String();
				targetObject->m_camera_type = m_TokenString;
				break;

			case TOKENR_CAMERA_SETTINGS:
				// 카메라 셋팅을 읽는다. 별 다른건 없다.
				break;

				//case TOKENR_TIMEVALUE:		//-> 이건 이미 MESH에 있다.

			case TOKENR_CAMERA_NEAR:
				targetObject->m_camera_near = Parsing_NumberFloat();
				break;

			case TOKENR_CAMERA_FAR:
				targetObject->m_camera_far = Parsing_NumberFloat();
				break;

			case TOKENR_CAMERA_FOV:
				targetObject->m_camera_fov = Parsing_NumberFloat();
				break;

			case TOKENR_CAMERA_TDIST:
				targetObject->m_camera_tdist = Parsing_NumberFloat();
				break;

		} // switch()

	} // while()
}

void ASE::ASEParser::Parsing_LightData(GeometryObject* targetObject)
{
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_LIGHT_TYPE:
				Parsing_String();
				targetObject->m_light_type = m_TokenString;	// 아마도 target
				break;

			case TOKENR_LIGHT_SHADOWS:
				Parsing_String();
				targetObject->m_light_shadows = m_TokenString;	// 아마도 target
				break;

			case TOKENR_LIGHT_USELIGHT:
				targetObject->m_light_uselight = Parsing_NumberLong();
				break;

			case TOKENR_LIGHT_SPOTSHAPE:
				Parsing_String();
				targetObject->m_light_spotshape = m_TokenString;	// 아마도 target
				break;

			case TOKENR_LIGHT_USEGLOBAL:
				targetObject->m_light_useglobal = Parsing_NumberLong();
				break;

			case TOKENR_LIGHT_ABSMAPBIAS:
				targetObject->m_light_absmapbias = Parsing_NumberLong();
				break;

			case TOKENR_LIGHT_OVERSHOOT:
				targetObject->m_light_overshoot = Parsing_NumberLong();
				break;

			case TOKENR_LIGHT_SETTINGS:
				// Light Setting으로 돌입
				// 돌입하면서, 하나의 Setting클래스 포인터를 만들고,
				// 그 포인터를 리스트에 넣는다.
			{
				// 새로운 포인터를 만들고, 그것을 리스트에 넣는다.
				Light_AnimationData* temp;
				temp = new Light_AnimationData;
				targetObject->m_lightanimation = temp;
				targetObject->m_list_lightanimation.push_back(temp);
			}
			break;

			case TOKENR_LIGHT_COLOR:
				targetObject->m_lightanimation->m_light_color.x = Parsing_NumberFloat();
				targetObject->m_lightanimation->m_light_color.y = Parsing_NumberFloat();
				targetObject->m_lightanimation->m_light_color.z = Parsing_NumberFloat();
				break;

			case TOKENR_LIGHT_INTENS:
				targetObject->m_lightanimation->m_light_intens = Parsing_NumberFloat();
				break;

			case TOKENR_LIGHT_ASPECT:
				targetObject->m_lightanimation->m_light_aspect = Parsing_NumberFloat();
				break;

			case TOKENR_LIGHT_HOTSPOT:
				targetObject->m_lightanimation->m_light_hotspot = Parsing_NumberFloat();
				break;

			case TOKENR_LIGHT_FALLOFF:
				targetObject->m_lightanimation->m_light_falloff = Parsing_NumberFloat();
				break;

			case TOKENR_LIGHT_TDIST:
				targetObject->m_lightanimation->m_light_tdist = Parsing_NumberFloat();
				break;

			case TOKENR_LIGHT_MAPBIAS:
				targetObject->m_lightanimation->m_light_mapbias = Parsing_NumberFloat();
				break;

			case TOKENR_LIGHT_MAPRANGE:
				targetObject->m_lightanimation->m_light_maprange = Parsing_NumberFloat();
				break;

			case TOKENR_LIGHT_MAPSIZE:
				targetObject->m_lightanimation->m_light_mapsize = Parsing_NumberLong();
				break;

			case TOKENR_LIGHT_RAYBIAS:
				targetObject->m_lightanimation->m_light_raybias = Parsing_NumberFloat();
				break;

			case TOKENR_LIGHT_ANIMATION:
				// 애니메이션 돌입함을 알리고, 그 다음부터 나오는 LIGHT_SETTING은
				// 리스트를 만들고서, 넣어야 한다.
				targetObject->m_light_isAnimate = true;
				break;

		} // switch()

	} // while()
}

void ASE::ASEParser::Parsing_Misc(GeometryObject* targetObject)
{
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			case TOKENR_PROP_MOTIONBLUR:
			{
				targetObject->m_prop_motionblur = Parsing_NumberLong();
			}
			break;

			case TOKENR_PROP_CASTSHADOW:
			{
				targetObject->m_prop_castshadow = Parsing_NumberLong();
			}
			break;

			case TOKENR_PROP_RECVSHADOW:
			{
				targetObject->m_prop_recvshadow = Parsing_NumberLong();
			}
			break;

			case TOKENR_MATERIAL_REF:
			{
				targetObject->m_material_ref = Parsing_NumberLong();
			}
			break;

			case TOKENR_WIREFRAME_COLOR:
			{
				targetObject->m_wireframe_color = Parsing_NumberVector3();
			}
			break;

		} // switch()

	} // while()
}

void ASE::ASEParser::Parsing_MaterialData(ASEMaterial* material)
{
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;
			case TOKENR_MATERIAL_NAME:
			{
				material->m_material_name = Parsing_String();
			}
			break;

			case TOKENR_MATERIAL_CLASS:
			{
				material->m_material_class = Parsing_String();
			}
			break;

			case TOKENR_MATERIAL_AMBIENT:
			{
				material->m_material_ambient.x = Parsing_NumberFloat();
				material->m_material_ambient.y = Parsing_NumberFloat();
				material->m_material_ambient.z = Parsing_NumberFloat();
			}
			break;

			case TOKENR_MATERIAL_DIFFUSE:
			{
				material->m_material_diffuse.x = Parsing_NumberFloat();
				material->m_material_diffuse.y = Parsing_NumberFloat();
				material->m_material_diffuse.z = Parsing_NumberFloat();
			}
			break;

			case TOKENR_MATERIAL_SPECULAR:
			{
				material->m_material_specular.x = Parsing_NumberFloat();
				material->m_material_specular.y = Parsing_NumberFloat();
				material->m_material_specular.z = Parsing_NumberFloat();
			}
			break;

			case TOKENR_MATERIAL_SHINE:
			{
				material->m_material_shine = Parsing_NumberFloat();
			}
			break;

			case TOKENR_MATERIAL_SHINESTRENGTH:
			{
				material->m_material_shinestrength = Parsing_NumberFloat();
			}
			break;

			case TOKENR_MATERIAL_TRANSPARENCY:
			{
				material->m_material_transparency = Parsing_NumberFloat();
			}
			break;

			case TOKENR_MATERIAL_WIRESIZE:
			{
				material->m_material_wiresize = Parsing_NumberFloat();
			}
			break;

			case TOKENR_NUMSUBMTLS:
			{
				material->m_numsubmtls = Parsing_NumberLong();
			}
			break;

			case TOKENR_SUBMATERIAL:
			{
				// (서브 매터리얼 번호를 읽고)
				//
			}
			break;

			/// MATERIAL - MAP_DIFFUSE 등등

			// 맵 하나를 만들고, 현재 Material에 넣어 준다
			case TOKENR_MAP_SELFILLUM:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// 임시로 쓰이는 '현재' 맵 데이터
			}
			break;

			// 맵 하나를 만들고, 현재 Material에 넣어 준다
			case TOKENR_MAP_SHINE:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// 임시로 쓰이는 '현재' 맵 데이터
			}
			break;

			// 맵 하나를 만들고, 현재 Material에 넣어 준다
			case TOKENR_MAP_BUMP:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// 임시로 쓰이는 '현재' 맵 데이터
			}
			break;

			// 맵 하나를 만들고, 현재 Material에 넣어 준다
			case TOKENR_MAP_OPACITY:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// 임시로 쓰이는 '현재' 맵 데이터
			}
			break;

			// 맵 하나를 만들고, 현재 Material에 넣어 준다
			case TOKENR_MAP_REFLECT:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// 임시로 쓰이는 '현재' 맵 데이터
			}
			break;

			// 맵 하나를 만들고, 현재 Material에 넣어 준다
			case TOKENR_MAP_GENERIC:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// 임시로 쓰이는 '현재' 맵 데이터
			}
			break;

			// 맵 하나를 만들고, 현재 Material에 넣어 준다
			case TOKENR_MAP_SPECULAR:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// 임시로 쓰이는 '현재' 맵 데이터
			}
			break;

			// 맵 하나를 만들고, 현재 Material에 넣어 준다
			case TOKENR_MAP_DIFFUSE:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// 임시로 쓰이는 '현재' 맵 데이터
			}
			break;

			case TOKENR_MAP_NAME:
			{
				material->m_map_temp->m_map_name = Parsing_String();
			}
			break;

			case TOKENR_MAP_CLASS:
			{
				material->m_map_temp->m_map_class = Parsing_String();
			}
			break;

			case TOKENR_MAP_SUBNO:
			{
				material->m_map_temp->m_subno = Parsing_NumberLong();
			}
			break;

			case TOKENR_MAP_AMOUNT:
			{
				material->m_map_temp->m_map_amount = Parsing_NumberFloat();
			}
			break;

			case TOKENR_BITMAP:
			{
				material->m_map_temp->m_bitmap = Parsing_String();
				// 비트맵이 한개라도 있으면 텍스쳐가 있는 것이다
				material->m_istextureexist = true;
			}
			break;

			case TOKENR_MAP_TYPE:
			{
				material->m_map_temp->m_map_type = Parsing_NumberLong();
			}
			break;

			case TOKENR_UVW_U_OFFSET:
			{
				material->m_map_temp->m_uvw_u_offset = Parsing_NumberFloat();
			}
			break;

			case TOKENR_UVW_V_OFFSET:
			{
				material->m_map_temp->m_uvw_v_offset = Parsing_NumberFloat();
			}
			break;

			case TOKENR_UVW_U_TILING:
			{
				material->m_map_temp->m_u_tiling = Parsing_NumberFloat();
			}
			break;

			case TOKENR_UVW_V_TILING:
			{
				material->m_map_temp->m_v_tiling = Parsing_NumberFloat();
			}
			break;

			case TOKENR_UVW_ANGLE:
			{
				material->m_map_temp->m_uvw_angle = Parsing_NumberFloat();
			}
			break;

			case TOKENR_UVW_BLUR:
			{
				material->m_map_temp->m_uvw_blur = Parsing_NumberFloat();
			}
			break;

			case TOKENR_UVW_BLUR_OFFSET:
			{
				material->m_map_temp->m_uvw_blur_offset = Parsing_NumberFloat();
			}
			break;

			case TOKENR_UVW_NOUSE_AMT:
			{
				material->m_map_temp->m_uvw_noise_amt = Parsing_NumberFloat();
			}
			break;

			case TOKENR_UVW_NOISE_SIZE:
			{
				material->m_map_temp->m_uvw_noise_size = Parsing_NumberFloat();
			}
			break;

			case TOKENR_UVW_NOISE_LEVEL:
			{
				material->m_map_temp->m_uvw_noise_level = Parsing_NumberFloat();
			}
			break;

			case TOKENR_UVW_NOISE_PHASE:
			{
				material->m_map_temp->m_uvw_noise_phase = Parsing_NumberFloat();
			}
			break;

			case TOKENR_BITMAP_FILTER:
			{
				material->m_map_temp->m_bitmap_filter = Parsing_NumberLong();
			}
			break;

		} // switch()

	} // while()
}

void ASE::ASEParser::Parsing_Dummy()
{
	int _braceDepth = 0;

	// 해당하는 토큰과 첫 번째 중괄호를 읽고 depth를 늘려 놓는다.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// 뭔가 잘못 된 것이다.
		TRACE("파싱 중 뭔가 잘못됨");
		return;
	}
	else
	{
		// 첫 번째 중괄호를 읽었으므로 뎁스 증가
		_braceDepth++;
	}

	LONG _nowToken;

	while (_braceDepth != 0)
	{
		_nowToken = m_lexer->GetToken(m_TokenString);

		switch (_nowToken)
		{
			case TOKEND_BLOCK_START:
			{
				_braceDepth++;
			}
			break;

			case TOKEND_BLOCK_END:
			{
				_braceDepth--;
			}
			break;

			default:
			{
				// 처리하지 않은 토큰이나, 다른 무엇인가가 들어왔을 때
			}
			break;

		} // switch()

	} // while()
}

///--------------------------------------------------
/// Parsing을 위한 단위별 함수들

// long을 읽어서 리턴해준다.
int ASE::ASEParser::Parsing_NumberLong()
{
	LONG _token;
	LONG _numberLong;

	_token = m_lexer->GetToken(m_TokenString);
	_numberLong = strtoul(m_TokenString, NULL, 10);

	return _numberLong;
}

// float를 읽어서 리턴해준다.
float ASE::ASEParser::Parsing_NumberFloat()
{
	LONG _token;
	float numberFloat;

	_token = m_lexer->GetToken(m_TokenString);
	numberFloat = (float)atof(m_TokenString);

	return numberFloat;
}

// String을 읽어서 리턴해준다.
LPSTR ASE::ASEParser::Parsing_String()
{
	// m_TokenString - char[255]
	m_lexer->GetToken(m_TokenString);

	return m_TokenString;
}

// int를 읽어서 리턴해준다.
int ASE::ASEParser::Parsing_NumberInt()
{
	LONG _token;
	int numberInt;

	_token = m_lexer->GetToken(m_TokenString);
	numberInt = (int)atoi(m_TokenString);

	return numberInt;
}

Vector3 ASE::ASEParser::Parsing_NumberVector3()
{
	Vector3 _vector3;

	m_lexer->GetToken(m_TokenString);
	_vector3.x = (float)atof(m_TokenString);

	m_lexer->GetToken(m_TokenString);
	_vector3.y = (float)atof(m_TokenString);

	m_lexer->GetToken(m_TokenString);
	_vector3.z = (float)atof(m_TokenString);

	return _vector3;
}

// 3개의 Float를 벡터 하나로 리턴해준다.
// (Max와 DX의 좌표차이로 yz변환)
Vector3 ASE::ASEParser::Parsing_NumberVector3_YZ()
{
	Vector3 _vector3;

	m_lexer->GetToken(m_TokenString);
	_vector3.x = (float)atof(m_TokenString);

	m_lexer->GetToken(m_TokenString);
	_vector3.z = (float)atof(m_TokenString);

	m_lexer->GetToken(m_TokenString);
	_vector3.y = (float)atof(m_TokenString);

	return _vector3;
}






