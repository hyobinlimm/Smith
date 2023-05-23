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
// �ε��Ѵ�.
// �̰��� �����ٸ� ������ ���������� ��� �����͸� �о �� �־�� �Ѵ�.
//
// Parsing�� ����:
// �׻� ������ �������� parsing�̶�� ���� �����Ͱ� ���� �԰ݿ� �°� �� �ִٴ� ���� ������ �Ѵ�.
// ������, ���� ���ο� ������ �ٲ�� �� �ִ°��� �����ϴٴ��� �ϴ� ��Ģ�� �����
// �˻��ϴ� ��ƾ�� ���������� ����. �ϴ��� ������� ���� �Ծ����� �� �ִٴ°��� ��������.
// -> �׷���, ��� ȣ���� �ϴ� �ϳ��� ū �Լ��� �ذ�ô�.
// -> depth�� �������� �����ۿ� ���� ���� �ڵ带 �־�߰ڴ�
//---------------------------------------------------------------------------------------------------
bool CASEParser::Load(LPSTR p_File)
{
	/// 0) ������ �ε��Ѵ�.
	if (!m_lexer->Open(p_File))
	{
		TRACE("������ ���� �߿� ������ �߻��߽��ϴ�!");
		return FALSE;
	}

	/// 1) �ٷ��̾ƺ����豳���� �Ŀ�Ǯ�� ����Լ� �Ѱ��� ó�� ��!
	Parsing_DivergeRecursiveALL(0);

	return TRUE;
}

/// 1�� ��ȯ

// CScenedata ����
bool CASEParser::TranslateToD3DFormat_scene(Scenedata* pscene)
{
	// �� ����
	*pscene = this->m_scenedata;
	return TRUE;
}

