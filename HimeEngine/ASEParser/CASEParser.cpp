#include "CASEParser.h"


CASEParser::CASEParser()
	: m_TokenString(), m_Token(0),
	m_data_asciiexport(0), m_scenedata(),
	m_materialcount(0), m_materialdata(nullptr), m_list_materialdata(),
	m_OneMesh(nullptr), m_MeshList(),
	m_animation(nullptr), m_list_animation(),
	m_lexer(nullptr)
{
	m_materialcount = 0;
	m_parsingmode = eNone;
}

CASEParser::~CASEParser()
{
	delete m_lexer;
}

bool CASEParser::Init()
{
	m_lexer = new ASE::CASELexer;

	return TRUE;
}
//---------------------------------------------------------------------------------------------------
// 로딩한다.
// 이것이 끝났다면 정해진 데이터형에 모든 데이터를 읽어서 들어가 있어야 한다.
//
// Parsing에 대해:
// 항상 느끼는 것이지만 parsing이라는 것은 데이터가 일정 규격에 맞게 들어가 있다는 것을 전제로 한다.
// 하지만, 파일 내부에 순서가 바뀌어 들어가 있는것이 가능하다던지 하는 규칙이 생기면
// 검색하는 루틴이 복잡해지기 마련. 일단은 순서대로 일정 규약으로 들어가 있다는것을 가정하자.
// -> 그래서, 재귀 호출을 하는 하나의 큰 함수로 해결봤다.
// -> depth를 기준으로 오동작에 대한 안전 코드를 넣어야겠다
//---------------------------------------------------------------------------------------------------
bool CASEParser::Load(LPSTR p_File)
{
	/// 0) 파일을 로드한다.
	if (!m_lexer->Open(p_File))
	{
		TRACE("파일을 여는 중에 문제가 발생했습니다!");
		return FALSE;
	}

	/// 1) 다롱이아부지김교수의 파워풀한 재귀함수 한개로 처리 끝!
	Parsing_DivergeRecursiveALL(0);

	return TRUE;
}

/// 1차 변환

// CScenedata 복사
bool CASEParser::TranslateToD3DFormat_scene(Scenedata* pscene)
{
	// 값 복사
	*pscene = this->m_scenedata;
	return TRUE;
}

/// <summary>
/// 메시의 최적화를 해 준다.
/// 노말값, 텍스쳐 좌표에 따라 버텍스를 늘리고, 중첩되는것은 제거하고..
/// </summary>
bool CASEParser::Convert_Optimize(GeomObject* pMesh)
{
	bool _isNewVertex = true;

	// 버텍스들은 일단 모두 복사
	for (unsigned int i = 0; i < pMesh->m_meshvertex.size(); i++)
	{
		Vertex* _pVertex = new Vertex;
		_pVertex->m_pos = pMesh->m_meshvertex[i]->m_pos;

		pMesh->m_opt_vertex.push_back(_pVertex);
	}

	// 버텍스 쪼개기 할때 비교하기 위해 만듦
	int _size = pMesh->m_mesh_numvertex;
	pMesh->m_normalList.resize(_size);

	// 나머지는 face를 기준으로 한 인덱스로 찾아야 함
	unsigned int _faceCount = pMesh->m_meshface.size();
	//TRACE("FaceCount : %d", _faceCount);

	// 일단 face를 이루는 Vertex에 normal값을 넣어주자.
	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		// vertex에 3개의 normal를 들고 있기 때문에 그 값을 하나하나 넣어준다. 
		for (int j = 0; j < 3; j++)
		{
			// Face를 이루는 vertex를 가지고와서 Normal을 넣어준다. 
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// vertex normal이 비어있으면 무조건 넣는다. 
			// vertex normal를 비교한다. 
			// 같으면 아무것도 안함. 
			// 다르면 vertex를 생성해서 m_opt_vertex에 넣어주자.
			// vertex 인덱스가 늘어남. 그러므로 pMesh->m_opt_index[i].index[j]에 추가된 애들을 넣어줘야함. 
			if (_nowVertex->m_normal.x == 0 && _nowVertex->m_normal.y == 0 && _nowVertex->m_normal.z == 0)
			{
				_nowVertex->m_normal.x = pMesh->m_meshface[i]->m_normalvertex[j].x;
				_nowVertex->m_normal.y = pMesh->m_meshface[i]->m_normalvertex[j].y;
				_nowVertex->m_normal.z = pMesh->m_meshface[i]->m_normalvertex[j].z;
				// m_normalList에 현재 Vertex에 있는 노말값들을 그대로 복사해온다. 
				int _tempVertexIndex = pMesh->m_meshface[i]->m_vertexindex[j];
				Vector3 _tempNormal = pMesh->m_meshface[i]->m_normalvertex[j];

				pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].push_back(make_pair(_tempVertexIndex, _tempNormal));
			}
			else
			{
				_isNewVertex = true;

				// 기존에 만든 벡터와 겹치는게 없는지 검사 
				for (int _index = 0; _index < pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].size(); _index++)
				{
					if (_index >= pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].size())break;
					if (pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].at(_index).second == pMesh->m_meshface[i]->m_normalvertex[j])
					{
						_isNewVertex = false;

						// 이미 같은 vertexNoraml 값이 있다면 face를 이루는 vertexIndex의 값을 바꿔주자. 
						pMesh->m_meshface[i]->m_vertexindex[j] = pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].at(_index).first;
						break;
					}
				}

				if (_isNewVertex)
				{
					// vertex 생성. 
					Vertex* _newVertex = new Vertex();

					// 이미 존재하는 버텍스의 인덱스를 갖고와서 포지션을 넣어주자. 
					int vertexIndex = pMesh->m_meshface[i]->m_vertexindex[j];
					_newVertex->m_pos = pMesh->m_meshvertex[vertexIndex]->m_pos;

					pMesh->m_opt_vertex.push_back(_newVertex);

					// 새로 생성된 vertex 에 normal값을 넣어준다. 
					_newVertex->m_normal.x = pMesh->m_meshface[i]->m_normalvertex[j].x;
					_newVertex->m_normal.y = pMesh->m_meshface[i]->m_normalvertex[j].y;
					_newVertex->m_normal.z = pMesh->m_meshface[i]->m_normalvertex[j].z;

					// 새로 생긴 vertex의 번호를 넣어준다. 
					int _tempVertexIndex = pMesh->m_opt_vertex.size() - 1;
					Vector3 _tempNormal = pMesh->m_meshface[i]->m_normalvertex[j];

					pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].push_back(make_pair(_tempVertexIndex, _tempNormal));

					pMesh->m_meshface[i]->m_vertexindex[j] = pMesh->m_opt_vertex.size() - 1;
				}
			}
		}
	}

	// 위에 vertex Normal를 넣은 것을 indexbuffer에 넣어줘야 한다. 
	// 인덱스는 그냥 복사
	pMesh->m_opt_index = new IndexList[pMesh->m_meshface.size()];

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pMesh->m_opt_index[i].index[j] = pMesh->m_meshface[i]->m_vertexindex[j];
		}
	}


	/// 이제 텍스처를 시작하자. 
	_isNewVertex = false;

	// 텍스처 쪼개기 할 때 비교하기 위해 만듦
	vector<vector<pair<int, Vector3>>> _textureList;
	int _textureSize = pMesh->m_mesh_numtvertex;
	_textureList.resize(_textureSize);

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		// 한 개의 face에 3개의 정점이 들어있다. 
		for (int j = 0; j < 3; j++)
		{
			// Face를 이루는 vertex를 가지고와서 텍스처 좌표를 넣어준다. 
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// vertex 텍스처가 비어있으면 무조건 넣는다. 
			// vertex 텍스처를 비교해서 같으면 아무것도 안함. 
			// 다르면 vertex를 생성해서 m_opt_vertex에 넣어주자.
			// vertex 인덱스가 늘어남. 그러므로 pMesh->m_opt_index[i].index[j]에 추가된 애들을 넣어줘야함. 
			if (_nowVertex->u == 0 && _nowVertex->v == 0)
			{
				// vertex에 texture 값을 넣어주자.
				int _TvertexIndex = pMesh->m_meshface[i]->m_TFace[j];
				_nowVertex->u = pMesh->m_mesh_tvertex[_TvertexIndex]->m_u;
				_nowVertex->v = pMesh->m_mesh_tvertex[_TvertexIndex]->m_v;
				_nowVertex->m_istextureset = true;

				Vector3 _uv;
				_uv.x = _nowVertex->u;
				_uv.y = _nowVertex->v;
				_uv.z = 0.f;

				_textureList[_TvertexIndex].push_back(make_pair(_TvertexIndex, _uv));
			}
			else
			{
				_isNewVertex = true;

				// 이럴 때 지역변수를 쓰면 매우 편함
				/// 한 개의 vertex의 Index에 들어갈 수 있는 텍스쳐 좌표들의 모임
				vector<pair<int, Vector3>> _tVertices = _textureList[pMesh->m_meshface[i]->m_TFace[j]];

				// 기존에 만든 벡터와 겹치는게 없는지 검사 
				for (int _index = 0; _index < _tVertices.size(); _index++)
				{
					if (_index >= _tVertices.size()) break;

					// 모임들에서, n번째 인덱스에 들어있는 UV값
					Vector3 _uvw = _tVertices.at(_index).second;

					// 텍스쳐의 좌표가 같으면...
					if (_uvw.x == _nowVertex->u && _uvw.y == _nowVertex->v)
					{
						_isNewVertex = false;

						// 같은 텍스쳐 값을 가지는 버텍스가 있으므로 그 인덱스로 바꿔준다.
						pMesh->m_meshface[i]->m_vertexindex[j] = pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].at(_index).first;

						//TRACE("같은 버텍스가 이미 있습니다! %d\n", pMesh->m_meshface[i]->m_vertexindex[j]);
						break;
					}
				}

				if (_isNewVertex)
				{
					// vertex 생성. 
					Vertex* _newVertex = new Vertex();

					// 이미 존재하는 버텍스의 인덱스를 갖고와서 포지션을 넣어주자. 
					int vertexIndex = pMesh->m_meshface[i]->m_vertexindex[j];
					_newVertex->m_pos = pMesh->m_opt_vertex[vertexIndex]->m_pos;
					_newVertex->m_normal = pMesh->m_opt_vertex[vertexIndex]->m_normal;

					// vertex에 texture 값을 넣어주자.
					int _TvertexIndex = pMesh->m_meshface[i]->m_TFace[j];
					_newVertex->u = pMesh->m_mesh_tvertex[_TvertexIndex]->m_u;
					_newVertex->v = pMesh->m_mesh_tvertex[_TvertexIndex]->m_v;
					_newVertex->m_istextureset = true;

					// 새로 추가가 되었다는 것을 알리기 위해서 특정 자료구조에 보관한다.
					Vector3 _uv;
					_uv.x = _newVertex->u;
					_uv.y = _newVertex->v;
					_uv.z = 0.f;
					_textureList[_TvertexIndex].push_back(make_pair(_TvertexIndex, _uv));

					// 새로 버텍스를 추가
					pMesh->m_opt_vertex.push_back(_newVertex);

					// 추가가 되었으므로 버텍스의 인덱스가 달라졌을 것이고, 그것을 업데이트 해 준다.
					pMesh->m_meshface[i]->m_vertexindex[j] = pMesh->m_opt_vertex.size() - 1;

					//TRACE("버텍스를 새로 추가했습니다.! %d\n", pMesh->m_meshface[i]->m_vertexindex[j]);
				}
			}
		}
	}

	// 위에 vertex Normal를 넣은 것을 indexbuffer에 넣어줘야 한다. 
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

