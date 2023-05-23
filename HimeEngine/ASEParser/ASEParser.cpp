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
	// 0) ������ �ε��Ѵ�.
	if (!m_lexer->Open(p_File))
	{
		TRACE("������ ���� �߿� ������ �߻��߽��ϴ�!");
		return false;
	}

	// 1) ���� ��Ҹ� ��� �Ľ��Ѵ�.
	ParsingAll();

	return true;
}

bool ASE::ASEParser::LoadSceneData(Scenedata* pScene)
{
	// �� ����
	*pScene = m_scenedata;
	return true;
}

/// <summary>
/// �޽ø� ��� �״�� �����Ѵ�.
/// 
/// Face�������� ��� ���ؽ��� ���� �ٸ��ٰ� �����ϰ� ���(!) �߰��Ѵ�.
/// ���ؽ��� ��û ������ ���̴�.
/// </summary>
bool ASE::ASEParser::CopyMesh(GeometryObject* pMesh)
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
	TRACE("FaceCount : %d\n", _faceCount);

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		TRACE("FaceNum : %d / %d\n", i, _faceCount);

		for (int j = 0; j < 3; j++)
		{
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// face�������� ���ؽ��� �븻�� �־��ش�.
			_nowVertex->m_normal = pMesh->m_meshface[i]->m_normalvertex[j];

			if (pMesh->m_mesh_tvertex.size() != 0)
			{
				_nowVertex->u = pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_u;
				_nowVertex->v = pMesh->m_mesh_tvertex[pMesh->m_meshface[i]->m_TFace[j]]->m_v;
			}
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

	return false;
}

