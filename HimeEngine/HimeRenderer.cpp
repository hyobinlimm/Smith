
#include "HimeDevice.h"
#include "HimeFont.h"
#include "HimeCamera.h"
#include "DXObjectBase.h"
#include "HimeRenderer.h"
#include "DXTKUIObject.h"
#include "MeshObject.h"
#include "FireEffect.h"

HimeRenderer::HimeRenderer()
	: m_ClientWidth(0), m_ClientHeight(0),
	m_pFont(nullptr), m_pTexture2D(nullptr),
	m_LastFrame(0)
{

}

HimeRenderer::~HimeRenderer()
{
	delete m_pTexture2D;
	delete m_pFont;

}

void HimeRenderer::Initialize()
{
	m_pDirect3D = HimeDevice::GetInst();

	///TODO: TEST- 불효과
	m_Fire = new FireEffect();
	m_Fire->Initialize();
	m_Fire->SetEmitDir(XMFLOAT3(0.0f, 1.0f, 60.0f));

	m_pFont = new HimeFont();
	m_pFont->Create();

	m_pTexture2D = new DXTKUIObject();
	m_pTexture2D->Create();
}

void HimeRenderer::Update(HimeCamera* pCamera, float deltaTime)
{
	static UINT _aniFrameCount = 0;

	for (auto _DXObject : m_RenderingQueue)
	{
		if (_DXObject->m_Animation_List.size() != 0)
		{
			m_LastFrame = _DXObject->m_LastFrame;

			_DXObject->UpdateAnimation(_aniFrameCount);
			//_DXObject->CalcLocalTM();
		}
	}

	// 애니메이션으로 bone이 이동했다면 skin도 그만큼 이동 시키자. 
	UpdateBoneWorldTM(m_RenderingQueue);

	for (auto _DXObject : m_RenderingQueue)
	{
		_DXObject->Update(pCamera);
		int a = 0;
	}

	++_aniFrameCount;

	// 다시 맨 처음 프레임으로 돌아가기 위한 것
	if (_aniFrameCount > m_LastFrame)
	{
		_aniFrameCount = 0;
	}

	/// TEST - 불효과 
	m_Fire->Update(deltaTime, pCamera);
	m_Fire->SetEyePos(pCamera->GetPosition());
}

void HimeRenderer::Update(HimeCamera* pCamera, float deltaTime, MeshBuildInfo* pMeshInfo)
{
	//static float _prevTime = 0; 
	static float _time_elapsed = 0;
	_time_elapsed += deltaTime;

	// 애니메이션의 프레임에 대한 상수
	static int _aniFrameCount = 0;

	// 애니메이션이 있다면 적절한 애니메이션을 틀어주자. 
	for (auto _DXObject : m_RenderingQueue)
	{
		if (_DXObject->m_Animation_List.size() != 0)
		{
			m_LastFrame = _DXObject->m_LastFrame;

			_DXObject->UpdateAnimation(_aniFrameCount);
		}
	}

	// 애니메이션으로 bone이 이동했다면 skin도 그만큼 이동 시키자. 
	UpdateBoneWorldTM(m_RenderingQueue);

	for (auto _DXObject : m_RenderingQueue)
	{
		if (_DXObject->m_NodeName == pMeshInfo->NodeName + L"Col")
		{
			XMMATRIX _pos = ::XMMatrixTranslationFromVector(XMLoadFloat4(&pMeshInfo->Pos));
			XMMATRIX _rot = XMMatrixIdentity();
			XMMATRIX _scale = ::XMMatrixScalingFromVector(XMLoadFloat4(&pMeshInfo->Scale));

			XMMATRIX _localTM = _scale * _rot * _pos;

			XMStoreFloat4x4(&_DXObject->m_NodeData.m_LocalTM, _localTM);

			if (pMeshInfo->isDraw == true)
			{
				_DXObject->m_isDraw = true;
			}
			else if (pMeshInfo->isDraw == false)
			{
				_DXObject->m_isDraw = false;
			}
		}

		if (_DXObject->m_NodeName == pMeshInfo->NodeName)
		{
			XMVECTOR _p = XMLoadFloat4(&pMeshInfo->Pos);
			XMVECTOR _r = XMLoadFloat4(&pMeshInfo->Rot);
			XMMATRIX _pos = ::XMMatrixTranslationFromVector(_p);
			XMMATRIX _rot = ::XMMatrixRotationRollPitchYawFromVector(_r);
			XMMATRIX _scale = XMMatrixIdentity();
			const float PI = 3.1415926535f;
			const float Rad2Deg = 360 / (PI * 2);
			const float Deg2Rad = (PI * 2) / 360;
			_DXObject->m_MovePos = _p;
			if (_DXObject->m_NodeName == L"Center")
			{
				_DXObject->m_MoveRot = Vector3{ _r.m128_f32[0],_r.m128_f32[1] + (180 * Deg2Rad) ,_r.m128_f32[3] };
			}
			else if (_DXObject->m_ID == pMeshInfo->IDName)
			{
				_DXObject->m_MoveRot = Vector3{ _r.m128_f32[0],_r.m128_f32[1] ,_r.m128_f32[3] };
			}

			if (pMeshInfo->isDraw == true)
			{
				_DXObject->m_isDraw = pMeshInfo->isDraw;
			}
			else if (pMeshInfo->isDraw == false)
			{
				if (pMeshInfo->IDName == _DXObject->m_ID)
					_DXObject->m_isDraw = pMeshInfo->isDraw;
			}

			if (pMeshInfo->isInterAct == true)
			{
				_DXObject->m_LightCount = 4;
			}
			else if (pMeshInfo->isInterAct == false)
			{
				_DXObject->m_LightCount = 2;
			}

			if (pMeshInfo->ObjType == L"Bubble")
			{
				_DXObject->m_LightCount = 1;
			}
		}

	}
	for (auto _DXObject : m_RenderingQueue)
	{
		_DXObject->Update(pCamera);
	}
	// 1초에 60프레임을 그리도록 하는 함수. 
	if (_time_elapsed > (float)0.8f)
	{
		++_aniFrameCount;

		_time_elapsed = 0;
	}

	// 다시 맨 처음 프레임으로 돌아가기 위한 것
	if (m_LastFrame < _aniFrameCount)
	{
		_aniFrameCount = 0;
	}

	/// TEST - 불효과 
	m_Fire->Update(deltaTime, pCamera);
	m_Fire->SetEyePos(pCamera->GetPosition());
}