bool CASEParser::ConvertAll(GeomObject* pMesh)
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
	//TRACE("FaceCount : %d", _faceCount);

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		for (int j = 0; j < 3; j++)
		{
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// face기준으로 버텍스의 노말을 넣어준다.
			// (예시용)
			_nowVertex->m_normal.x = 0;
			_nowVertex->m_normal.y = 0;
			_nowVertex->m_normal.z = 0;
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

/// <summary>
/// 메시의 최적화를 해 준다.
/// 노말값, 텍스쳐 좌표에 따라 버텍스를 늘리고, 중첩되는것은 제거하고..
/// </summary>
bool CASEParser::Convert_NewOptimize(GeomObject* pMesh)
{
	bool _result = false;

	// 1. 기본적인 포지션 정보를 넣어준다.
	_result = Convert_NewOptimize_Position(pMesh);

	// 2. 노말 데이터를 최적화한다.
	_result = Convert_NewOptimize_Normal(pMesh);

	// 3. 텍스쳐 데이터를 최적화한다.
	_result = Convert_NewOptimize_Texture(pMesh);

	return _result;
}

void CASEParser::ClearASEParser()
{
	if (this->m_list_materialdata.size() != 0)
	{
		for (auto _nowMat : m_list_materialdata)
		{
			if (_nowMat->m_map_diffuse != nullptr)
			{
				delete _nowMat->m_map_diffuse;
				_nowMat->m_map_diffuse = nullptr;
			}

			delete _nowMat;
		}
		this->m_list_materialdata.clear();
	}
	m_materialdata = nullptr;

	if (this->m_MeshList.size() != 0)
	{
		for (auto _obj : m_MeshList)
		{
			if (_obj->m_vector_wvertexs.size() != 0)
			{
				for (auto _wvertex : _obj->m_vector_wvertexs)
				{
					delete _wvertex;
				}
			}

			if (_obj->m_vector_bone_list.size() != 0)
			{
				for (auto _bone : _obj->m_vector_bone_list)
				{
					delete _bone;
				}
			}

			if (_obj->m_list_lightanimation.size() != 0)
			{
				for (auto _lightanimation : _obj->m_list_lightanimation)
				{
					delete _lightanimation;
				}
			}

			if (_obj->m_vector_shape_line.size() != 0)
			{
				for (auto _line : _obj->m_vector_shape_line)
				{
					delete _line;
				}
			}

			if (_obj->m_opt_vertex.size() != 0)
			{
				for (auto _vertex : _obj->m_opt_vertex)
				{
					delete _vertex;
				}
			}

			if (_obj->m_mesh_tvertex.size() != 0)
			{
				for (auto _tvertex : _obj->m_mesh_tvertex)
				{
					delete _tvertex;
				}
			}

			if (_obj->m_meshface.size() != 0)
			{
				for (auto _face : _obj->m_meshface)
				{
					delete _face;
				}
			}

			if (_obj->m_meshvertex.size() != 0)
			{
				for (auto _vectex : _obj->m_meshvertex)
				{
					delete _vectex;
				}
			}

			delete _obj;
		}
		this->m_MeshList.clear();
	}
	m_OneMesh = nullptr;

	if (this->m_list_animation.size() != 0)
	{
		for (auto _ani : m_list_animation)
		{
			delete _ani;
		}

		this->m_list_animation.clear();
	}
	m_animation = nullptr;
}

// 1. 기본적인 포지션 정보를 넣어준다.
bool CASEParser::Convert_NewOptimize_Position(GeomObject* pMesh)
{
	// 버텍스들은 일단 모두 복사
	for (size_t i = 0; i < pMesh->m_meshvertex.size(); i++)
	{
		Vertex* _pVertex = new Vertex;
		_pVertex->m_pos = pMesh->m_meshvertex[i]->m_pos;

		// 스키니오브젝트라면 wvertex값을 넣어주자. 
		if (Insert_SkinnedObject_Weight(pMesh, i) == true)
		{
			_pVertex->m_bw1 = pMesh->m_meshvertex[i]->m_bw1;
			_pVertex->m_bw2 = pMesh->m_meshvertex[i]->m_bw2;
			_pVertex->m_bw3 = pMesh->m_meshvertex[i]->m_bw3;
			_pVertex->m_bw4 = pMesh->m_meshvertex[i]->m_bw4;
			_pVertex->m_bw5 = pMesh->m_meshvertex[i]->m_bw5;
			_pVertex->m_bw6 = pMesh->m_meshvertex[i]->m_bw6;
			_pVertex->m_bw7 = pMesh->m_meshvertex[i]->m_bw7;
			_pVertex->m_bw8 = pMesh->m_meshvertex[i]->m_bw8;
			_pVertex->m_bw9 = pMesh->m_meshvertex[i]->m_bw9;
		}
		
		pMesh->m_opt_vertex.push_back(_pVertex);
	}

	return true;
}

// 2. 노말 데이터를 최적화한다.
bool CASEParser::Convert_NewOptimize_Normal(GeomObject* pGeomObject)
{
	/// 네거티브 스케일을 검사하자. 
	// 1. nodeTM에서 _scrale값을 뽑아내자.(decompose를 이용하면 됨.)
	XMVECTOR _pos;
	XMVECTOR _rot;
	XMVECTOR _scale;

	XMMATRIX _nodeTM = XMLoadFloat4x4(&pGeomObject->m_nodeTM);

	// decompose LoclaTM
	XMMatrixDecompose(&_scale, &_rot, &_pos, _nodeTM);

	XMVECTOR _determinant = XMMatrixDeterminant(_nodeTM);

	// negative scale인지 아닌지 판별하기 위한 변수 
	/*bool isNegative = false;

	// negative scale 이란? 
	// 원소중에 하나라도 음수가 있을때가 아니라
	// 한개 또는 3개가 음수일 때만.

	// 2. 뽑아 낸 _scale에 음수 값을 들고 있다면 negative scale이다. 
	if (_determinant.m128_f32[0] < 0)
	{
		isNegative = true;
	}
	
	// 3. negative scale이라면 -1를 곱해주자. 
	if (isNegative)
	{ 
		_scale = -1 * _scale;

		// -1를 곱한 _scale값을 nodeTM으로 다시 만들어 주자. 
		_nodeTM = XMMatrixScaling(_scale.m128_f32[0], _scale.m128_f32[1], _scale.m128_f32[2])
			* XMMatrixRotationQuaternion(_rot) 
			* XMMatrixTranslation(_pos.m128_f32[0], _pos.m128_f32[1], _pos.m128_f32[2]);

		XMStoreFloat4x4(&pGeomObject->m_nodeTM, _nodeTM);

		for (auto& _face : pGeomObject->m_meshface)
		{
			_face->m_normalvertex[0] *= -1;
			_face->m_normalvertex[1] *= -1;
			_face->m_normalvertex[2] *= -1;
		}
	}*/

	// 버텍스 쪼개기 할때 비교하기 위해 만듦
	pGeomObject->m_normalList.resize((size_t)pGeomObject->m_mesh_numvertex);

	// 나머지는 face를 기준으로 한 인덱스로 찾아야 함
	unsigned int _faceCount = pGeomObject->m_meshface.size();
	//TRACE("FaceCount : %d", _faceCount);

	bool _isNewVertex = true;

	// 일단 face를 이루는 Vertex에 normal값을 넣어주자.
	for (unsigned int i = 0; i < pGeomObject->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		// vertex에 3개의 normal를 들고 있기 때문에 그 값을 하나하나 넣어준다. 
		for (int j = 0; j < 3; j++)
		{
			// Face를 이루는 vertex를 가지고와서 Normal을 넣어준다. 
			int _nowVertexIndex = pGeomObject->m_meshface[i]->m_vertexindex[j];
			Vertex* _nowVertex = pGeomObject->m_opt_vertex[_nowVertexIndex];

			// vertex normal이 비어있으면 무조건 넣는다. 
			// vertex normal를 비교한다. 
			// 같으면 아무것도 안함. 
			// 다르면 vertex를 생성해서 m_opt_vertex에 넣어주자.
			// vertex 인덱스가 늘어남. 그러므로 pMesh->m_opt_index[i].index[j]에 추가된 애들을 넣어줘야함. 
			if (_nowVertex->m_normal.x == 0 && _nowVertex->m_normal.y == 0 && _nowVertex->m_normal.z == 0)
			{
				_nowVertex->m_normal.x = pGeomObject->m_meshface[i]->m_normalvertex[j].x;
				_nowVertex->m_normal.y = pGeomObject->m_meshface[i]->m_normalvertex[j].y;
				_nowVertex->m_normal.z = pGeomObject->m_meshface[i]->m_normalvertex[j].z;

				// m_normalList에 현재 Vertex에 있는 노말값들을 그대로 복사해온다. 
				///int _newVertexIndex = pMesh->m_meshface[i]->m_vertexindex[j];
				int _newVertexIndex = _nowVertexIndex;
				Vector3 _newNormal = pGeomObject->m_meshface[i]->m_normalvertex[j];

				pGeomObject->m_normalList[_nowVertexIndex].push_back(make_pair(_newVertexIndex, _newNormal));
			}
			else
			{
				_isNewVertex = true;

				// 기존에 만든 벡터와 겹치는게 없는지 검사 
				for (int _index = 0; _index < pGeomObject->m_normalList[_nowVertexIndex].size(); _index++)
				{
					if (pGeomObject->m_normalList[_nowVertexIndex].at(_index).second == pGeomObject->m_meshface[i]->m_normalvertex[j])
					{
						_isNewVertex = false;

						// 이미 같은 vertexNoraml 값이 있다면 face를 이루는 vertexIndex의 값을 바꿔주자. 
						pGeomObject->m_meshface[i]->m_vertexindex[j] = pGeomObject->m_normalList[_nowVertexIndex].at(_index).first;
						break;
					}
				}

				// 새로운 버텍스를 생성해서 추가
				if (_isNewVertex)
				{
					Vertex* _newVertex = new Vertex();

					// 이미 존재하는 버텍스의 인덱스를 갖고와서 포지션을 넣어주자. 
					int _vertexIndex = pGeomObject->m_meshface[i]->m_vertexindex[j];
					_newVertex->m_pos = pGeomObject->m_meshvertex[_vertexIndex]->m_pos;

					pGeomObject->m_opt_vertex.push_back(_newVertex);

					// 새로 생성된 vertex 에 normal값을 넣어준다. 
					_newVertex->m_normal.x = pGeomObject->m_meshface[i]->m_normalvertex[j].x;
					_newVertex->m_normal.y = pGeomObject->m_meshface[i]->m_normalvertex[j].y;
					_newVertex->m_normal.z = pGeomObject->m_meshface[i]->m_normalvertex[j].z;

					// 새로 생성된 vertex 에 wvertex값을 넣어준다. 
					_newVertex->m_bw1 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw1;
					_newVertex->m_bw2 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw2;
					_newVertex->m_bw3 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw3;
					_newVertex->m_bw4 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw4;
					_newVertex->m_bw5 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw5;
					_newVertex->m_bw6 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw6;
					_newVertex->m_bw7 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw7;
					_newVertex->m_bw8 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw8;
					_newVertex->m_bw9 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw9;

					// 새로 생긴 vertex의 번호를 넣어준다. 
					int _newVertexIndex = pGeomObject->m_opt_vertex.size() - 1;
					Vector3 _newNormal = pGeomObject->m_meshface[i]->m_normalvertex[j];

					pGeomObject->m_normalList[_vertexIndex].push_back(make_pair(_newVertexIndex, _newNormal));

					pGeomObject->m_meshface[i]->m_vertexindex[j] = pGeomObject->m_opt_vertex.size() - 1;
				}
			}
		}
	}

	// 위에 vertex Normal를 넣은 것을 indexbuffer에 넣어줘야 한다. 
	ApplyIndicesFromFaces(pGeomObject);

	return true;
}

// 이제 텍스처를 시작하자. 
bool CASEParser::Convert_NewOptimize_Texture(GeomObject* pMesh)
{
	bool _isNewVertex = false;

	// 텍스처 쪼개기 할 때 비교하기 위해 만듦
	vector<vector<pair<int, Vector3>>> _textureList;
	int _textureSize = pMesh->m_mesh_numtvertex;
	_textureList.resize(_textureSize);

	if (_textureSize == 0) return false;

	// 나머지는 face를 기준으로 한 인덱스로 찾아야 함
	unsigned int _faceCount = pMesh->m_meshface.size();
	//TRACE("FaceCount : %d", _faceCount);

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		// 한 개의 face에 3개의 정점이 들어있다. 
		for (int j = 0; j < 3; j++)
		{
			// Face를 이루는 vertex를 가지고와서 텍스처 좌표를 넣어준다. 
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// vertex 텍스처가 비어있으면 무조건 넣는다. 
			// vertex 텍스처를 비교해서 같으면 아무것도 안함. 
			// 다르면 vertex를 생성해서 m_opt_vertex에 넣어주자.
			// vertex 인덱스가 늘어남. 그러므로 pMesh->m_opt_index[i].index[j]에 추가된 애들을 넣어줘야함. 
			if (_nowVertex->u == 0 && _nowVertex->v == 0)
			{
				// vertex에 texture 값을 넣어주자.
				int _TvertexIndex = pMesh->m_meshface[i]->m_TFace[j];
				_nowVertex->u = pMesh->m_mesh_tvertex[_TvertexIndex]->m_u;
				_nowVertex->v = pMesh->m_mesh_tvertex[_TvertexIndex]->m_v;
				_nowVertex->m_istextureset = true;

				Vector3 _uv(_nowVertex->u, _nowVertex->v, 0.f);
				_textureList[_TvertexIndex].push_back(make_pair(_TvertexIndex, _uv));
			}
			else
			{
				_isNewVertex = true;

				// 이럴 때 지역변수를 쓰면 매우 편함
				/// 한 개의 vertex의 Index에 들어갈 수 있는 텍스쳐 좌표들의 모임
				vector<pair<int, Vector3>> _tVertices = _textureList[pMesh->m_meshface[i]->m_TFace[j]];

				// 기존에 만든 벡터와 겹치는게 없는지 검사 
				for (int _index = 0; _index < _tVertices.size(); _index++)
				{
					// 모임들에서, n번째 인덱스에 들어있는 UV값
					Vector3 _uvw = _tVertices.at(_index).second;

					// 텍스쳐의 좌표가 같으면...
					if (_uvw.x == _nowVertex->u && _uvw.y == _nowVertex->v)
					{
						_isNewVertex = false;

						// 같은 텍스쳐 값을 가지는 버텍스가 있으므로 그 인덱스로 바꿔준다.
						//Vertex* index = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];
						//pMesh->m_meshface[i]->m_vertexindex[j] = pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].at(_index).first;

						//TRACE("같은 버텍스가 이미 있습니다! %d\n", pMesh->m_meshface[i]->m_vertexindex[j]);
						break;
					}
				}

				// 새로운 버텍스를 생성해서 추가
				if (_isNewVertex)
				{
					Vertex* _newVertex = new Vertex();

					// 이미 존재하는 버텍스의 인덱스를 갖고와서 포지션을 넣어주자. 
					int vertexIndex = pMesh->m_meshface[i]->m_vertexindex[j];
					_newVertex->m_pos = pMesh->m_opt_vertex[vertexIndex]->m_pos;
					_newVertex->m_normal = pMesh->m_opt_vertex[vertexIndex]->m_normal;

					// vertex에 texture 값을 넣어주자.
					int _TvertexIndex = pMesh->m_meshface[i]->m_TFace[j];
					_newVertex->u = pMesh->m_mesh_tvertex[_TvertexIndex]->m_u;
					_newVertex->v = pMesh->m_mesh_tvertex[_TvertexIndex]->m_v;
					_newVertex->m_istextureset = true;

					// 새로 생성된 vertex 에 wvertex값을 넣어준다. 
					_newVertex->m_bw1 = pMesh->m_opt_vertex[vertexIndex]->m_bw1;
					_newVertex->m_bw2 = pMesh->m_opt_vertex[vertexIndex]->m_bw2;
					_newVertex->m_bw3 = pMesh->m_opt_vertex[vertexIndex]->m_bw3;
					_newVertex->m_bw4 = pMesh->m_opt_vertex[vertexIndex]->m_bw4;
					_newVertex->m_bw5 = pMesh->m_opt_vertex[vertexIndex]->m_bw5;
					_newVertex->m_bw6 = pMesh->m_opt_vertex[vertexIndex]->m_bw6;
					_newVertex->m_bw7 = pMesh->m_opt_vertex[vertexIndex]->m_bw7;
					_newVertex->m_bw8 = pMesh->m_opt_vertex[vertexIndex]->m_bw8;
					_newVertex->m_bw9 = pMesh->m_opt_vertex[vertexIndex]->m_bw9;

					// 새로 추가가 되었다는 것을 알리기 위해서 특정 자료구조에 보관한다.
					Vector3 _uv(_newVertex->u, _newVertex->v, 0.f);
					_textureList[_TvertexIndex].push_back(make_pair(_TvertexIndex, _uv));

					// 새로 버텍스를 추가
					pMesh->m_opt_vertex.push_back(_newVertex);

					// 추가가 되었으므로 버텍스의 인덱스가 달라졌을 것이고, 그것을 업데이트 해 준다.
					pMesh->m_meshface[i]->m_vertexindex[j] = pMesh->m_opt_vertex.size() - 1;

					//TRACE("버텍스를 새로 추가했습니다.! %d\n", pMesh->m_meshface[i]->m_vertexindex[j]);
				}
			}
		}
	}

	// 위에 텍스쳐 정보를 넣은 버텍스의 인덱스 정보를 indexbuffer에 넣어줘야 한다. 
	ApplyIndicesFromFaces(pMesh);

	return true;
}