/// <summary>
/// �޽��� ����ȭ�� �� �ش�.
/// �븻 ��, �ؽ��� ��ǥ�� ���� ���ؽ��� �ø���, ��ø�Ǵ°��� �����ϰ�..
///
/// Face�������� ��� ���ؽ��� ���� �ٸ��ٰ� �����ϰ� �����, ������ ���ؽ��� �ߺ��Ǵ� ���� �ִ��� üũ �� �߰��Ѵ�.
/// �־��� ��� O(N^2)�̴�.
/// </summary>
bool ASE::ASEParser::OptimizeMesh(GeometryObject* pMesh)
{
	// �̰��� �޽ð� �ƴ϶�� ����
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
			// 1. Vertex�� �����Ѵ�.
			// ��, �����Ͱ� ������ �������� �ʴ´�.
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

			// 2. ���ݱ��� ���յ� Vertex�߿� ���� Vertex�� �ִ��� �˻��Ѵ�.
			int index = 0;
			for (unsigned int vlistindex = 0; vlistindex < pMesh->m_opt_vertex.size(); vlistindex++, index++)
			{
				// ���� ���ؽ��� ������ ���� ���յȰ��� �����ϰ� ������ ��������.
				if (pMesh->m_opt_vertex[index]->m_pos == (_pVertex)->m_pos &&
					pMesh->m_opt_vertex[index]->m_normal == (_pVertex)->m_normal &&
					pMesh->m_opt_vertex[index]->u == (_pVertex)->u &&
					pMesh->m_opt_vertex[index]->v == (_pVertex)->v)
				{
					TRACE("���� ���ؽ� ���� : %d\n", index);
					delete _pVertex;
					_pVertex = NULL;
					break;
				}
			}

			// vertex�� �������� �ʾҴٸ� ���� vertex�� ���ٴ� ���̹Ƿ�
			// ����Ʈ�� �߰��� ��, �ε����� �־��ش� ��ȣ�� index (0���� �����ϱ� ����, ���� for������ ++�� �Ǳ� ����)
			if (_pVertex != NULL)
			{
				pMesh->m_opt_vertex.push_back(_pVertex);
				pMesh->m_opt_index[i].index[j] = index;
			}
			// vertex�� �����Ǿ��� �� -> ���� Vertex�� �ִٴ� ��. �׿� �ش��ϴ� �ε����� index��.
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
	// ���ؽ��� ���� (�븻���� ���� �������� ����.)
	for (unsigned int i = 0; i < pMesh->m_meshvertex.size(); i++)
	{
		Vertex* _pVertex = new Vertex;
		_pVertex->m_pos = pMesh->m_meshvertex[i]->m_pos;
		_pVertex->m_indices = pMesh->m_meshvertex[i]->m_indices;

		pMesh->m_opt_vertex.push_back(_pVertex);
	}

	// �������� face�� �������� �� �ε����� ã�ƾ� ��
	unsigned int _faceCount = pMesh->m_meshface.size();
	TRACE("FaceCount : %d", _faceCount);

	/// ���ø��� ���ؽ��� ��ȣ �ο� �� ������ �����ϱ� ����.
	unsigned int split_Vertex_Num = 0 + pMesh->m_meshvertex.size();
	/// �� ���̽��� �Ҵ�Ǿ� �ִ� ���ؽ� �븻������ ���ϸ� ���ؽ��� �븻���� ä���ش�. 
	/// ���� ���ؽ��� �븻���� �ΰ��̻� ���� ����� ���ø��� ���ش�.
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
							/// ���� ���δٸ� ���ؽ� �븻������ ���� ������ �����͵� �߿����� �� �������� �ߺ��Ͽ� �ִٸ� ������ ���� �ʿ䰡 ����.
							if ((pMesh->m_opt_vertex[k]->m_indices == pMesh->m_meshface[i]->m_vertexindex[j]))
							{
								/// �븻���� �ٸ� �� ���� ����
								if ((pMesh->m_opt_vertex[k]->m_normal == pMesh->m_meshface[i]->m_normalvertex[j]))
								{
									pMesh->m_meshface[i]->m_vertexindex[j] = k;
									Is_Data_Exist = true;
									break;
								}

							}
						}
					}

					/// ���� ������ �����Ǿ��� ���ؽ��� ���ٸ�
					/// ���� ���ؽ��� ���ο� ���ؽ��� �����Ѵ�.
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
						// �Žÿ� ���εǴ� ���ؽ� ��ȣ�� �ٲ���� �ϰ���?!
						pMesh->m_meshface[i]->m_vertexindex[j] = split_Vertex_Num;
						split_Vertex_Num++;

						continue;
					}
				}
			}

			/// ���� �ؽ��İ� ���� �Ǿ��ִ��� uv���� �ٸ� �� ���� ����
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

					// �Žÿ� ���εǴ� ���ؽ� ��ȣ�� �ٲ���� �ϰ���?!
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
	// Lexer���� �� ���� ��ū�� �����´�.
	LONG _nowToken;

	// ��ū�� �� ���� �����鼭, ������ ó���� �� �ش�.
	while (_nowToken = m_lexer->GetToken(m_TokenString), _nowToken != TOKEND_END)
	{
		// �ֻ����� ��ū���� �о��ٸ�, �Ϻθ� �д� �Լ��� ȣ�� �� �ش�.
		// ������ ���� ���� �� ���� �ݺ��Ѵ�.
		// 
		// Top level category�� ID �� ���� �� ���� ����� ��� �߰�ȣ�� ���� �����Ѵ�.
		// ���ο��� �߰�ȣ�� ī�����ϸ� ����� ���� ������ �д´�.
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

			/// ���� Node�� ���� ������Ʈ��

			// Geometry Object
			case TOKENR_GEOMOBJECT:
			{
				// Geometry Object �ϳ��� ������ ����, �� ������ ��� �е��� �Ѵ�.
				// ���ο��� �߰�ȣ�� ����-�ݱ� ���� üũ�� �Ѵ�.
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
				// ����Ʈ�� �ִٴ� ���� �ǹ���.
				// �Ʒ��� ������ ���� ����Ʈ�� ���´�.
			}
			break;

			case TOKENR_MATERIAL_COUNT:
			{
				m_materialcount = Parsing_NumberLong();
			}
			break;

			case TOKENR_MATERIAL:
			{
				// �� Material�� �����ϰ� ��ȣ �Ű�����
				int _newMaterialNumber = Parsing_NumberLong();
				Parsing_Material(_newMaterialNumber);
			}
			break;

			// �� ���� �׷� ����
			case TOKENR_GROUP:
			{
				// �� ������ �̸��� ��Ʈ������ ������� �ϴµ�.
				/// �̰��� Ư�� ��� �� ��
				/// Depth 1 category�̸�, Hierarchy �������� �з� �Ǿ� �ִ�.
			}
			break;

			case TOKEND_END:
			{
				// ������ ���� ��Ÿ���� ��. while�� Ż���ؾ� �Ѵ�.
				TRACE("TRACE: �Ľ���: ������ ���� �ý��ϴ�!\n");
				return;
			}
			break;

			default:
			{
				// ���� ������ �ְų�
				// ó������ ���� ��ū�̴�.

				// �ƹ��͵� ���� �ʴ´�.
			}
			break;

		} // switch()

	} // while()


	// ���������� ���⿡ �Դٸ�, �� ���� ASE������ �� �о��ٴ� ���̴�.
	return;
}


// ���(Scene) ������ �Ľ��Ѵ�.
void ASE::ASEParser::Parsing_Scene()
{
	int _braceDepth = 0;

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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
				// ó������ ���� ��ū�̳�, �ٸ� �����ΰ��� ������ ��
				///case TOKENR_SCENE_ENVMAP:
			}
			break;

		} // switch()

	} // while()
}