void HimeRenderer::TextUpdate(TextData* pTextData)
{
	for (auto _TextData : m_RenderText)
	{
		if (_TextData->IDName == pTextData->IDName)
		{
			_TextData->IDName = pTextData->IDName;
			_TextData->x = pTextData->x;
			_TextData->y = pTextData->y;
			_TextData->FontSize = pTextData->FontSize;
			_TextData->color = pTextData->color;
			_TextData->value = pTextData->value;
			_TextData->value2 = pTextData->value2;
			_TextData->value3 = pTextData->value3;
			_TextData->isDraw = pTextData->isDraw;
		}
	}

}

void HimeRenderer::TextureUpdate(UIData* pTextureData)
{
	for (auto _now : m_RenderUI)
	{
		if (_now->Name == pTextureData->Name)
		{
			/*_now->left = pTextureData->left;
			_now->top = pTextureData->top;
			_now->right = pTextureData->right;
			_now->bottom = pTextureData->bottom;*/
			_now->isDraw = pTextureData->isDraw;
		}
	}
}

void HimeRenderer::TextBufferClear()
{
	/// 다 그렸다면 text와 texture 렌더큐를 비워준다. 
	if (m_RenderText.size() != 0)
	{
		for (auto _now : m_RenderText)
		{
			if (_now->IDName != L"\0")
			{
				_now->IDName = L"";
			}
			if (_now->text != L"\0")
			{
				_now->text = L"";
			}
			delete _now;
			_now = nullptr;
		}
	}
	m_RenderText.clear();
}

void HimeRenderer::TextureBufferClaer()
{
	if (m_RenderUI.size() != 0)
	{
		for (auto _now : m_RenderUI)
		{
			if (_now->FileName != L"\0")
			{
				_now->FileName = L"";
			}
			delete _now;
			_now = nullptr;
		}
	}
	m_RenderUI.clear();
}

void HimeRenderer::UpdateBoneWorldTM(std::vector<DXObjectBase*> objects)
{
	std::vector<ASEParser::Bone*> _Bones;

	// skin오브젝트의 본 리스트를 저장
	for (auto _nowObj : objects)
	{
		if (_nowObj->m_IsSkinningObj == true)
		{
			_Bones = _nowObj->m_BoneList;
			break;
		}
	}

	if (_Bones.size() != 0)
	{
		// 본 리스트의 이름과 오브젝트의 이름을 찾아서 nodeTM를 넣어주자. 
		for (auto _bone : _Bones)
		{
			for (auto _nowObj : objects)
			{
				//string _nodeName(_nowObj->m_NodeName.begin(), _nowObj->m_NodeName.end());
				if (_bone->m_bone_name == _nowObj->m_stringName)
				{
					// boneObject가 애니메이션 되면서 TM이 바뀌기 때문에 업데이트 될때마다 받아와야함.
					MeshObject* _temp2;
					_temp2 = dynamic_cast<MeshObject*>(_nowObj);
					if (_temp2)
					{
						_bone->m_boneTM_WorldTM = _temp2->GetWorld();
					}
					break;
				}
			}
		}
	}


	if (_Bones.size() != 0)
	{
		for (int i = 0; i < _Bones.size(); i++)
		{
			_Bones.pop_back();
		}
	}
	_Bones.clear();

}