bool CASEParser::Insert_SkinnedObject_Weight(GeomObject* pMesh, int vertexNum)
{
	if (pMesh->m_vector_bone_list.size() != 0)
	{
		VertexWeight* _wvertex = pMesh->m_vector_wvertexs[vertexNum];

		// 가중치와 본의 정보를 저장
		for (size_t index = 0; index < _wvertex->m_bone_blending_weight.size(); index++)
		{
			if (index == 0)
			{
				pMesh->m_meshvertex[vertexNum]->m_bw1 = { _wvertex->m_bone_blending_weight[index]->m_bone_number,
			   _wvertex->m_bone_blending_weight[index]->m_bone_weight };
			}

			if (index == 1)
			{
				pMesh->m_meshvertex[vertexNum]->m_bw2 = { _wvertex->m_bone_blending_weight[index]->m_bone_number,
			   _wvertex->m_bone_blending_weight[index]->m_bone_weight };
			}

			if (index == 2)
			{
				pMesh->m_meshvertex[vertexNum]->m_bw3 = { _wvertex->m_bone_blending_weight[index]->m_bone_number,
			   _wvertex->m_bone_blending_weight[index]->m_bone_weight };
			}

			if (index == 3)
			{
				pMesh->m_meshvertex[vertexNum]->m_bw4 = { _wvertex->m_bone_blending_weight[index]->m_bone_number,
			   _wvertex->m_bone_blending_weight[index]->m_bone_weight };
			}

			if (index == 4)
			{
				pMesh->m_meshvertex[vertexNum]->m_bw5 = { _wvertex->m_bone_blending_weight[index]->m_bone_number,
			   _wvertex->m_bone_blending_weight[index]->m_bone_weight };
			}

			if (index == 5)
			{
				pMesh->m_meshvertex[vertexNum]->m_bw6 = { _wvertex->m_bone_blending_weight[index]->m_bone_number,
			   _wvertex->m_bone_blending_weight[index]->m_bone_weight };
			}

			if (index == 6)
			{
				pMesh->m_meshvertex[vertexNum]->m_bw7 = { _wvertex->m_bone_blending_weight[index]->m_bone_number,
			   _wvertex->m_bone_blending_weight[index]->m_bone_weight };
			}

			if (index == 7)
			{
				pMesh->m_meshvertex[vertexNum]->m_bw8 = { _wvertex->m_bone_blending_weight[index]->m_bone_number,
			   _wvertex->m_bone_blending_weight[index]->m_bone_weight };
			}

			if (index == 8)
			{
				pMesh->m_meshvertex[vertexNum]->m_bw9 = { _wvertex->m_bone_blending_weight[index]->m_bone_number,
			   _wvertex->m_bone_blending_weight[index]->m_bone_weight };
			}
		}
		
		return true;
	}

	return false;
}