// ���� Geometry Object�� ����, ����Ʈ�� �߰� �� �ش�.
void ASE::ASEParser::Parsing_GeometryObject()
{
	GeometryObject* _newObject = new GeometryObject();
	_newObject->m_scenedata = m_scenedata;		// �� ����. �ִϸ��̼� ��� �� ������Ʈ�� �˾ƾ� �ϱ� �����̴�.

	_newObject->m_type = eASEObjectType::Geometry;

	/// GEOMOBJECT ��ū �ڿ� �ٷ� ������ ������ ������ �ִ�.
	/// 1. �̰��� Ȯ�������� (�о� ����) ������ ó�� �� ������
	/// 2. while���� ������ Ư�� ��ū�� ���� �� ���� ������ �����̴�.
	/// ������ ������ Ȯ���� ���� while�� ������ ���̽��� ó���� �� �ص� �� �� ����.

	// ������ ������ ����Ǿ�� �Ѵ�. {} ¦�� ���� ī�����Ѵ�.
	int _braceDepth = 0;

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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
				// ���(ASE������Ʈ)���� ���� �� �� �ִ� ������ ���̴�.
				// ������Ʈ�� ������ ���� �־�� �� ���� �ٸ���.
				_newObject->m_nodename = Parsing_String();
			}
			break;

			case TOKENR_NODE_PARENT:
			{
				// �� ����� �θ� ����� ����
				_newObject->m_isparentexist = true;
				_newObject->m_nodeparent = Parsing_String();
			}
			break;

			// NodeTM�� �Ľ��Ѵ�.
			case TOKENR_NODE_TM:
			{
				Parsing_NodeTM(_newObject);
			}
			break;

			// Geometry Object�� Mesh ������ ������ �ִ�.
			case TOKENR_MESH:
			{
				Parsing_Mesh(_newObject);
			}
			break;

			// �ִϸ��̼� ������ �ִٸ� �д´�.
			case TOKENR_TM_ANIMATION:
			{
				Parsing_TMAnimation(_newObject);
			}
			break;

			/// Bone

			case TOKENR_MESH_NUMBONE:
			{
				// ���� ����. �̰� �ִٸ� �̰��� Skinned Mesh��� �Ǵ��Ѵ�.
				_newObject->m_is_skinningobject = true;
				_newObject->m_type = eASEObjectType::SkinnedMesh;
				_newObject->m_numbone = Parsing_NumberLong();
			}
			break;

			// �� (��Ű��) ������Ʈ�� ������ ����(Bone)�� ����
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
				/// ��� ü���� �� �ְ�, Bone�� �����ϰ� �ִٴ� ���� �̰��� ��Ű�� ������Ʈ��� ���̴�.
				_newObject->m_is_skinningobject = true;
				_newObject->m_type = eASEObjectType::SkinnedMesh;

				// �� �ϳ��� ���� �ӽ� ������ ����, ���Ϳ� �ְ�
				Bone* _tempbone = new Bone;
				_newObject->m_bone = _tempbone;
				_newObject->m_vector_bone_list.push_back(_tempbone);

				// Bone�� �ѹ��� �о� ����
				_newObject->m_bone->m_bone_number = Parsing_NumberLong();
			}
			break;

			// ���� �̸��� �־�� �Ѵ�.
			case TOKENR_BONE_NAME:
			{
				_newObject->m_bone->m_bone_name = Parsing_String();
			}
			break;

			case TOKENR_BONE_PROPERTY:
			{
				// �� ���� ABSOLUTE�� ������� �ϴµ�, �� ���� ����.
				// �������� TM_ROW0~3�� �����µ� ���� ���õ�..
			}
			break;

			case TOKENR_MESH_WVERTEXS:
			{

			}
			break;

			case TOKENR_MESH_WEIGHT:
			{
				// ���ؽ� �ϳ��� ������ ���� ����Ʈ�� ����
				VertexWeight* _wVertex = new VertexWeight;
				_newObject->m_wvertex = _wVertex;
				_newObject->m_vector_wvertexs.push_back(_wVertex);
				_newObject->m_wvertex->m_wvertex_number = Parsing_NumberLong();
			}
			break;

			case TOKENR_BONE_BLENGING_WEIGHT:
			{
				// ��ü �� �ܰ踦 ���°ž�...
				// ����ġ �Ѱ��� ���� ����Ʈ�� �ִ´�
				Weight* _weight = new Weight;
				_newObject->m_wvertex->m_temp_bone_blend_weight = _weight;
				_weight->m_bone_number = Parsing_NumberLong();
				_weight->m_bone_weight = Parsing_NumberFloat();
				_newObject->m_wvertex->m_bone_blending_weight.push_back(_weight);
				/// �� ��....
			}
			break;

			default:
			{
				// ó������ ���� ��ū�̳�, �ٸ� �����ΰ��� ������ ��
			}
			break;

		} // switch()

	} // while()

	m_ASEObjectList.push_back(_newObject);
}