void HimeRenderer::BeginRender()
{
	m_pDirect3D->DXClearRenderTargetView();

	m_pDirect3D->GetDeviceContext()->OMSetDepthStencilState(m_pDirect3D->GetNormalDSS(), 0);

	// DXTX Font에서 블랜드 스테이트에 자신들의 기본값을 넣어준다. 
	// 그래서 오브젝트 텍스처가 블렌딩 되어버린다. 그 부분을 수정하기 위해, 
	// 블랜드 스테이트를 nullptr? 초기화?를 해줘야 한다. 
	const FLOAT blendFactor[4]{ };
	m_pDirect3D->GetDeviceContext()->OMSetBlendState(nullptr, blendFactor, 0xffffffff);

}

void HimeRenderer::Render()
{
	for (auto _DXObject : m_RenderingQueue)
	{
		if (_DXObject->m_isDraw == true) // 내가 현재 그려질 것인지 아닌지 검사하고 그린다.   
		{
			_DXObject->Render();
		}
	}

	//m_pTexture2D->DrawTexture(200, 200, 600, 600);
	for (auto _nowTex : m_RenderUI)
	{
		if (_nowTex->isDraw == true)
		{
			if (_nowTex->isMovable == true)
			{
				m_pTexture2D->DrawTexture(_nowTex->FileName, 0 + _nowTex->MoveX, 0 + _nowTex->MoveY, m_ClientWidth + _nowTex->MoveX, m_ClientHeight - _nowTex->MoveY / 3);
			}
			else if (_nowTex->isTurn == true)
			{
				m_pTexture2D->DrawTexture(_nowTex->FileName, _nowTex->Left, _nowTex->Top, _nowTex->Right, _nowTex->Bottom);
			}
			else
			{
				if (_nowTex->Type == L"Effect" || _nowTex->Type == L"Animation")
				{
					m_pTexture2D->RenderSprite(_nowTex->FileName, 0, 0, m_ClientWidth, m_ClientHeight, _nowTex->frame, _nowTex->width, _nowTex->height);
				}
				else if (_nowTex->Type == L"Clock")
				{
					m_pTexture2D->RotationSprite(_nowTex->FileName, 0, 0, m_ClientWidth, m_ClientHeight, _nowTex->rotate);
				}
				else
				{
					m_pTexture2D->DrawTexture(_nowTex->FileName, 0, 0, m_ClientWidth, m_ClientHeight);
				}
			}
		}
	}

	/// TEST - 불효과
	if (m_isFireDraw == true)
	{
		m_Fire->Render();
	}

	//m_RenderText에 무언가가 있다면 출력
	if (m_RenderText.size() != 0)
	{
		RenderText();
	}

	/// update할 때 렌더큐를 다시 받고, 그린 후 삭제를 하면
	/// update에서 굳이 for문으로 탐색을 할 필요가 없을 것 같다 - 신성현왈
	//m_RenderingQueue.clear();
}

void HimeRenderer::EndRender()
{
	m_pDirect3D->GetDeviceContext()->RSSetState(0);
	m_pDirect3D->DXPresent();
}

void HimeRenderer::SetFireEffect(bool checkDraw)
{
	m_isFireDraw = checkDraw;
}

void HimeRenderer::AddDXObjectToRenderQueue(DXObjectBase* dxObject)
{
	m_RenderingQueue.push_back(dxObject);
}

void HimeRenderer::CreateStirngData(int x, int y, float size, DirectX::XMFLOAT4 color, const char* text)
{
	/// 입력받은 문자를 textdata 구조체에 담는 함수 

	TextData* _newText = new TextData();

	_newText->x = x;
	_newText->y = y;
	_newText->FontSize = size;
	_newText->color = color;

	/// 이때 깊은 복사 해야함. 

	// 글자 갯수 나오는데	
	UINT len = strlen(text) + 1;

	// Wide Character => 글자 갯수 1개 당 2바이트
	TCHAR* _buffer = new TCHAR[len];

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, text, len, _buffer, len);

	_newText->text = _buffer;

	m_RenderText.emplace_back(_newText);

}

void HimeRenderer::CreateStirngData(TextData* pTextData)
{
	m_RenderText.emplace_back(pTextData);
}

void HimeRenderer::CreateTextureData(UIData* textureData)
{
	m_RenderUI.push_back(textureData);


	m_pTexture2D->CreateTextureList(textureData->FileName);
}

void HimeRenderer::RenderText()
{
	for (auto _nowTextBuffer : m_RenderText)
	{
		if (_nowTextBuffer->textValue != L"")
		{
			m_pFont->DrawTextColor(_nowTextBuffer->x, _nowTextBuffer->y, _nowTextBuffer->FontSize, _nowTextBuffer->color, _nowTextBuffer->isDraw, (TCHAR*)_nowTextBuffer->text.c_str(), _nowTextBuffer->textValue.c_str());
		}
		else
		{
			m_pFont->DrawTextColor(_nowTextBuffer->x, _nowTextBuffer->y, _nowTextBuffer->FontSize, _nowTextBuffer->color, _nowTextBuffer->isDraw, (TCHAR*)_nowTextBuffer->text.c_str(), _nowTextBuffer->value, _nowTextBuffer->value2, _nowTextBuffer->value3);
		}
	}
}