// 버텍스 버퍼가 변경되었으므로 indexbuffer도 갱신되어야 한다. 
void CASEParser::ApplyIndicesFromFaces(GeomObject* pMesh)
{
	pMesh->m_opt_index = new IndexList[pMesh->m_meshface.size()];

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pMesh->m_opt_index[i].index[j] = pMesh->m_meshface[i]->m_vertexindex[j];
		}
	}
}

ASEParser::GeomObject* CASEParser::GetMesh(int index)
{
	return m_MeshList[index];
}

//----------------------------------------------------------------
// 재귀 호출됨을 전제로 하는 분기 함수이다.
//
// 일단 읽고, 토큰에 따라 동작한다.
//
// 재귀를 들어가는 조건은 { 을 만날 경우이다.
// 리턴하는 조건은 '}'를 만나거나 TOKEND_END를 만났을 때이다.
//
// 더불어, 재귀를 들어가기 전 토큰을 읽어, 지금 이 함수가 어떤 모드인지를 결정한다.
//
//----------------------------------------------------------------
void CASEParser::Parsing_DivergeRecursiveALL(int depth)
{
	//----------------------------------------------------------------------
	// 지역변수들 선언
	// 재귀 돌리다가 일정 이상 루프를 도는 경우(오류)를 체크하기 위한 변수
	int i = 0;
	// 리스트에 데이터를 넣어야 할 때 쓰이는 리스트 카운터.
	int listcount = 0;		// 매우 C스러운데 다른 해결책이 없을까?
	// 이것은 현 토큰이 어떤 종류인가를 판별해준다.
	// 이것을 멤버 변수로 하고 재귀를 돌리는것은 위험하다? -> 잘못된 재귀방식이다?
	LONG nowtoken;
	//----------------------------------------------------------------------

	Matrix TM1, TM2;
	Quaternion tempQT;
	Quaternion prevQT;
	Quaternion resultQT;

	/// 이것을 하면 한 개의 토큰을 읽고, 그 종류와 내용을 알 수 있다.
	while (nowtoken = m_lexer->GetToken(m_TokenString), nowtoken != TOKEND_BLOCK_END)
	{
		// 일단 한 개의 토큰을 읽고, 그것이 괄호 닫기가 아니라면.
		// 넘어온 토큰에 따라 처리해준다.

		static int iv = 0;

		switch (nowtoken)
		{
		case TOKEND_BLOCK_START:

			Parsing_DivergeRecursiveALL(depth++);
			break;

		case TOKENR_HELPER_CLASS:
			break;

			//--------------------
			// 3DSMAX_ASCIIEXPORT
			//--------------------

		case TOKENR_3DSMAX_ASCIIEXPORT:
			m_data_asciiexport = Parsing_NumberLong();
			break;

			//--------------------
			// COMMENT
			//--------------------

		case TOKENR_COMMENT:
			Parsing_String();	// 그냥 m_TokenString에 읽어버리는 역할 뿐.
			//AfxMessageBox( m_TokenString, NULL, NULL);		/// 임시로 코멘트를 출력해본다
			break;

			//--------------------
			// SCENE
			//--------------------

		case TOKENR_SCENE:
			//
			break;
		case TOKENR_SCENE_FILENAME:
			m_scenedata.m_filename = Parsing_String();		// 일관성 있는 함수의 사용을 위해 String과 Int도 만들어줬다.
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
		{
			m_scenedata.m_scene_background_static.x = Parsing_NumberFloat();
			m_scenedata.m_scene_background_static.y = Parsing_NumberFloat();
			m_scenedata.m_scene_background_static.z = Parsing_NumberFloat();
		}
		break;
		case TOKENR_SCENE_AMBIENT_STATIC:
		{
			m_scenedata.m_scene_ambient_static.x = Parsing_NumberFloat();
			m_scenedata.m_scene_ambient_static.y = Parsing_NumberFloat();
			m_scenedata.m_scene_ambient_static.z = Parsing_NumberFloat();
		}
		break;

		case TOKENR_SCENE_ENVMAP:
		{
			// 그냥 안의 내용을 읽어버린다 }가 나올때까지
			while (nowtoken = m_lexer->GetToken(m_TokenString), nowtoken != TOKEND_BLOCK_END) {
			}
		}
		break;

		//--------------------
		// MATERIAL_LIST
		//--------------------
		case TOKENR_MATERIAL_LIST:
			break;

		case TOKENR_MATERIAL_COUNT:
		{
			m_materialcount = Parsing_NumberInt();
		}
		break;

		case TOKENR_MATERIAL:
		{
			/// material 구조체를 생성
			// material 이 1개 이상일 수 있기 때문에 그걸 관리할 list에 넣어주자.
			m_materialdata = new ASEMaterial();
			m_list_materialdata.push_back(m_materialdata);
		}
		break;

		case TOKENR_MATERIAL_NAME:
		{
			m_materialdata->m_material_name = Parsing_String();
		}
		break;

		case TOKENR_MATERIAL_CLASS:
		{
			m_materialdata->m_material_class = Parsing_String();
		}
		break;

		case TOKENR_MATERIAL_AMBIENT:
		{
			m_materialdata->m_material_ambient = Parsing_NumberRGBVector3();
		}
		break;

		case TOKENR_MATERIAL_DIFFUSE:
		{
			m_materialdata->m_material_diffuse = Parsing_NumberRGBVector3();
		}
		break;

		case TOKENR_MATERIAL_SPECULAR:
		{
			m_materialdata->m_material_specular = Parsing_NumberRGBVector3();
		}
		break;

		case TOKENR_MATERIAL_SHINE:
		{
			m_materialdata->m_material_shine = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_SHINESTRENGTH:
		{
			m_materialdata->m_material_shinestrength = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_TRANSPARENCY:
		{
			m_materialdata->m_material_transparency = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_WIRESIZE:
		{
			m_materialdata->m_material_wiresize = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_SHADING:
		{
			m_materialdata->m_material_shading = Parsing_NumberInt();
		}
		break;

		case TOKENR_MATERIAL_XP_FALLOFF:
		{
			m_materialdata->m_material_xp_falloff = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_SELFILLUM:
		{
			m_materialdata->m_material_selfillum = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_FALLOFF:
		{
			m_materialdata->m_material_falloff = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_XP_TYPE:
		{
			m_materialdata->m_material_xp_type = Parsing_NumberInt();
		}
		break;

		case TOKENR_MAP_DIFFUSE:
		{
			// MAP_DIFFUSE 구조체 읽어오기 시작
			m_materialdata->m_map_diffuse = new MaterialMap();
		}
		break;

		case TOKENR_MAP_SPECULAR:
		{
			// MAP_SPECULAR 구조체 읽어오기 시작
			m_materialdata->m_map_specular = new MaterialMap();
		}
		break;

		case TOKENR_MAP_SHINE: // 반사고아
		{
			// MAP_DIFFUSE 구조체 읽어오기 시작
			m_materialdata->m_map_shine = new MaterialMap();
		}
		break;

		case TOKENR_MAP_NAME:
		{
			m_materialdata->m_map_diffuse->m_map_name = Parsing_String();
		}
		break;

		case TOKENR_MAP_CLASS:
		{
			m_materialdata->m_map_diffuse->m_map_class = Parsing_String();
		}
		break;

		case TOKENR_MAP_SUBNO:
		{
			m_materialdata->m_map_diffuse->m_subno = Parsing_NumberInt();
		}
		break;

		case TOKENR_MAP_AMOUNT:
		{
			m_materialdata->m_map_diffuse->m_map_amount = Parsing_NumberFloat();
		}
		break;

		case TOKENR_BITMAP:
		{
			m_materialdata->m_map_diffuse->m_bitmap = Parsing_String();
		}
		break;

		case TOKENR_MAP_TYPE:
		{
			m_materialdata->m_map_diffuse->m_map_type = Parsing_NumberInt();
		}
		break;

		case TOKENR_UVW_U_OFFSET:
		{
			m_materialdata->m_map_diffuse->m_uvw_u_offset = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_V_OFFSET:
		{
			m_materialdata->m_map_diffuse->m_uvw_v_offset = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_U_TILING:
		{
			m_materialdata->m_map_diffuse->m_u_tiling = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_V_TILING:
		{
			m_materialdata->m_map_diffuse->m_v_tiling = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_ANGLE:
		{
			m_materialdata->m_map_diffuse->m_uvw_angle = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_BLUR:
		{
			m_materialdata->m_map_diffuse->m_uvw_blur = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_BLUR_OFFSET:
		{
			m_materialdata->m_map_diffuse->m_uvw_blur_offset = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_NOUSE_AMT:
		{
			m_materialdata->m_map_diffuse->m_uvw_nouse_amt = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_NOISE_SIZE:
		{
			m_materialdata->m_map_diffuse->m_uvw_noise_size = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_NOISE_LEVEL:
		{
			m_materialdata->m_map_diffuse->m_uvw_noise_level = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_NOISE_PHASE:
		{
			m_materialdata->m_map_diffuse->m_uvw_noise_phase = Parsing_NumberFloat();
		}
		break;

		case TOKENR_BITMAP_FILTER:
		{
			m_materialdata->m_map_diffuse->m_bitmap_filter = Parsing_NumberInt();
		}
		break;

		//--------------------
		// HELPEROBJECT
		//--------------------

		//--------------------
		// GEOMOBJECT
		//--------------------
		
		//--------------------
		// ANIMATION
		//--------------------

		//--------------------
		// SHAPEOBJECT
		//--------------------

		case TOKENR_GROUP:
			//	한 개의 그룹 시작. 이 다음에 이름이 스트링으로 나오기는 하는데.
		break;

		case TOKENR_HELPEROBJECT:
			// 일단 생성하고
			// 오브젝트의 타입 정해줌. 이것에 따라 서로 다른 파싱 모드 발동.
		{
			m_parsingmode = eParsingmode::eHelperObject;
			GeomObject* temp = new GeomObject;
			m_OneMesh = temp;
			m_OneMesh->m_type = eHelperObject;
			m_OneMesh->m_scenedata = m_scenedata;		// 클래스간 값 복사
			m_MeshList.push_back(m_OneMesh);
		}
		break;

		case TOKENR_GEOMOBJECT:
			/// 이 토큰을 만났다는건 새로운 메시가 생겼다는 것이다. 지역 변수로 mesh를 하나 선언, 
			/// 그 포인터를 리스트에 넣고, m_onemesh에 그 포인터를 복사, 그대로 쓰면 될까?
		{
			m_parsingmode = eParsingmode::eGeomobject;

			m_OneMesh = new GeomObject();
			m_OneMesh->m_type = eGeomobject;
			m_MeshList.push_back(m_OneMesh);
		}
		break;

		case TOKENR_TM_ANIMATION: // 애니메이션 블록에 들어왔다면.
		{
			m_parsingmode = eParsingmode::eAnimation;

			// 애니메이션을 생성해준다. 
			Create_animationdata_to_list();
		}
		break;

		case TOKENR_ANIMATION_NAME:
		{
			m_animation->m_nodename = m_OneMesh->m_nodename;
			m_animation->m_typename = Parsing_String();
			m_OneMesh->m_isAnimated = true;
		}
		break;

		case TOKENR_SHAPEOBJECT:
		{
			m_parsingmode = eParsingmode::eShape;

			m_OneMesh = new GeomObject();
			m_OneMesh->m_type = eShape;
			m_MeshList.push_back(m_OneMesh);
		}
		break;
		
		case TOKENR_NODE_NAME:
			// 어쩄든 지금은 오브젝트들을 구별 할 수 있는 유일한 값이다.
			// 모드에 따라 넣어야 할 곳이 다르다.
		{
			switch (m_parsingmode)
			{
			case eParsingmode::eHelperObject:
			{
				m_OneMesh->m_nodename = Parsing_String();
			}
			break;

			case eParsingmode::eGeomobject:
			{
				m_OneMesh->m_nodename = Parsing_String();
			}
			break;

			case eParsingmode::eAnimation:
			{
				m_animation->m_nodename = Parsing_String();
			}
			break;

			case eParsingmode::eShape:
			{
				m_OneMesh->m_nodename = Parsing_String();
			}
			break;

			}
		}
		break;

		case TOKENR_NODE_PARENT:
			// 현 노드의 부모 노드의 정보.
		{
			switch (m_parsingmode)
			{
			case eParsingmode::eHelperObject:
			{
				m_OneMesh->m_nodeparent = Parsing_String();
			}
			break;

			case eParsingmode::eGeomobject:
			{
				m_OneMesh->m_nodeparent = Parsing_String();
			}
			break;
			
			case eParsingmode::eShape:
			{
				m_OneMesh->m_nodeparent = Parsing_String();
			}
			break;
			}
		}
		break;

		/// NODE_TM
		case TOKENR_NODE_TM:
			// m_parsingmode	=	eGeomobject;
			// (NODE_TM으로 진입 후 NODE_NAME이 한번 더 나온다.)
			// (Animation과도 구별을 해야 하기 때문에 이렇게 모드를 적어준다)
			/// 효빈 : NODE_NAME이 한번 더 나오는데
			/// 현재 진행하는 것과 nodeName이 같은지 비교해야 하는 것인가? 
			/// 아니면 이전에 이미 나왔으니 저장할 필요가 없는가?

			/// 게다가,
			// 카메라는 NodeTM이 두번 나온다. 두번째라면 넣지 않는다.
			// 아예 이 재귀에서 리턴을 시키고 싶지만. 중간에 읽는것을 무시해야 하기 때문에...
			//if (m_onemesh->m_camera_isloadTarget) {
			//	'}'이 나올때까지 나오는건 무조건 무시! 뭐 이런거 안되나..
			// 재귀구조의 문제점이다....
		break;

		case TOKENR_INHERIT_POS:
			// 카메라는 NodeTM이 두번 나온다. 두번째라면 넣지 않는다.
			//m_OneMesh->m_inherit_pos = Parsing_NumberVector3();
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject || m_parsingmode == eShape)
			{
				m_OneMesh->m_inherit_pos = Parsing_NumberVector3();
			}
		}
		break;
		
		case TOKENR_INHERIT_ROT:
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject || m_parsingmode == eShape)
			{
				m_OneMesh->m_inherit_rot = Parsing_NumberVector3();
			}
		}
		break;
		
		case TOKENR_INHERIT_SCL:
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject || m_parsingmode == eShape)
			{
				m_OneMesh->m_inherit_rot = Parsing_NumberVector3();
			}
		}
		break;
		
		case TOKENR_TM_ROW0:
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject || m_parsingmode == eShape)
			{
				m_OneMesh->m_tm_row0 = Parsing_NumberVector3();

				// NodeTM 0열에 값을 넣어준다. 
				m_OneMesh->m_nodeTM._11 = m_OneMesh->m_tm_row0.x;
				m_OneMesh->m_nodeTM._12 = m_OneMesh->m_tm_row0.y;
				m_OneMesh->m_nodeTM._13 = m_OneMesh->m_tm_row0.z;
				m_OneMesh->m_nodeTM._14 = 0.f;
			}
		}
		break;
		
		case TOKENR_TM_ROW1:
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject || m_parsingmode == eShape)
			{
				m_OneMesh->m_tm_row1 = Parsing_NumberVector3();

				// NodeTM 0열에 값을 넣어준다. 
				m_OneMesh->m_nodeTM._31 = m_OneMesh->m_tm_row1.x;
				m_OneMesh->m_nodeTM._32 = m_OneMesh->m_tm_row1.y;
				m_OneMesh->m_nodeTM._33 = m_OneMesh->m_tm_row1.z;
				m_OneMesh->m_nodeTM._34 = 0.f;
			}
		}
		break;
		
		case TOKENR_TM_ROW2:
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject || m_parsingmode == eShape)
			{
				m_OneMesh->m_tm_row2 = Parsing_NumberVector3();

				// NodeTM 2열에 값을 넣어준다. 
				m_OneMesh->m_nodeTM._21 = m_OneMesh->m_tm_row2.x;
				m_OneMesh->m_nodeTM._22 = m_OneMesh->m_tm_row2.y;
				m_OneMesh->m_nodeTM._23 = m_OneMesh->m_tm_row2.z;
				m_OneMesh->m_nodeTM._24 = 0.f;
			}
		}
		break;
		
		case TOKENR_TM_ROW3:
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject || m_parsingmode == eShape)
			{
				m_OneMesh->m_tm_row3 = Parsing_NumberVector3();

				// NodeTM 0열에 값을 넣어준다. 
				m_OneMesh->m_nodeTM._41 = m_OneMesh->m_tm_row3.x;
				m_OneMesh->m_nodeTM._42 = m_OneMesh->m_tm_row3.y;
				m_OneMesh->m_nodeTM._43 = m_OneMesh->m_tm_row3.z;
				m_OneMesh->m_nodeTM._44 = 1.f;
			}
		}
		break;
		
		case TOKENR_TM_POS:
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject || m_parsingmode == eShape)
			{
				m_OneMesh->m_tm_pos = Parsing_NumberVector3();
			}
		}
		break;
		
		case TOKENR_TM_ROTAXIS:
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject || m_parsingmode == eShape)
			{
				m_OneMesh->m_tm_rotaxis = Parsing_NumberVector3();
			}
		}
		break;
		
		case TOKENR_TM_ROTANGLE:
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject || m_parsingmode == eShape)
			{
				m_OneMesh->m_tm_rotangle = Parsing_NumberFloat();
			}
		}
		break;
		
		case TOKENR_TM_SCALE:
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject || m_parsingmode == eShape)
			{
				m_OneMesh->m_tm_scale = Parsing_NumberVector3();
			}
		}
		break;
		
		case TOKENR_TM_SCALEAXIS:
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject)
			{
				m_OneMesh->m_tm_scaleaxis = Parsing_NumberVector3();
			}
		}
		break;
		
		case TOKENR_TM_SCALEAXISANG:
		{
			if (m_parsingmode == eGeomobject || m_parsingmode == eHelperObject)
			{
				m_OneMesh->m_tm_scaleaxisang = Parsing_NumberFloat();
			}
		}
		// 현재 카메라 상태였다면 이미 노드를 읽은 것으로 표시해준다.
		break;

		/// ANIMATION	
		case TOKENR_CONTROL_POS_TRACK:	// 	decompose된 좌표를 읽어오길 시작할 것이다.
			break;

		case TOKENR_CONTROL_POS_SAMPLE:
		{
			CAnimation_pos* temp = new CAnimation_pos();  // 값을 담을 임시 변수.

			// keyFrame
			// m_scenedata.m_firstframe * m_scenedata.m_ticksperframe = 프레임이 흐르는 시간
			// 만약 time == 0으로 시작하는게 아니라면, 
			// m_scenedata.m_firstframe * m_scenedata.m_ticksperframe 시간을 빼줘서 0으로 보정한다. 
			int tempTime = Parsing_NumberInt();
			temp->m_time = tempTime - (m_scenedata.m_firstframe * m_scenedata.m_ticksperframe);

			// postion
			temp->m_pos = Parsing_NumberVector3();

			// 임시 변수에 넣은 애니메이션의 postion Vecter에 넣어주자. 
			
			if (temp->m_time >= 0 ||// 보정한 시간의 값이 0보다 작고, 마지막 프레임 시간보다 크면 저장하지 않는다. 
				temp->m_time < (m_scenedata.m_lastframe * m_scenedata.m_ticksperframe) - (m_scenedata.m_firstframe * m_scenedata.m_ticksperframe))
			{
				m_animation->m_position.push_back(temp);
			}
		}
		break;

		case TOKENR_CONTROL_ROT_TRACK: // 이제 회전을 읽어올 준비하자. 
			break;

		case TOKENR_CONTROL_ROT_SAMPLE:
		{
			CAnimation_rot* temp = new CAnimation_rot();

			// keyFrame
			// keyFrame
			int tempTime = Parsing_NumberInt();
			temp->m_time = tempTime - (m_scenedata.m_firstframe * m_scenedata.m_ticksperframe);

			// axis
			temp->m_rot = Parsing_NumberVector3();

			// angle
			temp->m_angle = Parsing_NumberFloat();

			// 쿼터니언 변환!
				// ASE의 축, 각도로 이루어진 값은, 이전 프레임에서 얼마만큼 변했는지에 대한 정보
				// 하지만 우리는 쿼터니언 값 자체를 0 에서 => 해당 값으로 변경해야함.
				// 그래서 쿼터니언의 상대 값을 누적시켜야함 => 절대값을 만들어줌
			tempQT =  XMQuaternionRotationAxis(temp->m_rot, temp->m_angle);
			
			// 상대적인 변량이 없기 때문에 지금 값을 넣어준다.
			if (m_animation->m_rotation.size() == 0)
			{
				temp->m_rotQT_accumulation = tempQT;
			}
			else
			{
				// 이전 쿼터니언 값을 얻어와서 현재 쿼터니언 값에 더해주면 된다..!
				//int _quateindex = m_animation->m_rotation.size() - 1;

				 //prevQT = m_animation->m_rotation[_quateindex]->m_rotQT_accumulation;

				temp->m_rotQT_accumulation = tempQT;
			}

			if (temp->m_time >= 0 || // 보정한 시간의 값이 0보다 작고, 마지막 프레임 시간보다 크면 저장하지 않는다. 
				temp->m_time < (m_scenedata.m_lastframe * m_scenedata.m_ticksperframe) - (m_scenedata.m_firstframe * m_scenedata.m_ticksperframe)) 
			{
				m_animation->m_rotation.push_back(temp);
			}

		}
			break;

		/// MESH
		case TOKENR_MESH:
			break;
		case TOKENR_TIMEVALUE:
			m_OneMesh->m_timevalue = Parsing_NumberFloat();
			break;
		case TOKENR_MESH_NUMBONE:
			// 이게 있다면 이것은 Skinned Mesh라고 단정을 짓는다.
			// 내용 입력
			m_OneMesh->m_numbone = Parsing_NumberInt();
			break;
		case TOKENR_MESH_NUMSKINWEIGHT:
			m_OneMesh->m_numskinweight = Parsing_NumberInt();
			break;
		case TOKENR_MESH_NUMVERTEX:
		{
			m_OneMesh->m_mesh_numvertex = Parsing_NumberInt();
		}
		break;
		case TOKENR_MESH_NUMFACES:
		{
			m_OneMesh->m_mesh_numfaces = Parsing_NumberInt();
		}
		break;

		/// MESH_VERTEX_LIST
		case TOKENR_MESH_VERTEX_LIST:
			//
			// 버텍스의 값들을 집어넣어야 하는데
			// 이미 벡터로 선언이 돼 있으므로 그냥 넣으면 된다.
			break;
		case TOKENR_MESH_VERTEX:
		{// 데이터 입력
			Vertex* _meshVertex = new Vertex();
			_meshVertex->m_indices = Parsing_NumberInt();
			_meshVertex->m_pos = Parsing_NumberVector3();

			m_OneMesh->m_meshvertex.push_back(_meshVertex);
		}
		break;

		/// Bone
		case TOKENR_SKIN_INITTM:
			break;
		case TOKENR_BONE_LIST:  
		{
			// 이제 bone를 읽을 준비를 하자.
			//m_OneMesh->m_is_skinningobject = true;
		}
			break;
		case TOKENR_BONE:
		{
			/// 모드 체인지 해 주고, Bone을 소유하고 있다는 것은 이것은 스키닝 오브젝트라는 것이다.
			// 본 하나를 만들어서 임시 포인터 보관, 벡터에 넣고
			// Bone의 넘버를 읽어 주자
			Bone* _newBone = new Bone();
			m_OneMesh->m_is_skinningobject = true;
			m_OneMesh->m_bone = _newBone;
			m_OneMesh->m_bone->m_bone_number = Parsing_NumberInt();	
			m_OneMesh->m_vector_bone_list.push_back(m_OneMesh->m_bone);
		}
		break;

		//이 다음에 본의 이름을 넣어야 한다. 하지만 {를 한 개 더 열었으므로 임시 포인터 변수로서 보관해야겠지.
		case TOKENR_BONE_NAME:
		{
			// TOKENR_BONE 에서 읽어온 몇번째 본인지 넣어준다. 
			int _BoneIndex = m_OneMesh->m_bone->m_bone_number;
			// mesh에 bone list의 index에 접근하여 이름을 넣어주자. 
			m_OneMesh->m_vector_bone_list[_BoneIndex]->m_bone_name = Parsing_String();
		}
		break;

		case TOKENR_BONE_PROPERTY:
			// 이 다음 ABSOLUTE가 나오기는 하는데, 쓸 일이 없다.
			break;
			// 다음에는 TM_ROW0~3이 나오는데 역시 무시됨..

		case TOKENR_MESH_WVERTEXS: // 이제 weight를 읽어줄거다. 
			break;

		case TOKENR_MESH_WEIGHT:
		{
			// 버텍스 하나의 정보를 만들어서 리스트에 넣음
			VertexWeight* _newWVertex = new VertexWeight();
			m_OneMesh->m_wvertex = _newWVertex;
			m_OneMesh->m_wvertex->m_wvertex_number = Parsing_NumberInt();
			m_OneMesh->m_vector_wvertexs.push_back(m_OneMesh->m_wvertex);
		}
		break;

		case TOKENR_BONE_BLENGING_WEIGHT:
		{
			// 가중치 한개를 만들어서 리스트에 넣는다
			Weight* _newWeight = new Weight;
			_newWeight->m_bone_number = Parsing_NumberInt();
			_newWeight->m_bone_weight = Parsing_NumberFloat();

			// 가중치를 넣을 mesh의 
			int _index = m_OneMesh->m_wvertex->m_wvertex_number;
			m_OneMesh->m_vector_wvertexs[_index]->m_bone_blending_weight.push_back(_newWeight);
		}
		break;


		/// MESH_FACE_LIST
		case TOKENR_MESH_FACE_LIST:
			//
			break;
		case TOKENR_MESH_FACE:
		{
			// Face의 번호인데...
			Face* _meshFace = new Face();

			// Face 인덱스를 읽는다. 
			Parsing_NumberInt();

			// A:를 읽고
			Parsing_String();
			_meshFace->m_vertexindex[0] = Parsing_NumberInt();
			// B:
			Parsing_String();
			_meshFace->m_vertexindex[1] = Parsing_NumberInt();
			// C:
			Parsing_String();
			_meshFace->m_vertexindex[2] = Parsing_NumberInt();

			/// (뒤에 정보가 더 있지만 default에 의해 스킵될 것이다.)
			/// ......
			// 벡터에 넣어준다.
			m_OneMesh->m_meshface.push_back(_meshFace);
		}
		break;

		/// MESH_NORMALS
		case TOKENR_MESH_NORMALS:
			break;
		case TOKENR_MESH_FACENORMAL:
		{
			listcount = Parsing_NumberInt();

			m_OneMesh->m_meshface[listcount]->m_normal = Parsing_NumberVector3();
		}
		break;
		case TOKENR_MESH_VERTEXNORMAL:
		{
			static int _tempNum = 0; // 버텍스 노말의 인덱스
			int _index = _tempNum % 3; // vertex Normal은 face당 3개이므로 모듈러 연산을 해줌. 

			int _vertexNum = Parsing_NumberInt();
			Vector3 _normal = Parsing_NumberVector3();

			// TOKENR_MESH_FACENORMAL에서 생성된 face의 구조체에 vertexNormal값을 넣어준다. 
			m_OneMesh->m_meshface[listcount]->m_vertexindex[_index] = _vertexNum;
			m_OneMesh->m_meshface[listcount]->m_normalvertex[_index] = _normal;

			_tempNum++; // TOKENR_MESH_FACENORMAL를 만나지 전까지 index를 ++ 해줌. 
		}
		break;

		/// MESH_TVERTLIST
		case TOKENR_MESH_NUMTVERTEX:
		{
			m_OneMesh->m_mesh_numtvertex = Parsing_NumberInt();
		}
		break;
		case TOKENR_MESH_TVERTLIST:
			break;
		case TOKENR_MESH_TVERT:
			// 버텍스의 인덱스가 나오는데 어차피 순서와 같으므로 버린다.
			// 새로운 TVertex를 만들어서 벡터에 넣는다
			// ase 파서를 보니 vertex의 값이 중복된다. 
			// 이 중복되지 않는 애들만 저장을 해야 할 것 같다. 
		{
			Parsing_NumberInt();
			float _u = Parsing_NumberFloat();
			float _v = Parsing_NumberFloat();
			float _w = Parsing_NumberFloat();

			// TextureVertex를 만들어서 UV값을 넣어준다. 
			COneTVertex* _nowTextureVertex = new COneTVertex();

			_nowTextureVertex->m_u = _u;
			_nowTextureVertex->m_v = 1 - _v; // y위치가 텍스처와 바뀌어서 반대로 보정해줌. 
			_nowTextureVertex->m_w = _w;

			m_OneMesh->m_mesh_tvertex.push_back(_nowTextureVertex);
		}
		break;
		case TOKENR_MESH_NUMTVFACES:
		{
			m_OneMesh->m_mesh_tvfaces = Parsing_NumberInt();
		}
		break;

		case TOKENR_MESH_TFACELIST:
			break;
		case TOKENR_MESH_TFACE:
		{
			int _index = Parsing_NumberInt();

			int _tamp01 = Parsing_NumberInt();
			int _tamp02 = Parsing_NumberInt();
			int _tamp03 = Parsing_NumberInt();

			m_OneMesh->m_meshface[_index]->m_TFace[0] = _tamp01;
			m_OneMesh->m_meshface[_index]->m_TFace[2] = _tamp02;
			m_OneMesh->m_meshface[_index]->m_TFace[1] = _tamp03;
		}
		break;

		case TOKENR_PROP_MOTIONBLUR:
		{
			m_OneMesh->m_prop_motionblur = Parsing_NumberInt();
		}
		break;

		case TOKENR_PROP_CASTSHADOW:
		{
			m_OneMesh->m_prop_castshadow = Parsing_NumberInt();
		}
		break;

		case TOKENR_PROP_RECVSHADOW:
		{
			m_OneMesh->m_prop_recvshadow = Parsing_NumberInt();
		}
		break;

		case TOKENR_MATERIAL_REF:
		{
			m_OneMesh->m_material_ref = Parsing_NumberInt();
		}
		break;
	
		case TOKEND_END:
			// 아마도 이건 파일의 끝이 나타났을때인것 같은데. while을 탈출해야 하는데?

			//AfxMessageBox("파일의 끝을 본 것 같습니다!", MB_OK, NULL);
			//TRACE("TRACE: 파싱중: 파일의 끝을 봤습니다!\n");
			return;

			break;

			/// 위의 아무것도 해당하지 않을때
		default:
			// 아무것도 하지 않는다.
			break;

		}	// switch()


		///-----------------------------------------------
		/// 안전 코드.
		i++;
		if (i > 1000000)
		{
			// 루프를 1000000번이상이나 돌 이유가 없다. (데이터가 100000개가 아닌이상)
			// 만약 1000000이상 돌았다면 확실히 뭔가 문제가 있는 것이므로
			TRACE("루프를 백만번 돌았습니다!");
			return;
		}
		/// 안전 코드.
		///-----------------------------------------------

	}		// while()

	// 여기까지 왔다면 while()을 벗어났다는 것이고 그 말은
	// 괄호를 닫았다는 것이므로
	// 리턴하자 (재귀함수)

	return;
}