// ���� Shape Object�� ����, ����Ʈ�� �߰� �� �ش�.
void ASE::ASEParser::Parsing_ShapeObject()
{
	GeometryObject* _newObject = new GeometryObject();
	_newObject->m_scenedata = m_scenedata;		// �� ����. �ִϸ��̼� ��� �� ������Ʈ�� �˾ƾ� �ϱ� �����̴�.

	_newObject->m_type = eASEObjectType::Shape;

	/// ������ ������ �����Ƿ� �̷��� �ص� �ȴ�. �ٸ�...
	// ������ ��� �Ľ��Ѵ�. ������ ������ ����Ǿ�� �Ѵ�. ���� {} ¦�� ���� ī�����ϰ� �ʹ�..
	//Parsing_NodeTM(_newObject);
	//Parsing_Shape(_newObject);
	//Parsing_TMAnimation(_newObject);

	/// ���� ����� ��ū�� ���� ����, �װ��� ����ް� �Ľ��ϴ� ���� ���� ���̴�.


	// ������ ������ ����Ǿ�� �Ѵ�. {} ¦�� ���� ī�����Ѵ�.
	int _braceDepth = 0;

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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
				// ���(ASE������Ʈ)���� ���� �� �� �ִ� ������ ���̴�.
				// ������Ʈ�� ������ ���� �־�� �� ���� �ٸ���.
				_newObject->m_nodename = Parsing_String();
			}
			break;

			case TOKENR_NODE_PARENT:
			{
				// �� ����� �θ� ����� ����
				_newObject->m_isparentexist = true;
				_newObject->m_nodeparent = Parsing_String();
			}
			break;

			// NodeTM�� �Ľ��Ѵ�.
			case TOKENR_NODE_TM:
			{
				Parsing_NodeTM(_newObject);
			}
			break;

			// Shape Object�� Shape ������ ������ �ִ�.
			case TOKENR_SHAPEOBJECT:
			{
				Parsing_Shape(_newObject);
			}
			break;

			// �ִϸ��̼� ������ �ִٸ� �д´�.
			case TOKENR_TM_ANIMATION:
			{
				Parsing_TMAnimation(_newObject);
			}
			break;

			default:
			{
				// ó������ ���� ��ū�̳�, �ٸ� �����ΰ��� ������ ��
			}
			break;

		} // switch()

	} // while()

	m_ASEObjectList.push_back(_newObject);
}

void ASE::ASEParser::Parsing_CameraObject()
{
	GeometryObject* _newObject = new GeometryObject();
	_newObject->m_scenedata = m_scenedata;		// �� ����. �ִϸ��̼� ��� �� ������Ʈ�� �˾ƾ� �ϱ� �����̴�.

	_newObject->m_type = eASEObjectType::Camera;

	// ������ ������ ����Ǿ�� �Ѵ�. {} ¦�� ���� ī�����Ѵ�.
	int _braceDepth = 0;

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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
				// ī�޶�� NodeTM�� �� �� ���´�.
				// �� ��°��� ���� �ʴ´�.
				if (_newObject->m_camera_isloadTarget) break;

				_newObject->m_nodename = Parsing_String();
			}
			break;

			case TOKENR_NODE_PARENT:
			{
				// �� ����� �θ� ����� ����
				_newObject->m_isparentexist = true;
				_newObject->m_nodeparent = Parsing_String();
			}
			break;

			// NodeTM�� �Ľ��Ѵ�.
			case TOKENR_NODE_TM:
			{
				Parsing_NodeTM(_newObject);
			}
			break;

			// ī�޶� ������ �Ľ��Ѵ�.
			case TOKENR_CAMERAOBJECT:
			{
				Parsing_CameraData(_newObject);
			}
			break;

			// �ִϸ��̼� ������ �ִٸ� �д´�.
			case TOKENR_TM_ANIMATION:
			{
				Parsing_TMAnimation(_newObject);
			}
			break;

			default:
			{
				// ó������ ���� ��ū�̳�, �ٸ� �����ΰ��� ������ ��
			}
			break;

		} // switch()

	} // while()

	m_ASEObjectList.push_back(_newObject);
}