/// <summary>
/// �޽��� ����ȭ�� �� �ش�.
/// �븻��, �ؽ��� ��ǥ�� ���� ���ؽ��� �ø���, ��ø�Ǵ°��� �����ϰ�..
/// </summary>
bool CASEParser::Convert_Optimize(GeomObject* pMesh)
{
	bool _isNewVertex = true;

	// ���ؽ����� �ϴ� ��� ����
	for (unsigned int i = 0; i < pMesh->m_meshvertex.size(); i++)
	{
		Vertex* _pVertex = new Vertex;
		_pVertex->m_pos = pMesh->m_meshvertex[i]->m_pos;

		pMesh->m_opt_vertex.push_back(_pVertex);
	}

	// ���ؽ� �ɰ��� �Ҷ� ���ϱ� ���� ����
	int _size = pMesh->m_mesh_numvertex;
	pMesh->m_normalList.resize(_size);

	// �������� face�� �������� �� �ε����� ã�ƾ� ��
	unsigned int _faceCount = pMesh->m_meshface.size();
	//TRACE("FaceCount : %d", _faceCount);

	// �ϴ� face�� �̷�� Vertex�� normal���� �־�����.
	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		// vertex�� 3���� normal�� ��� �ֱ� ������ �� ���� �ϳ��ϳ� �־��ش�. 
		for (int j = 0; j < 3; j++)
		{
			// Face�� �̷�� vertex�� ������ͼ� Normal�� �־��ش�. 
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// vertex normal�� ��������� ������ �ִ´�. 
			// vertex normal�� ���Ѵ�. 
			// ������ �ƹ��͵� ����. 
			// �ٸ��� vertex�� �����ؼ� m_opt_vertex�� �־�����.
			// vertex �ε����� �þ. �׷��Ƿ� pMesh->m_opt_index[i].index[j]�� �߰��� �ֵ��� �־������. 
			if (_nowVertex->m_normal.x == 0 && _nowVertex->m_normal.y == 0 && _nowVertex->m_normal.z == 0)
			{
				_nowVertex->m_normal.x = pMesh->m_meshface[i]->m_normalvertex[j].x;
				_nowVertex->m_normal.y = pMesh->m_meshface[i]->m_normalvertex[j].y;
				_nowVertex->m_normal.z = pMesh->m_meshface[i]->m_normalvertex[j].z;
				// m_normalList�� ���� Vertex�� �ִ� �븻������ �״�� �����ؿ´�. 
				int _tempVertexIndex = pMesh->m_meshface[i]->m_vertexindex[j];
				Vector3 _tempNormal = pMesh->m_meshface[i]->m_normalvertex[j];

				pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].push_back(make_pair(_tempVertexIndex, _tempNormal));
			}
			else
			{
				_isNewVertex = true;

				// ������ ���� ���Ϳ� ��ġ�°� ������ �˻� 
				for (int _index = 0; _index < pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].size(); _index++)
				{
					if (_index >= pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].size())break;
					if (pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].at(_index).second == pMesh->m_meshface[i]->m_normalvertex[j])
					{
						_isNewVertex = false;

						// �̹� ���� vertexNoraml ���� �ִٸ� face�� �̷�� vertexIndex�� ���� �ٲ�����. 
						pMesh->m_meshface[i]->m_vertexindex[j] = pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].at(_index).first;
						break;
					}
				}

				if (_isNewVertex)
				{
					// vertex ����. 
					Vertex* _newVertex = new Vertex();

					// �̹� �����ϴ� ���ؽ��� �ε����� ����ͼ� �������� �־�����. 
					int vertexIndex = pMesh->m_meshface[i]->m_vertexindex[j];
					_newVertex->m_pos = pMesh->m_meshvertex[vertexIndex]->m_pos;

					pMesh->m_opt_vertex.push_back(_newVertex);

					// ���� ������ vertex �� normal���� �־��ش�. 
					_newVertex->m_normal.x = pMesh->m_meshface[i]->m_normalvertex[j].x;
					_newVertex->m_normal.y = pMesh->m_meshface[i]->m_normalvertex[j].y;
					_newVertex->m_normal.z = pMesh->m_meshface[i]->m_normalvertex[j].z;

					// ���� ���� vertex�� ��ȣ�� �־��ش�. 
					int _tempVertexIndex = pMesh->m_opt_vertex.size() - 1;
					Vector3 _tempNormal = pMesh->m_meshface[i]->m_normalvertex[j];

					pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].push_back(make_pair(_tempVertexIndex, _tempNormal));

					pMesh->m_meshface[i]->m_vertexindex[j] = pMesh->m_opt_vertex.size() - 1;
				}
			}
		}
	}

	// ���� vertex Normal�� ���� ���� indexbuffer�� �־���� �Ѵ�. 
	// �ε����� �׳� ����
	pMesh->m_opt_index = new IndexList[pMesh->m_meshface.size()];

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pMesh->m_opt_index[i].index[j] = pMesh->m_meshface[i]->m_vertexindex[j];
		}
	}


	/// ���� �ؽ�ó�� ��������. 
	_isNewVertex = false;

	// �ؽ�ó �ɰ��� �� �� ���ϱ� ���� ����
	vector<vector<pair<int, Vector3>>> _textureList;
	int _textureSize = pMesh->m_mesh_numtvertex;
	_textureList.resize(_textureSize);

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		// �� ���� face�� 3���� ������ ����ִ�. 
		for (int j = 0; j < 3; j++)
		{
			// Face�� �̷�� vertex�� ������ͼ� �ؽ�ó ��ǥ�� �־��ش�. 
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// vertex �ؽ�ó�� ��������� ������ �ִ´�. 
			// vertex �ؽ�ó�� ���ؼ� ������ �ƹ��͵� ����. 
			// �ٸ��� vertex�� �����ؼ� m_opt_vertex�� �־�����.
			// vertex �ε����� �þ. �׷��Ƿ� pMesh->m_opt_index[i].index[j]�� �߰��� �ֵ��� �־������. 
			if (_nowVertex->u == 0 && _nowVertex->v == 0)
			{
				// vertex�� texture ���� �־�����.
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

				// �̷� �� ���������� ���� �ſ� ����
				/// �� ���� vertex�� Index�� �� �� �ִ� �ؽ��� ��ǥ���� ����
				vector<pair<int, Vector3>> _tVertices = _textureList[pMesh->m_meshface[i]->m_TFace[j]];

				// ������ ���� ���Ϳ� ��ġ�°� ������ �˻� 
				for (int _index = 0; _index < _tVertices.size(); _index++)
				{
					if (_index >= _tVertices.size()) break;

					// ���ӵ鿡��, n��° �ε����� ����ִ� UV��
					Vector3 _uvw = _tVertices.at(_index).second;

					// �ؽ����� ��ǥ�� ������...
					if (_uvw.x == _nowVertex->u && _uvw.y == _nowVertex->v)
					{
						_isNewVertex = false;

						// ���� �ؽ��� ���� ������ ���ؽ��� �����Ƿ� �� �ε����� �ٲ��ش�.
						pMesh->m_meshface[i]->m_vertexindex[j] = pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].at(_index).first;

						//TRACE("���� ���ؽ��� �̹� �ֽ��ϴ�! %d\n", pMesh->m_meshface[i]->m_vertexindex[j]);
						break;
					}
				}

				if (_isNewVertex)
				{
					// vertex ����. 
					Vertex* _newVertex = new Vertex();

					// �̹� �����ϴ� ���ؽ��� �ε����� ����ͼ� �������� �־�����. 
					int vertexIndex = pMesh->m_meshface[i]->m_vertexindex[j];
					_newVertex->m_pos = pMesh->m_opt_vertex[vertexIndex]->m_pos;
					_newVertex->m_normal = pMesh->m_opt_vertex[vertexIndex]->m_normal;

					// vertex�� texture ���� �־�����.
					int _TvertexIndex = pMesh->m_meshface[i]->m_TFace[j];
					_newVertex->u = pMesh->m_mesh_tvertex[_TvertexIndex]->m_u;
					_newVertex->v = pMesh->m_mesh_tvertex[_TvertexIndex]->m_v;
					_newVertex->m_istextureset = true;

					// ���� �߰��� �Ǿ��ٴ� ���� �˸��� ���ؼ� Ư�� �ڷᱸ���� �����Ѵ�.
					Vector3 _uv;
					_uv.x = _newVertex->u;
					_uv.y = _newVertex->v;
					_uv.z = 0.f;
					_textureList[_TvertexIndex].push_back(make_pair(_TvertexIndex, _uv));

					// ���� ���ؽ��� �߰�
					pMesh->m_opt_vertex.push_back(_newVertex);

					// �߰��� �Ǿ����Ƿ� ���ؽ��� �ε����� �޶����� ���̰�, �װ��� ������Ʈ �� �ش�.
					pMesh->m_meshface[i]->m_vertexindex[j] = pMesh->m_opt_vertex.size() - 1;

					//TRACE("���ؽ��� ���� �߰��߽��ϴ�.! %d\n", pMesh->m_meshface[i]->m_vertexindex[j]);
				}
			}
		}
	}

	// ���� vertex Normal�� ���� ���� indexbuffer�� �־���� �Ѵ�. 
	// �ε����� �׳� ����
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
	// ���ؽ����� �ϴ� ��� ����
	for (unsigned int i = 0; i < pMesh->m_meshvertex.size(); i++)
	{
		Vertex* _pVertex = new Vertex;
		_pVertex->m_pos = pMesh->m_meshvertex[i]->m_pos;

		pMesh->m_opt_vertex.push_back(_pVertex);
	}

	// �������� face�� �������� �� �ε����� ã�ƾ� ��
	unsigned int _faceCount = pMesh->m_meshface.size();
	//TRACE("FaceCount : %d", _faceCount);

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		for (int j = 0; j < 3; j++)
		{
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// face�������� ���ؽ��� �븻�� �־��ش�.
			// (���ÿ�)
			_nowVertex->m_normal.x = 0;
			_nowVertex->m_normal.y = 0;
			_nowVertex->m_normal.z = 0;
		}
	}

	// �ε����� �׳� ����
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
/// �޽��� ����ȭ�� �� �ش�.
/// �븻��, �ؽ��� ��ǥ�� ���� ���ؽ��� �ø���, ��ø�Ǵ°��� �����ϰ�..
/// </summary>
bool CASEParser::Convert_NewOptimize(GeomObject* pMesh)
{
	bool _result = false;

	// 1. �⺻���� ������ ������ �־��ش�.
	_result = Convert_NewOptimize_Position(pMesh);

	// 2. �븻 �����͸� ����ȭ�Ѵ�.
	_result = Convert_NewOptimize_Normal(pMesh);

	// 3. �ؽ��� �����͸� ����ȭ�Ѵ�.
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

// 1. �⺻���� ������ ������ �־��ش�.
bool CASEParser::Convert_NewOptimize_Position(GeomObject* pMesh)
{
	// ���ؽ����� �ϴ� ��� ����
	for (size_t i = 0; i < pMesh->m_meshvertex.size(); i++)
	{
		Vertex* _pVertex = new Vertex;
		_pVertex->m_pos = pMesh->m_meshvertex[i]->m_pos;

		// ��Ű�Ͽ�����Ʈ��� wvertex���� �־�����. 
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

// 2. �븻 �����͸� ����ȭ�Ѵ�.
bool CASEParser::Convert_NewOptimize_Normal(GeomObject* pGeomObject)
{
	/// �װ�Ƽ�� �������� �˻�����. 
	// 1. nodeTM���� _scrale���� �̾Ƴ���.(decompose�� �̿��ϸ� ��.)
	XMVECTOR _pos;
	XMVECTOR _rot;
	XMVECTOR _scale;

	XMMATRIX _nodeTM = XMLoadFloat4x4(&pGeomObject->m_nodeTM);

	// decompose LoclaTM
	XMMatrixDecompose(&_scale, &_rot, &_pos, _nodeTM);

	XMVECTOR _determinant = XMMatrixDeterminant(_nodeTM);

	// negative scale���� �ƴ��� �Ǻ��ϱ� ���� ���� 
	/*bool isNegative = false;

	// negative scale �̶�? 
	// �����߿� �ϳ��� ������ �������� �ƴ϶�
	// �Ѱ� �Ǵ� 3���� ������ ����.

	// 2. �̾� �� _scale�� ���� ���� ��� �ִٸ� negative scale�̴�. 
	if (_determinant.m128_f32[0] < 0)
	{
		isNegative = true;
	}
	
	// 3. negative scale�̶�� -1�� ��������. 
	if (isNegative)
	{ 
		_scale = -1 * _scale;

		// -1�� ���� _scale���� nodeTM���� �ٽ� ����� ����. 
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

	// ���ؽ� �ɰ��� �Ҷ� ���ϱ� ���� ����
	pGeomObject->m_normalList.resize((size_t)pGeomObject->m_mesh_numvertex);

	// �������� face�� �������� �� �ε����� ã�ƾ� ��
	unsigned int _faceCount = pGeomObject->m_meshface.size();
	//TRACE("FaceCount : %d", _faceCount);

	bool _isNewVertex = true;

	// �ϴ� face�� �̷�� Vertex�� normal���� �־�����.
	for (unsigned int i = 0; i < pGeomObject->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		// vertex�� 3���� normal�� ��� �ֱ� ������ �� ���� �ϳ��ϳ� �־��ش�. 
		for (int j = 0; j < 3; j++)
		{
			// Face�� �̷�� vertex�� ������ͼ� Normal�� �־��ش�. 
			int _nowVertexIndex = pGeomObject->m_meshface[i]->m_vertexindex[j];
			Vertex* _nowVertex = pGeomObject->m_opt_vertex[_nowVertexIndex];

			// vertex normal�� ��������� ������ �ִ´�. 
			// vertex normal�� ���Ѵ�. 
			// ������ �ƹ��͵� ����. 
			// �ٸ��� vertex�� �����ؼ� m_opt_vertex�� �־�����.
			// vertex �ε����� �þ. �׷��Ƿ� pMesh->m_opt_index[i].index[j]�� �߰��� �ֵ��� �־������. 
			if (_nowVertex->m_normal.x == 0 && _nowVertex->m_normal.y == 0 && _nowVertex->m_normal.z == 0)
			{
				_nowVertex->m_normal.x = pGeomObject->m_meshface[i]->m_normalvertex[j].x;
				_nowVertex->m_normal.y = pGeomObject->m_meshface[i]->m_normalvertex[j].y;
				_nowVertex->m_normal.z = pGeomObject->m_meshface[i]->m_normalvertex[j].z;

				// m_normalList�� ���� Vertex�� �ִ� �븻������ �״�� �����ؿ´�. 
				///int _newVertexIndex = pMesh->m_meshface[i]->m_vertexindex[j];
				int _newVertexIndex = _nowVertexIndex;
				Vector3 _newNormal = pGeomObject->m_meshface[i]->m_normalvertex[j];

				pGeomObject->m_normalList[_nowVertexIndex].push_back(make_pair(_newVertexIndex, _newNormal));
			}
			else
			{
				_isNewVertex = true;

				// ������ ���� ���Ϳ� ��ġ�°� ������ �˻� 
				for (int _index = 0; _index < pGeomObject->m_normalList[_nowVertexIndex].size(); _index++)
				{
					if (pGeomObject->m_normalList[_nowVertexIndex].at(_index).second == pGeomObject->m_meshface[i]->m_normalvertex[j])
					{
						_isNewVertex = false;

						// �̹� ���� vertexNoraml ���� �ִٸ� face�� �̷�� vertexIndex�� ���� �ٲ�����. 
						pGeomObject->m_meshface[i]->m_vertexindex[j] = pGeomObject->m_normalList[_nowVertexIndex].at(_index).first;
						break;
					}
				}

				// ���ο� ���ؽ��� �����ؼ� �߰�
				if (_isNewVertex)
				{
					Vertex* _newVertex = new Vertex();

					// �̹� �����ϴ� ���ؽ��� �ε����� ����ͼ� �������� �־�����. 
					int _vertexIndex = pGeomObject->m_meshface[i]->m_vertexindex[j];
					_newVertex->m_pos = pGeomObject->m_meshvertex[_vertexIndex]->m_pos;

					pGeomObject->m_opt_vertex.push_back(_newVertex);

					// ���� ������ vertex �� normal���� �־��ش�. 
					_newVertex->m_normal.x = pGeomObject->m_meshface[i]->m_normalvertex[j].x;
					_newVertex->m_normal.y = pGeomObject->m_meshface[i]->m_normalvertex[j].y;
					_newVertex->m_normal.z = pGeomObject->m_meshface[i]->m_normalvertex[j].z;

					// ���� ������ vertex �� wvertex���� �־��ش�. 
					_newVertex->m_bw1 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw1;
					_newVertex->m_bw2 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw2;
					_newVertex->m_bw3 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw3;
					_newVertex->m_bw4 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw4;
					_newVertex->m_bw5 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw5;
					_newVertex->m_bw6 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw6;
					_newVertex->m_bw7 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw7;
					_newVertex->m_bw8 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw8;
					_newVertex->m_bw9 = pGeomObject->m_meshvertex[_vertexIndex]->m_bw9;

					// ���� ���� vertex�� ��ȣ�� �־��ش�. 
					int _newVertexIndex = pGeomObject->m_opt_vertex.size() - 1;
					Vector3 _newNormal = pGeomObject->m_meshface[i]->m_normalvertex[j];

					pGeomObject->m_normalList[_vertexIndex].push_back(make_pair(_newVertexIndex, _newNormal));

					pGeomObject->m_meshface[i]->m_vertexindex[j] = pGeomObject->m_opt_vertex.size() - 1;
				}
			}
		}
	}

	// ���� vertex Normal�� ���� ���� indexbuffer�� �־���� �Ѵ�. 
	ApplyIndicesFromFaces(pGeomObject);

	return true;
}

// ���� �ؽ�ó�� ��������. 
bool CASEParser::Convert_NewOptimize_Texture(GeomObject* pMesh)
{
	bool _isNewVertex = false;

	// �ؽ�ó �ɰ��� �� �� ���ϱ� ���� ����
	vector<vector<pair<int, Vector3>>> _textureList;
	int _textureSize = pMesh->m_mesh_numtvertex;
	_textureList.resize(_textureSize);

	if (_textureSize == 0) return false;

	// �������� face�� �������� �� �ε����� ã�ƾ� ��
	unsigned int _faceCount = pMesh->m_meshface.size();
	//TRACE("FaceCount : %d", _faceCount);

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		// �� ���� face�� 3���� ������ ����ִ�. 
		for (int j = 0; j < 3; j++)
		{
			// Face�� �̷�� vertex�� ������ͼ� �ؽ�ó ��ǥ�� �־��ش�. 
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// vertex �ؽ�ó�� ��������� ������ �ִ´�. 
			// vertex �ؽ�ó�� ���ؼ� ������ �ƹ��͵� ����. 
			// �ٸ��� vertex�� �����ؼ� m_opt_vertex�� �־�����.
			// vertex �ε����� �þ. �׷��Ƿ� pMesh->m_opt_index[i].index[j]�� �߰��� �ֵ��� �־������. 
			if (_nowVertex->u == 0 && _nowVertex->v == 0)
			{
				// vertex�� texture ���� �־�����.
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

				// �̷� �� ���������� ���� �ſ� ����
				/// �� ���� vertex�� Index�� �� �� �ִ� �ؽ��� ��ǥ���� ����
				vector<pair<int, Vector3>> _tVertices = _textureList[pMesh->m_meshface[i]->m_TFace[j]];

				// ������ ���� ���Ϳ� ��ġ�°� ������ �˻� 
				for (int _index = 0; _index < _tVertices.size(); _index++)
				{
					// ���ӵ鿡��, n��° �ε����� ����ִ� UV��
					Vector3 _uvw = _tVertices.at(_index).second;

					// �ؽ����� ��ǥ�� ������...
					if (_uvw.x == _nowVertex->u && _uvw.y == _nowVertex->v)
					{
						_isNewVertex = false;

						// ���� �ؽ��� ���� ������ ���ؽ��� �����Ƿ� �� �ε����� �ٲ��ش�.
						//Vertex* index = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];
						//pMesh->m_meshface[i]->m_vertexindex[j] = pMesh->m_normalList[pMesh->m_meshface[i]->m_vertexindex[j]].at(_index).first;

						//TRACE("���� ���ؽ��� �̹� �ֽ��ϴ�! %d\n", pMesh->m_meshface[i]->m_vertexindex[j]);
						break;
					}
				}

				// ���ο� ���ؽ��� �����ؼ� �߰�
				if (_isNewVertex)
				{
					Vertex* _newVertex = new Vertex();

					// �̹� �����ϴ� ���ؽ��� �ε����� ����ͼ� �������� �־�����. 
					int vertexIndex = pMesh->m_meshface[i]->m_vertexindex[j];
					_newVertex->m_pos = pMesh->m_opt_vertex[vertexIndex]->m_pos;
					_newVertex->m_normal = pMesh->m_opt_vertex[vertexIndex]->m_normal;

					// vertex�� texture ���� �־�����.
					int _TvertexIndex = pMesh->m_meshface[i]->m_TFace[j];
					_newVertex->u = pMesh->m_mesh_tvertex[_TvertexIndex]->m_u;
					_newVertex->v = pMesh->m_mesh_tvertex[_TvertexIndex]->m_v;
					_newVertex->m_istextureset = true;

					// ���� ������ vertex �� wvertex���� �־��ش�. 
					_newVertex->m_bw1 = pMesh->m_opt_vertex[vertexIndex]->m_bw1;
					_newVertex->m_bw2 = pMesh->m_opt_vertex[vertexIndex]->m_bw2;
					_newVertex->m_bw3 = pMesh->m_opt_vertex[vertexIndex]->m_bw3;
					_newVertex->m_bw4 = pMesh->m_opt_vertex[vertexIndex]->m_bw4;
					_newVertex->m_bw5 = pMesh->m_opt_vertex[vertexIndex]->m_bw5;
					_newVertex->m_bw6 = pMesh->m_opt_vertex[vertexIndex]->m_bw6;
					_newVertex->m_bw7 = pMesh->m_opt_vertex[vertexIndex]->m_bw7;
					_newVertex->m_bw8 = pMesh->m_opt_vertex[vertexIndex]->m_bw8;
					_newVertex->m_bw9 = pMesh->m_opt_vertex[vertexIndex]->m_bw9;

					// ���� �߰��� �Ǿ��ٴ� ���� �˸��� ���ؼ� Ư�� �ڷᱸ���� �����Ѵ�.
					Vector3 _uv(_newVertex->u, _newVertex->v, 0.f);
					_textureList[_TvertexIndex].push_back(make_pair(_TvertexIndex, _uv));

					// ���� ���ؽ��� �߰�
					pMesh->m_opt_vertex.push_back(_newVertex);

					// �߰��� �Ǿ����Ƿ� ���ؽ��� �ε����� �޶����� ���̰�, �װ��� ������Ʈ �� �ش�.
					pMesh->m_meshface[i]->m_vertexindex[j] = pMesh->m_opt_vertex.size() - 1;

					//TRACE("���ؽ��� ���� �߰��߽��ϴ�.! %d\n", pMesh->m_meshface[i]->m_vertexindex[j]);
				}
			}
		}
	}

	// ���� �ؽ��� ������ ���� ���ؽ��� �ε��� ������ indexbuffer�� �־���� �Ѵ�. 
	ApplyIndicesFromFaces(pMesh);

	return true;
}

bool CASEParser::Insert_SkinnedObject_Weight(GeomObject* pMesh, int vertexNum)
{
	if (pMesh->m_vector_bone_list.size() != 0)
	{
		VertexWeight* _wvertex = pMesh->m_vector_wvertexs[vertexNum];

		// ����ġ�� ���� ������ ����
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


// ���ؽ� ���۰� ����Ǿ����Ƿ� indexbuffer�� ���ŵǾ�� �Ѵ�. 
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
// ��� ȣ����� ������ �ϴ� �б� �Լ��̴�.
//
// �ϴ� �а�, ��ū�� ���� �����Ѵ�.
//
// ��͸� ���� ������ { �� ���� ����̴�.
// �����ϴ� ������ '}'�� �����ų� TOKEND_END�� ������ ���̴�.
//
// ���Ҿ�, ��͸� ���� �� ��ū�� �о�, ���� �� �Լ��� � ��������� �����Ѵ�.
//
//----------------------------------------------------------------
void CASEParser::Parsing_DivergeRecursiveALL(int depth)
{
	//----------------------------------------------------------------------
	// ���������� ����
	// ��� �����ٰ� ���� �̻� ������ ���� ���(����)�� üũ�ϱ� ���� ����
	int i = 0;
	// ����Ʈ�� �����͸� �־�� �� �� ���̴� ����Ʈ ī����.
	int listcount = 0;		// �ſ� C����� �ٸ� �ذ�å�� ������?
	// �̰��� �� ��ū�� � �����ΰ��� �Ǻ����ش�.
	// �̰��� ��� ������ �ϰ� ��͸� �����°��� �����ϴ�? -> �߸��� ��͹���̴�?
	LONG nowtoken;
	//----------------------------------------------------------------------

	Matrix TM1, TM2;
	Quaternion tempQT;
	Quaternion prevQT;
	Quaternion resultQT;

	/// �̰��� �ϸ� �� ���� ��ū�� �а�, �� ������ ������ �� �� �ִ�.
	while (nowtoken = m_lexer->GetToken(m_TokenString), nowtoken != TOKEND_BLOCK_END)
	{
		// �ϴ� �� ���� ��ū�� �а�, �װ��� ��ȣ �ݱⰡ �ƴ϶��.
		// �Ѿ�� ��ū�� ���� ó�����ش�.

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
			Parsing_String();	// �׳� m_TokenString�� �о������ ���� ��.
			//AfxMessageBox( m_TokenString, NULL, NULL);		/// �ӽ÷� �ڸ�Ʈ�� ����غ���
			break;

			//--------------------
			// SCENE
			//--------------------

		case TOKENR_SCENE:
			//
			break;
		case TOKENR_SCENE_FILENAME:
			m_scenedata.m_filename = Parsing_String();		// �ϰ��� �ִ� �Լ��� ����� ���� String�� Int�� ��������.
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
			// �׳� ���� ������ �о������ }�� ���ö�����
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
			/// material ����ü�� ����
			// material �� 1�� �̻��� �� �ֱ� ������ �װ� ������ list�� �־�����.
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
			// MAP_DIFFUSE ����ü �о���� ����
			m_materialdata->m_map_diffuse = new MaterialMap();
		}
		break;

		case TOKENR_MAP_SPECULAR:
		{
			// MAP_SPECULAR ����ü �о���� ����
			m_materialdata->m_map_specular = new MaterialMap();
		}
		break;

		case TOKENR_MAP_SHINE: // �ݻ���
		{
			// MAP_DIFFUSE ����ü �о���� ����
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
			//	�� ���� �׷� ����. �� ������ �̸��� ��Ʈ������ ������� �ϴµ�.
		break;

		case TOKENR_HELPEROBJECT:
			// �ϴ� �����ϰ�
			// ������Ʈ�� Ÿ�� ������. �̰Ϳ� ���� ���� �ٸ� �Ľ� ��� �ߵ�.
		{
			m_parsingmode = eParsingmode::eHelperObject;
			GeomObject* temp = new GeomObject;
			m_OneMesh = temp;
			m_OneMesh->m_type = eHelperObject;
			m_OneMesh->m_scenedata = m_scenedata;		// Ŭ������ �� ����
			m_MeshList.push_back(m_OneMesh);
		}
		break;

		case TOKENR_GEOMOBJECT:
			/// �� ��ū�� �����ٴ°� ���ο� �޽ð� ����ٴ� ���̴�. ���� ������ mesh�� �ϳ� ����, 
			/// �� �����͸� ����Ʈ�� �ְ�, m_onemesh�� �� �����͸� ����, �״�� ���� �ɱ�?
		{
			m_parsingmode = eParsingmode::eGeomobject;

			m_OneMesh = new GeomObject();
			m_OneMesh->m_type = eGeomobject;
			m_MeshList.push_back(m_OneMesh);
		}
		break;

		case TOKENR_TM_ANIMATION: // �ִϸ��̼� ��Ͽ� ���Դٸ�.
		{
			m_parsingmode = eParsingmode::eAnimation;

			// �ִϸ��̼��� �������ش�. 
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
			// ��� ������ ������Ʈ���� ���� �� �� �ִ� ������ ���̴�.
			// ��忡 ���� �־�� �� ���� �ٸ���.
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
			// �� ����� �θ� ����� ����.
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
			// (NODE_TM���� ���� �� NODE_NAME�� �ѹ� �� ���´�.)
			// (Animation���� ������ �ؾ� �ϱ� ������ �̷��� ��带 �����ش�)
			/// ȿ�� : NODE_NAME�� �ѹ� �� �����µ�
			/// ���� �����ϴ� �Ͱ� nodeName�� ������ ���ؾ� �ϴ� ���ΰ�? 
			/// �ƴϸ� ������ �̹� �������� ������ �ʿ䰡 ���°�?

			/// �Դٰ�,
			// ī�޶�� NodeTM�� �ι� ���´�. �ι�°��� ���� �ʴ´�.
			// �ƿ� �� ��Ϳ��� ������ ��Ű�� ������. �߰��� �д°��� �����ؾ� �ϱ� ������...
			//if (m_onemesh->m_camera_isloadTarget) {
			//	'}'�� ���ö����� �����°� ������ ����! �� �̷��� �ȵǳ�..
			// ��ͱ����� �������̴�....
		break;

		case TOKENR_INHERIT_POS:
			// ī�޶�� NodeTM�� �ι� ���´�. �ι�°��� ���� �ʴ´�.
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

				// NodeTM 0���� ���� �־��ش�. 
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

				// NodeTM 0���� ���� �־��ش�. 
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

				// NodeTM 2���� ���� �־��ش�. 
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

				// NodeTM 0���� ���� �־��ش�. 
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
		// ���� ī�޶� ���¿��ٸ� �̹� ��带 ���� ������ ǥ�����ش�.
		break;

		/// ANIMATION	
		case TOKENR_CONTROL_POS_TRACK:	// 	decompose�� ��ǥ�� �о���� ������ ���̴�.
			break;

		case TOKENR_CONTROL_POS_SAMPLE:
		{
			CAnimation_pos* temp = new CAnimation_pos();  // ���� ���� �ӽ� ����.

			// keyFrame
			// m_scenedata.m_firstframe * m_scenedata.m_ticksperframe = �������� �帣�� �ð�
			// ���� time == 0���� �����ϴ°� �ƴ϶��, 
			// m_scenedata.m_firstframe * m_scenedata.m_ticksperframe �ð��� ���༭ 0���� �����Ѵ�. 
			int tempTime = Parsing_NumberInt();
			temp->m_time = tempTime - (m_scenedata.m_firstframe * m_scenedata.m_ticksperframe);

			// postion
			temp->m_pos = Parsing_NumberVector3();

			// �ӽ� ������ ���� �ִϸ��̼��� postion Vecter�� �־�����. 
			
			if (temp->m_time >= 0 ||// ������ �ð��� ���� 0���� �۰�, ������ ������ �ð����� ũ�� �������� �ʴ´�. 
				temp->m_time < (m_scenedata.m_lastframe * m_scenedata.m_ticksperframe) - (m_scenedata.m_firstframe * m_scenedata.m_ticksperframe))
			{
				m_animation->m_position.push_back(temp);
			}
		}
		break;

		case TOKENR_CONTROL_ROT_TRACK: // ���� ȸ���� �о�� �غ�����. 
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

			// ���ʹϾ� ��ȯ!
				// ASE�� ��, ������ �̷���� ����, ���� �����ӿ��� �󸶸�ŭ ���ߴ����� ���� ����
				// ������ �츮�� ���ʹϾ� �� ��ü�� 0 ���� => �ش� ������ �����ؾ���.
				// �׷��� ���ʹϾ��� ��� ���� �������Ѿ��� => ���밪�� �������
			tempQT =  XMQuaternionRotationAxis(temp->m_rot, temp->m_angle);
			
			// ������� ������ ���� ������ ���� ���� �־��ش�.
			if (m_animation->m_rotation.size() == 0)
			{
				temp->m_rotQT_accumulation = tempQT;
			}
			else
			{
				// ���� ���ʹϾ� ���� ���ͼ� ���� ���ʹϾ� ���� �����ָ� �ȴ�..!
				//int _quateindex = m_animation->m_rotation.size() - 1;

				 //prevQT = m_animation->m_rotation[_quateindex]->m_rotQT_accumulation;

				temp->m_rotQT_accumulation = tempQT;
			}

			if (temp->m_time >= 0 || // ������ �ð��� ���� 0���� �۰�, ������ ������ �ð����� ũ�� �������� �ʴ´�. 
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
			// �̰� �ִٸ� �̰��� Skinned Mesh��� ������ ���´�.
			// ���� �Է�
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
			// ���ؽ��� ������ ����־�� �ϴµ�
			// �̹� ���ͷ� ������ �� �����Ƿ� �׳� ������ �ȴ�.
			break;
		case TOKENR_MESH_VERTEX:
		{// ������ �Է�
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
			// ���� bone�� ���� �غ� ����.
			//m_OneMesh->m_is_skinningobject = true;
		}
			break;
		case TOKENR_BONE:
		{
			/// ��� ü���� �� �ְ�, Bone�� �����ϰ� �ִٴ� ���� �̰��� ��Ű�� ������Ʈ��� ���̴�.
			// �� �ϳ��� ���� �ӽ� ������ ����, ���Ϳ� �ְ�
			// Bone�� �ѹ��� �о� ����
			Bone* _newBone = new Bone();
			m_OneMesh->m_is_skinningobject = true;
			m_OneMesh->m_bone = _newBone;
			m_OneMesh->m_bone->m_bone_number = Parsing_NumberInt();	
			m_OneMesh->m_vector_bone_list.push_back(m_OneMesh->m_bone);
		}
		break;

		//�� ������ ���� �̸��� �־�� �Ѵ�. ������ {�� �� �� �� �������Ƿ� �ӽ� ������ �����μ� �����ؾ߰���.
		case TOKENR_BONE_NAME:
		{
			// TOKENR_BONE ���� �о�� ���° ������ �־��ش�. 
			int _BoneIndex = m_OneMesh->m_bone->m_bone_number;
			// mesh�� bone list�� index�� �����Ͽ� �̸��� �־�����. 
			m_OneMesh->m_vector_bone_list[_BoneIndex]->m_bone_name = Parsing_String();
		}
		break;

		case TOKENR_BONE_PROPERTY:
			// �� ���� ABSOLUTE�� ������� �ϴµ�, �� ���� ����.
			break;
			// �������� TM_ROW0~3�� �����µ� ���� ���õ�..

		case TOKENR_MESH_WVERTEXS: // ���� weight�� �о��ٰŴ�. 
			break;

		case TOKENR_MESH_WEIGHT:
		{
			// ���ؽ� �ϳ��� ������ ���� ����Ʈ�� ����
			VertexWeight* _newWVertex = new VertexWeight();
			m_OneMesh->m_wvertex = _newWVertex;
			m_OneMesh->m_wvertex->m_wvertex_number = Parsing_NumberInt();
			m_OneMesh->m_vector_wvertexs.push_back(m_OneMesh->m_wvertex);
		}
		break;

		case TOKENR_BONE_BLENGING_WEIGHT:
		{
			// ����ġ �Ѱ��� ���� ����Ʈ�� �ִ´�
			Weight* _newWeight = new Weight;
			_newWeight->m_bone_number = Parsing_NumberInt();
			_newWeight->m_bone_weight = Parsing_NumberFloat();

			// ����ġ�� ���� mesh�� 
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
			// Face�� ��ȣ�ε�...
			Face* _meshFace = new Face();

			// Face �ε����� �д´�. 
			Parsing_NumberInt();

			// A:�� �а�
			Parsing_String();
			_meshFace->m_vertexindex[0] = Parsing_NumberInt();
			// B:
			Parsing_String();
			_meshFace->m_vertexindex[1] = Parsing_NumberInt();
			// C:
			Parsing_String();
			_meshFace->m_vertexindex[2] = Parsing_NumberInt();

			/// (�ڿ� ������ �� ������ default�� ���� ��ŵ�� ���̴�.)
			/// ......
			// ���Ϳ� �־��ش�.
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
			static int _tempNum = 0; // ���ؽ� �븻�� �ε���
			int _index = _tempNum % 3; // vertex Normal�� face�� 3���̹Ƿ� ��ⷯ ������ ����. 

			int _vertexNum = Parsing_NumberInt();
			Vector3 _normal = Parsing_NumberVector3();

			// TOKENR_MESH_FACENORMAL���� ������ face�� ����ü�� vertexNormal���� �־��ش�. 
			m_OneMesh->m_meshface[listcount]->m_vertexindex[_index] = _vertexNum;
			m_OneMesh->m_meshface[listcount]->m_normalvertex[_index] = _normal;

			_tempNum++; // TOKENR_MESH_FACENORMAL�� ������ ������ index�� ++ ����. 
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
			// ���ؽ��� �ε����� �����µ� ������ ������ �����Ƿ� ������.
			// ���ο� TVertex�� ���� ���Ϳ� �ִ´�
			// ase �ļ��� ���� vertex�� ���� �ߺ��ȴ�. 
			// �� �ߺ����� �ʴ� �ֵ鸸 ������ �ؾ� �� �� ����. 
		{
			Parsing_NumberInt();
			float _u = Parsing_NumberFloat();
			float _v = Parsing_NumberFloat();
			float _w = Parsing_NumberFloat();

			// TextureVertex�� ���� UV���� �־��ش�. 
			COneTVertex* _nowTextureVertex = new COneTVertex();

			_nowTextureVertex->m_u = _u;
			_nowTextureVertex->m_v = 1 - _v; // y��ġ�� �ؽ�ó�� �ٲ� �ݴ�� ��������. 
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
			// �Ƹ��� �̰� ������ ���� ��Ÿ�������ΰ� ������. while�� Ż���ؾ� �ϴµ�?

			//AfxMessageBox("������ ���� �� �� �����ϴ�!", MB_OK, NULL);
			//TRACE("TRACE: �Ľ���: ������ ���� �ý��ϴ�!\n");
			return;

			break;

			/// ���� �ƹ��͵� �ش����� ������
		default:
			// �ƹ��͵� ���� �ʴ´�.
			break;

		}	// switch()


		///-----------------------------------------------
		/// ���� �ڵ�.
		i++;
		if (i > 1000000)
		{
			// ������ 1000000���̻��̳� �� ������ ����. (�����Ͱ� 100000���� �ƴ��̻�)
			// ���� 1000000�̻� ���Ҵٸ� Ȯ���� ���� ������ �ִ� ���̹Ƿ�
			TRACE("������ �鸸�� ���ҽ��ϴ�!");
			return;
		}
		/// ���� �ڵ�.
		///-----------------------------------------------

	}		// while()

	// ������� �Դٸ� while()�� ����ٴ� ���̰� �� ����
	// ��ȣ�� �ݾҴٴ� ���̹Ƿ�
	// �������� (����Լ�)

	return;
}


///----------------------------------------------------------------------
/// parsing�� ���� ������ �Լ���
///----------------------------------------------------------------------

// long�� �о �������ش�.
int CASEParser::Parsing_NumberLong()
{
	LONG			token;
	LONG			tempNumber;

	token = m_lexer->GetToken(m_TokenString);	//ASSERT(token == TOKEND_NUMBER);
	tempNumber = strtoul(m_TokenString, NULL, 10); //���ڿ��� ��ȣ ���� long ���������� ��ȯ

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
	/// ��m_TokenString ( char[255] ) �̱� ������ CString�� ������ ���� ���Ŷ� �����ߴµ�, �������� CString�� �� ������� �� ����. �˾Ƽ� �޾Ƶ��̴µ�?
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


// 3���� Float�� ���� �ϳ���
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

	return			tempVector3;		// ����ƽ ������ ���۷������ٴ� �� ������ ����.
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

	return			tempVector3;		// ����ƽ ������ ���۷������ٴ� �� ������ ����.
}

///--------------------------------------------------
/// ���ο��� ������ ����, ����Ʈ�� �ִ´�
///--------------------------------------------------
// �޽ø� �ϳ� �������� �����ϰ�, �� �����͸� ����Ʈ�� �ִ´�.
void CASEParser::Create_onemesh_to_list()
{
	GeomObject* temp = new GeomObject;
	m_OneMesh = temp;
	m_OneMesh->m_scenedata = m_scenedata;		// Ŭ������ �� ����
	m_MeshList.push_back(m_OneMesh);
}

// ��Ʈ���� �ϳ��� �������� �����ϰ�, �� �����͸� ����Ʈ�� �ִ´�.
void CASEParser::Create_materialdata_to_list()
{
	ASEMaterial* temp = new ASEMaterial;
	m_materialdata = temp;
	m_list_materialdata.push_back(m_materialdata);
}

// �ִϸ��̼ǵ����� �ϳ��� �������� �����ϰ�, �� �����͸� ����Ʈ�� �ִ´�.
void CASEParser::Create_animationdata_to_list()
{
	Animation* temp = new Animation;
	m_animation = temp;
	m_list_animation.push_back(m_animation);
}

// ���� �ϳ���..
void CASEParser::Create_onevertex_to_list()
{
	Vertex* temp = new Vertex;
	m_OneMesh->m_meshvertex.push_back(temp);
}