///----------------------------------------------------------------------
/// parsing을 위한 단위별 함수들
///----------------------------------------------------------------------

// long을 읽어서 리턴해준다.
int CASEParser::Parsing_NumberLong()
{
	LONG			token;
	LONG			tempNumber;

	token = m_lexer->GetToken(m_TokenString);	//ASSERT(token == TOKEND_NUMBER);
	tempNumber = strtoul(m_TokenString, NULL, 10); //문자열을 부호 없는 long 정수값으로 변환

	return			tempNumber;
}

// float
float CASEParser::Parsing_NumberFloat()
{
	LONG			token;
	float			tempNumber;

	token = m_lexer->GetToken(m_TokenString);	//ASSERT(token == TOKEND_NUMBER);
	tempNumber = (float)atof(m_TokenString);

	return			tempNumber;
}

// String
LPSTR CASEParser::Parsing_String()
{
	/// ※m_TokenString ( char[255] ) 이기 때문에 CString에 넣으면 에러 날거라 생각했는데, 생각보다 CString은 잘 만들어진 것 같다. 알아서 받아들이는데?
	m_lexer->GetToken(m_TokenString);

	return m_TokenString;
}


// int
int CASEParser::Parsing_NumberInt() {

	LONG			token;
	int				tempNumber;

	token = m_lexer->GetToken(m_TokenString);	//ASSERT(token == TOKEND_NUMBER);
	tempNumber = (int)atoi(m_TokenString);

	return			tempNumber;
}