void ASE::ASEParser::Parsing_LightObject()
{
	GeometryObject* _newObject = new GeometryObject();
	_newObject->m_scenedata = m_scenedata;		// �� ����. �ִϸ��̼� ��� �� ������Ʈ�� �˾ƾ� �ϱ� �����̴�.

	_newObject->m_type = eASEObjectType::Light;

	// ������ ������ ����Ǿ�� �Ѵ�. {} ¦�� ���� ī�����Ѵ�.
	int _braceDepth = 0;

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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
				// ���������� ������ Light�� ���� �ʴ´�.
				if (_newObject->m_light_isloadTarget) break;

				_newObject->m_nodename = Parsing_String();
			}
			break;

			case TOKENR_NODE_PARENT:
			{
				// �� ����� �θ� ����� ����
				_newObject->m_isparentexist = true;
				_newObject->m_nodeparent = Parsing_String();
			}
			break;

			// NodeTM�� �Ľ��Ѵ�.
			case TOKENR_NODE_TM:
			{
				Parsing_NodeTM(_newObject);
			}
			break;

			// �� ������ �Ľ��Ѵ�.
			case TOKENR_LIGHTOBJECT:
			{
				Parsing_LightData(_newObject);
			}
			break;

			// �ִϸ��̼� ������ �ִٸ� �д´�.
			case TOKENR_TM_ANIMATION:
			{
				Parsing_TMAnimation(_newObject);
			}
			break;

			default:
			{
				// ó������ ���� ��ū�̳�, �ٸ� �����ΰ��� ������ ��
			}
			break;

		} // switch()

	} // while()

	m_ASEObjectList.push_back(_newObject);
}

void ASE::ASEParser::Parsing_HelperObject()
{
	GeometryObject* _newObject = new GeometryObject();
	_newObject->m_scenedata = m_scenedata;		// �� ����. �ִϸ��̼� ��� �� ������Ʈ�� �˾ƾ� �ϱ� �����̴�.

	_newObject->m_type = eASEObjectType::Helper;

	// ������ ������ ����Ǿ�� �Ѵ�. {} ¦�� ���� ī�����Ѵ�.
	int _braceDepth = 0;

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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
				// ���(ASE������Ʈ)���� ���� �� �� �ִ� ������ ���̴�.
				// ������Ʈ�� ������ ���� �־�� �� ���� �ٸ���.
				_newObject->m_nodename = Parsing_String();
			}
			break;

			case TOKENR_NODE_PARENT:
			{
				// �� ����� �θ� ����� ����
				_newObject->m_isparentexist = true;
				_newObject->m_nodeparent = Parsing_String();
			}
			break;

			// NodeTM�� �Ľ��Ѵ�.
			case TOKENR_NODE_TM:
			{
				Parsing_NodeTM(_newObject);
			}
			break;

			// �ִϸ��̼� ������ �ִٸ� �д´�.
			case TOKENR_TM_ANIMATION:
			{
				Parsing_TMAnimation(_newObject);
			}
			break;

			default:
			{
				// ó������ ���� ��ū�̳�, �ٸ� �����ΰ��� ������ ��
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
/// �� ���� ��� ��ȯ ����� �Ľ��Ѵ�.
/// </summary>
void ASE::ASEParser::Parsing_NodeTM(GeometryObject* targetObject)
{
	// ������ ������ ����Ǿ�� �Ѵ�. {} ¦�� ���� ī�����Ѵ�.
	int _braceDepth = 0;

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
		_braceDepth++;
	}

	LONG _nowToken;

	/// ��ū�� �� ���� �����鼭, ������ ó���� �� �ش�.
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
				// (NODE_TM���� ���� �� NODE_NAME�� �ѹ� �� ���´�.)
				// (Animation���� ������ �ؾ� �ϱ� ������ �̷��� ��带 �����ش�)

				/// �Դٰ�,
				// ī�޶�� NodeTM�� �ι� ���´�. �ι�°��� ���� �ʴ´�.
				// �ƿ� �� ��Ϳ��� ������ ��Ű�� ������. �߰��� �д°��� �����ؾ� �ϱ� ������...
				//if (targetObject->m_camera_isloadTarget) {
				//	'}'�� ���ö����� �����°� ������ ����! �� �̷��� �ȵǳ�..
				// ��ͱ����� �������̴�....

				break;

			case TOKENR_INHERIT_POS:
			{
				// ī�޶�� NodeTM�� �ι� ���´�. �ι�°��� ���� �ʴ´�.
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

				// SKIN_INITTM�� ��� �����Ѵ�.
				///if (m_parsingmode == eSkin_InitTM) break;
				///if (m_parsingmode == eBone_List) break;
				targetObject->m_tm_row0.x = Parsing_NumberFloat();
				targetObject->m_tm_row0.y = Parsing_NumberFloat();	/// yz�� INodeTM���� �ٲ۴�. Ư���� ������ ���ٸ� ���⿡�� �̸� �ٲ���..
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

				// ���� ī�޶� ���¿��ٸ� �̹� ��带 ���� ������ ǥ�����ش�.
				if (targetObject->m_type == eASEObjectType::Camera) targetObject->m_camera_isloadTarget = true;
				if (targetObject->m_type == eASEObjectType::Light) targetObject->m_light_isloadTarget = true;
			}
			break;

			// �ƹ� ��ū���� �ش����� ���� ��
			default:
			{
				// ó������ ���� ��ū�̳�, �ٸ� �����ΰ��� ������ ��
			}
			break;

		} // switch()

	} // while()
}

void ASE::ASEParser::Parsing_TMAnimation(GeometryObject* targetObject)
{
	// �ִϸ��̼� �����͸� ����, ����Ʈ�� �־��ش�.
	Animation* _newAnimation = new Animation;
	m_list_animation.push_back(_newAnimation);

	// ������Ʈ�� �ִϸ��̼� �����Ͱ� ������ ����Ѵ�.
	targetObject->m_isAnimated = true;

	// �� �����κ��� �ִϸ��̼� ƽ ������ �����´�.
	_newAnimation->m_ticksperFrame = m_scenedata.m_ticksperframe;
	_newAnimation->m_minKeyframe = m_scenedata.m_ticksperframe;

	// ����ؼ� ������ �д´�.
	Parsing_TMAnimationData(_newAnimation);
}

void ASE::ASEParser::Parsing_TMAnimationData(Animation* animation)
{
	Matrix TM1, TM2;
	Quaternion tempQT;
	Quaternion prevQT;
	Quaternion resultQT;

	// ������ ������ ����Ǿ�� �Ѵ�. {} ¦�� ���� ī�����Ѵ�.
	int _braceDepth = 0;

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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

			// �� �� ������ �׳� ��ȣ�� ���� �͹ۿ��� �ϴ� ���� ����.
			case TOKENR_CONTROL_POS_TRACK:
				break;

			case TOKENR_CONTROL_ROT_TRACK:
				break;

			case TOKENR_CONTROL_SCALE_TRACK:
				break;

			case TOKENR_CONTROL_POS_SAMPLE:
			{
				// �������� �ϳ� �����, �а�, ���Ϳ� �ִ´�
				CAnimation_pos* _pos = new CAnimation_pos;
				_pos->m_time = Parsing_NumberInt();
				_pos->m_pos.x = Parsing_NumberFloat();
				_pos->m_pos.z = Parsing_NumberFloat();
				_pos->m_pos.y = Parsing_NumberFloat();

				// m_position�� vector�̴�. �ڿ� �� ���� �߰����� �ش�.
				animation->m_position.push_back(_pos);
			}
			break;

			case TOKENR_CONTROL_ROT_SAMPLE:
			{
				// �����̼��� �ϳ� �����, �а�, ���Ϳ� �ִ´�
				CAnimation_rot* _rot = new CAnimation_rot;
				_rot->m_time = Parsing_NumberInt();
				_rot->m_rot.x = Parsing_NumberFloat();
				_rot->m_rot.z = Parsing_NumberFloat();
				_rot->m_rot.y = Parsing_NumberFloat();
				_rot->m_angle = Parsing_NumberFloat();

				/// ���ݱ����� ������ �о ����, ���ʹϾ����� ����� �ִ´�.
				//Quaternion tempQT;	(���ʿ� ������ִ�)
				//Quaternion prevQT;
				//Quaternion resultQT;

				// ���� ���ʹϿ��� ���ؼ�,
				tempQT = XMQuaternionRotationAxis(_rot->m_rot, _rot->m_angle);

				/// ���ʹϿ��� ����� �Ű������� Ȯ���ߴ�.
				TM1 = XMMatrixRotationAxis(_rot->m_rot, _rot->m_angle);
				TM2 = XMMatrixRotationQuaternion(tempQT);

				// ������ ����� 0�̶�� ���� ������ ���� ���̹Ƿ� �׳� �ְ�
				if (animation->m_rotation.size() == 0)
				{
					// ���� ���� �״�� �־��ش�.
					_rot->m_rotQT_accumulation = tempQT;
					animation->m_rotation.push_back(_rot);
				}
				else
				{
					// �ƴ϶�� ���� ������ �ִ� ���̹Ƿ� ���� ����� Multiply
					// �̰� ���� ����
					prevQT = animation->m_rotation[animation->m_rotation.size() - 1]->m_rotQT_accumulation;
					resultQT = XMQuaternionMultiply(prevQT, tempQT);
					_rot->m_rotQT_accumulation = resultQT;
					animation->m_rotation.push_back(_rot);
				}
			}
			break;

			case TOKENR_CONTROL_SCALE_SAMPLE:
			{
				// �������� �ϳ� ����� ���Ϳ� �ִ´�
				CAnimation_scl* _scl = new CAnimation_scl;
				_scl->m_time = Parsing_NumberInt();
				_scl->m_scale = Parsing_NumberVector3_YZ();
				_scl->m_scaleaxis = Parsing_NumberVector3_YZ();
				_scl->m_scaleaxisang = Parsing_NumberFloat();

				// ���� ���ʹϿ�
				tempQT = XMQuaternionRotationAxis(_scl->m_scaleaxis, _scl->m_scaleaxisang);

				// ������ ����� 0�̶�� ���� ������ ���� ���̹Ƿ� �׳�
				if (animation->m_scale.size() == 0)
				{
					// ���� ���� �״�� �־��ش�.
					_scl->m_scalerotQT_accumulation = tempQT;
				}
				else
				{
					// �ƴ϶�� ���� ������ �ִ� ���̹Ƿ� ���� ����� Multiply
					// �̰� ���� ����
					prevQT = animation->m_scale[animation->m_scale.size() - 1]->m_scalerotQT_accumulation;
					resultQT = XMQuaternionMultiply(prevQT, tempQT);
					_scl->m_scalerotQT_accumulation = resultQT;

				}

				// ���Ϳ� �־��ش�.
				animation->m_scale.push_back(_scl);
			}
			break;

		} // switch()

	} // while()
}