// 3개의 Float를 벡터 하나로
Vector3 CASEParser::Parsing_NumberVector3()
{
	LONG				token;
	Vector3			tempVector3;

	token = m_lexer->GetToken(m_TokenString);
	tempVector3.x = (float)atof(m_TokenString);
	token = m_lexer->GetToken(m_TokenString);
	tempVector3.z = (float)atof(m_TokenString);
	token = m_lexer->GetToken(m_TokenString);
	tempVector3.y = (float)atof(m_TokenString);

	return			tempVector3;		// 스태틱 변수의 레퍼런스보다는 값 전달을 하자.
}

Vector3 CASEParser::Parsing_NumberRGBVector3()
{
	LONG				token;
	Vector3			tempVector3;

	token = m_lexer->GetToken(m_TokenString);
	tempVector3.x = (float)atof(m_TokenString);
	token = m_lexer->GetToken(m_TokenString);
	tempVector3.y = (float)atof(m_TokenString);
	token = m_lexer->GetToken(m_TokenString);
	tempVector3.z = (float)atof(m_TokenString);

	return			tempVector3;		// 스태틱 변수의 레퍼런스보다는 값 전달을 하자.
}

///--------------------------------------------------
/// 내부에서 뭔가를 생성, 리스트에 넣는다
///--------------------------------------------------
// 메시를 하나 동적으로 생성하고, 그 포인터를 리스트에 넣는다.
void CASEParser::Create_onemesh_to_list()
{
	GeomObject* temp = new GeomObject;
	m_OneMesh = temp;
	m_OneMesh->m_scenedata = m_scenedata;		// 클래스간 값 복사
	m_MeshList.push_back(m_OneMesh);
}

// 메트리얼 하나를 동적으로 생성하고, 그 포인터를 리스트에 넣는다.
void CASEParser::Create_materialdata_to_list()
{
	ASEMaterial* temp = new ASEMaterial;
	m_materialdata = temp;
	m_list_materialdata.push_back(m_materialdata);
}

// 애니메이션데이터 하나를 동적으로 생성하고, 그 포인터를 리스트에 넣는다.
void CASEParser::Create_animationdata_to_list()
{
	Animation* temp = new Animation;
	m_animation = temp;
	m_list_animation.push_back(m_animation);
}

// 정점 하나를..
void CASEParser::Create_onevertex_to_list()
{
	Vertex* temp = new Vertex;
	m_OneMesh->m_meshvertex.push_back(temp);
}