void ASE::ASEParser::Parsing_Mesh(GeometryObject* targetObject)
{
	int _braceDepth = 0;

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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
				// ���ؽ��� ������ ����־�� �ϴµ�
				// �̹� ���ͷ� ������ �� �����Ƿ� �׳� ������ �ȴ�.
				break;

			case TOKENR_MESH_VERTEX:
			{
				// ������ �Է�

				int vertexnumber;
				vertexnumber = Parsing_NumberLong();		// ù��°�� ���ؽ��� ��ȣ

				// ������ ��� �����غ�����, �Ҵ�� ũ�⸸ ��Ȯ�ϴٸ�
				// vertex���� ��ü�� ��ȣ�� �ֱ� ������ ī��Ʈ��, ������ �ʿ����!

				// ���ؽ� ���Ϳ� �����͸� �ϳ� ����
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
				facenumber = Parsing_NumberLong();			// Face�� ��ȣ�ε�...

				Parsing_String();		// A:�� �а�
				temp->m_vertexindex[0] = Parsing_NumberLong();
				Parsing_String();		// B:
				temp->m_vertexindex[2] = Parsing_NumberLong();	/// 021�ΰ��� y-z��ǥ�� ��ȯ ����
				Parsing_String();		// C:
				temp->m_vertexindex[1] = Parsing_NumberLong();

				/// (�ڿ� ������ �� ������ default�� ���� ��ŵ�� ���̴�.)
				/// ......

				static int count = 0;
				count++;
				// ���Ϳ� �־��ش�.
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
				// ���ؽ��� �ε����� �����µ� ������ ������ �����Ƿ� ������.
				Parsing_NumberLong();

				// ���ο� TVertex�� ���� ���Ϳ� �ִ´�
				COneTVertex* temp = new COneTVertex;
				temp->m_u = Parsing_NumberFloat();
				temp->m_v = 1.0f - Parsing_NumberFloat();		/// y-z ������ ������ v�� 1-v�� ����� ��
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
				facenumber = Parsing_NumberLong();	// Face�� ��ȣ. TVFaces�� VFaces�� ������ ���ٰ� �����Ѵ�. (�׷��ٸ� �ٷ� ���� NUMTVFACES�� �� ������? �ϴ� �ǹ��� ���� ������.)
				targetObject->m_meshface[facenumber]->m_TFace[0] = Parsing_NumberLong();
				targetObject->m_meshface[facenumber]->m_TFace[2] = Parsing_NumberLong();/// 021�ΰ��� y-z��ǥ�� ��ȯ ����
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
				// ���̽� �븻 ������ �Է�
				int facenumber;
				facenumber = Parsing_NumberLong();			// Face�� ��ȣ�ε�...

				targetObject->m_meshface[facenumber]->m_normal.x = Parsing_NumberFloat();
				targetObject->m_meshface[facenumber]->m_normal.z = Parsing_NumberFloat();
				targetObject->m_meshface[facenumber]->m_normal.y = Parsing_NumberFloat();

				///case TOKENR_MESH_VERTEXNORMAL:
				for (int i = 0; i < 3; i++)
				{
					if (m_lexer->GetToken(m_TokenString) == TOKENR_MESH_VERTEXNORMAL)
					{
						//	�ϴ� ���� �ϳ��� �дµ� �̰��� ���ؽ��� �ε���.
						Parsing_NumberLong();

						// 021������ ��������..
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

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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
				// ����Ʈ�� �� ���� ������ �߰��Ѵ�.
				ShapeLine* temp = new ShapeLine;
				m_nowshapeline = temp;
				m_nowshapeline->m_line_number = Parsing_NumberLong();		// ���� ���� �����ΰ�?
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
				temp->m_pos = Parsing_NumberVector3_YZ();		/// ��������� �ѹ� �� �ô�. ���߿� �� �̰����� ��ü����.

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

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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
				// ī�޶� ������ �д´�. �� �ٸ��� ����.
				break;

				//case TOKENR_TIMEVALUE:		//-> �̰� �̹� MESH�� �ִ�.

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

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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
				targetObject->m_light_type = m_TokenString;	// �Ƹ��� target
				break;

			case TOKENR_LIGHT_SHADOWS:
				Parsing_String();
				targetObject->m_light_shadows = m_TokenString;	// �Ƹ��� target
				break;

			case TOKENR_LIGHT_USELIGHT:
				targetObject->m_light_uselight = Parsing_NumberLong();
				break;

			case TOKENR_LIGHT_SPOTSHAPE:
				Parsing_String();
				targetObject->m_light_spotshape = m_TokenString;	// �Ƹ��� target
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
				// Light Setting���� ����
				// �����ϸ鼭, �ϳ��� SettingŬ���� �����͸� �����,
				// �� �����͸� ����Ʈ�� �ִ´�.
			{
				// ���ο� �����͸� �����, �װ��� ����Ʈ�� �ִ´�.
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
				// �ִϸ��̼� �������� �˸���, �� �������� ������ LIGHT_SETTING��
				// ����Ʈ�� �����, �־�� �Ѵ�.
				targetObject->m_light_isAnimate = true;
				break;

		} // switch()

	} // while()
}

void ASE::ASEParser::Parsing_Misc(GeometryObject* targetObject)
{
	int _braceDepth = 0;

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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
				// (���� ���͸��� ��ȣ�� �а�)
				//
			}
			break;

			/// MATERIAL - MAP_DIFFUSE ���

			// �� �ϳ��� �����, ���� Material�� �־� �ش�
			case TOKENR_MAP_SELFILLUM:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// �ӽ÷� ���̴� '����' �� ������
			}
			break;

			// �� �ϳ��� �����, ���� Material�� �־� �ش�
			case TOKENR_MAP_SHINE:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// �ӽ÷� ���̴� '����' �� ������
			}
			break;

			// �� �ϳ��� �����, ���� Material�� �־� �ش�
			case TOKENR_MAP_BUMP:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// �ӽ÷� ���̴� '����' �� ������
			}
			break;

			// �� �ϳ��� �����, ���� Material�� �־� �ش�
			case TOKENR_MAP_OPACITY:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// �ӽ÷� ���̴� '����' �� ������
			}
			break;

			// �� �ϳ��� �����, ���� Material�� �־� �ش�
			case TOKENR_MAP_REFLECT:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// �ӽ÷� ���̴� '����' �� ������
			}
			break;

			// �� �ϳ��� �����, ���� Material�� �־� �ش�
			case TOKENR_MAP_GENERIC:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// �ӽ÷� ���̴� '����' �� ������
			}
			break;

			// �� �ϳ��� �����, ���� Material�� �־� �ش�
			case TOKENR_MAP_SPECULAR:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// �ӽ÷� ���̴� '����' �� ������
			}
			break;

			// �� �ϳ��� �����, ���� Material�� �־� �ش�
			case TOKENR_MAP_DIFFUSE:
			{
				material->m_map_diffuse = new MaterialMap;
				material->m_map_temp = material->m_map_diffuse;	// �ӽ÷� ���̴� '����' �� ������
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
				// ��Ʈ���� �Ѱ��� ������ �ؽ��İ� �ִ� ���̴�
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

	// �ش��ϴ� ��ū�� ù ��° �߰�ȣ�� �а� depth�� �÷� ���´�.
	if (m_lexer->GetToken(m_TokenString) != TOKEND_BLOCK_START)
	{
		// ���� �߸� �� ���̴�.
		TRACE("�Ľ� �� ���� �߸���");
		return;
	}
	else
	{
		// ù ��° �߰�ȣ�� �о����Ƿ� ���� ����
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
				// ó������ ���� ��ū�̳�, �ٸ� �����ΰ��� ������ ��
			}
			break;

		} // switch()

	} // while()
}

///--------------------------------------------------
/// Parsing�� ���� ������ �Լ���

// long�� �о �������ش�.
int ASE::ASEParser::Parsing_NumberLong()
{
	LONG _token;
	LONG _numberLong;

	_token = m_lexer->GetToken(m_TokenString);
	_numberLong = strtoul(m_TokenString, NULL, 10);

	return _numberLong;
}

// float�� �о �������ش�.
float ASE::ASEParser::Parsing_NumberFloat()
{
	LONG _token;
	float numberFloat;

	_token = m_lexer->GetToken(m_TokenString);
	numberFloat = (float)atof(m_TokenString);

	return numberFloat;
}

// String�� �о �������ش�.
LPSTR ASE::ASEParser::Parsing_String()
{
	// m_TokenString - char[255]
	m_lexer->GetToken(m_TokenString);

	return m_TokenString;
}

// int�� �о �������ش�.
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

// 3���� Float�� ���� �ϳ��� �������ش�.
// (Max�� DX�� ��ǥ���̷� yz��ȯ)
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






