#include "DirectXDefine.h"
#include "ResourceManager.h"
#include "InputLayouts.h"
#include "Effects.h"
#include "FireEffect.h"
#include "HimeCamera.h"

FireEffect::FireEffect()
	: m_pVB(nullptr), m_pIB(nullptr), m_IndexSize(0),
	m_View(), m_Proj(),
	m_IndexCount(0),
	m_Material(),
	m_DirLights(),
	m_EyePosW(),
	m_TexTransform(),
	mInitVB(0), mDrawVB(0), mStreamOutVB(0), mTexArraySRV(0), mRandomTexSRV(0)

{
	::XMStoreFloat4x4(&m_TexTransform, XMMatrixIdentity());
	::XMStoreFloat4x4(&m_World, XMMatrixIdentity());

	m_pD3dDevice = HimeDevice::GetInst()->GetD3dDevice();
	m_pDeviceContext = HimeDevice::GetInst()->GetDeviceContext();
	m_pResourceManager = ResourceManager::GetInst();

	mFirstRun = true;
	mGameTime = 0.0f;
	mTimeStep = 0.0f;
	mAge = 0.0f;

	mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitDirW = XMFLOAT3(0.0f, 2.0f, 0.0f);
}

FireEffect::FireEffect(XMVECTOR pos)
	: m_pVB(nullptr), m_pIB(nullptr), m_IndexSize(0),
	m_View(), m_Proj(),
	m_IndexCount(0),
	m_Material(),
	m_DirLights(),
	m_EyePosW(),
	m_TexTransform()
{
	m_Position = pos;
	::XMStoreFloat4x4(&m_TexTransform, XMMatrixIdentity());
	::XMStoreFloat4x4(&m_World, XMMatrixIdentity());

	m_pD3dDevice = HimeDevice::GetInst()->GetD3dDevice();
	m_pDeviceContext = HimeDevice::GetInst()->GetDeviceContext();
	m_pResourceManager = ResourceManager::GetInst();

	mFirstRun = true;
	mGameTime = 0.0f;
	mTimeStep = 0.0f;
	mAge = 0.0f;

	mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitDirW = XMFLOAT3(0.0f, 1.0f, 0.0f);

	mD3DHelper = new d3dHelper();
}

FireEffect::~FireEffect()
{
	ReleaseCOM(mInitVB);
	ReleaseCOM(mDrawVB);
	ReleaseCOM(mStreamOutVB);

	ReleaseCOM(m_pDiffuseMapSRV);
	ReleaseCOM(m_pIB);
	ReleaseCOM(m_pVB);
}

float FireEffect::GetAge() const
{
	return mAge;
}

void FireEffect::SetEyePos(const XMFLOAT3& eyePosW)
{
	mEyePosW = eyePosW;
}

void FireEffect::SetEmitPos(const XMFLOAT3& emitPosW)
{
	mEmitPosW = emitPosW;
}

void FireEffect::SetEmitDir(const XMFLOAT3& emitDirW)
{
	mEmitDirW = emitDirW;
}

void FireEffect::Reset()
{
	mFirstRun = true;
	mAge = 0.0f;
}

void FireEffect::SetTexResource()
{
	{// Create the random data.
		XMFLOAT4 randomValues[1024];

		for (int i = 0; i < 1024; ++i)
		{
			randomValues[i].x = MathHelper::RandF(-1.0f, 1.0f);
			randomValues[i].y = MathHelper::RandF(-1.0f, 1.0f);
			randomValues[i].z = MathHelper::RandF(-1.0f, 1.0f);
			randomValues[i].w = MathHelper::RandF(-1.0f, 1.0f);
		}

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = randomValues;
		initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
		initData.SysMemSlicePitch = 0;

		// Create the texture.
		D3D11_TEXTURE1D_DESC texDesc;
		texDesc.Width = 1024;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.ArraySize = 1;

		ID3D11Texture1D* randomTex = 0;
		HR(m_pD3dDevice->CreateTexture1D(&texDesc, &initData, &randomTex));

		//
		// Create the resource view.
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
		viewDesc.Texture1D.MostDetailedMip = 0;

		ID3D11ShaderResourceView* randomTexSRV = 0;
		HR(m_pD3dDevice->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV));
		ReleaseCOM(randomTex);

		mRandomTexSRV = randomTexSRV;
	}

	std::vector<std::wstring> filenames;
	filenames.push_back(L"../Resource/Texture/flare0.dds");
	{
		UINT size = filenames.size();

		std::vector<ID3D11Resource*> srcTex(size);
		for (UINT i = 0; i < size; ++i)
		{
			ID3D11ShaderResourceView* shaderResourceView;
			HR(CreateDDSTextureFromFile(m_pD3dDevice,
				filenames[i].c_str(),
				&srcTex[i],
				&shaderResourceView));


			shaderResourceView->Release();
		}

		// Create the texture array.  Each element in the texture 
		// array has the same format/dimensions.
		D3D11_TEXTURE2D_DESC texElementDesc;
		((ID3D11Texture2D*)srcTex[0])->GetDesc(&texElementDesc);

		D3D11_TEXTURE2D_DESC texArrayDesc;
		texArrayDesc.Width = texElementDesc.Width;
		texArrayDesc.Height = texElementDesc.Height;
		texArrayDesc.MipLevels = texElementDesc.MipLevels;
		texArrayDesc.ArraySize = size;
		texArrayDesc.Format = texElementDesc.Format;
		texArrayDesc.SampleDesc.Count = 1;
		texArrayDesc.SampleDesc.Quality = 0;
		texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
		texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texArrayDesc.CPUAccessFlags = 0;
		texArrayDesc.MiscFlags = 0;

		ID3D11Texture2D* texArray = 0;
		HR(m_pD3dDevice->CreateTexture2D(&texArrayDesc, 0, &texArray));

		// Copy individual texture elements into texture array.
		// for each texture element...
		for (UINT texElement = 0; texElement < size; ++texElement)
		{
			// for each mipmap level...
			for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
			{
				const uint32_t subResourceIndex = mipLevel;
				const uint32_t destinationSubresource =
					D3D11CalcSubresource(mipLevel,
						texElement,
						texElementDesc.MipLevels);
				m_pDeviceContext->CopySubresourceRegion(texArray,
					static_cast<uint32_t> (destinationSubresource),
					0,
					0,
					0,
					srcTex[texElement],
					subResourceIndex,
					nullptr);
			}
		}

		// Create a resource view to the texture array.
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Texture2DArray.MostDetailedMip = 0;
		viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
		viewDesc.Texture2DArray.FirstArraySlice = 0;
		viewDesc.Texture2DArray.ArraySize = size;

		ID3D11ShaderResourceView* texArraySRV = 0;
		HR(m_pD3dDevice->CreateShaderResourceView(texArray, &viewDesc, &texArraySRV));

		// Cleanup--we only need the resource view.
		ReleaseCOM(texArray);

		for (UINT i = 0; i < size; ++i)
			ReleaseCOM(srcTex[i]);

		mTexArraySRV = texArraySRV;
	}
}

void FireEffect::Initialize()
{
	mMaxParticles = 300;

	SetTexResource();

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(VertexStruct::Particle) * 1;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// The initial particle emitter has type 0 and age 0.  The rest
	// of the particle attributes do not apply to an emitter.
	VertexStruct::Particle p;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &p;

	HR(m_pD3dDevice->CreateBuffer(&vbd, &vinitData, &mInitVB));

	//
	// Create the ping-pong buffers for stream-out and drawing.
	//
	vbd.ByteWidth = sizeof(VertexStruct::Particle) * mMaxParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	HR(m_pD3dDevice->CreateBuffer(&vbd, 0, &mDrawVB));
	HR(m_pD3dDevice->CreateBuffer(&vbd, 0, &mStreamOutVB));
}

void FireEffect::Update(float detalTime, HimeCamera* pCamera)
{
	mGameTime += detalTime;
	mTimeStep = detalTime;

	mAge += detalTime;

	mCamera = pCamera;

	::XMStoreFloat4x4(&m_View, pCamera->View());
	::XMStoreFloat4x4(&m_Proj, pCamera->Proj());
	m_EyePosW = XMFLOAT3(pCamera->GetPosition().x, pCamera->GetPosition().y, pCamera->GetPosition().z + 10);
}

void FireEffect::Render()
{
	XMFLOAT4X4 _tempPos;
	_tempPos._11 = 0.5f; _tempPos._12 = 0.f; _tempPos._13 = 0.f; _tempPos._14 = 0.f;
	_tempPos._21 = 0.f; _tempPos._22 = 0.5f; _tempPos._23 = 0.f; _tempPos._24 = 0.f;
	_tempPos._31 = 0.f; _tempPos._32 = 0.f; _tempPos._33 = 0.5f; _tempPos._34 = 0.f;
	_tempPos._41 = -0.9f; _tempPos._42 = 0.f; _tempPos._43 = 0.7f; _tempPos._44 = 1.f;

	XMMATRIX _tempViewProj = mCamera->ViewProj();
	XMMATRIX _Pos = _tempPos * _tempViewProj;

	Effects::FireFX->SetViewProj(_Pos);
	//Effects::FireFX->SetViewProj(mCamera->ViewProj());
	Effects::FireFX->SetGameTime(mGameTime);
	Effects::FireFX->SetTimeStep(mTimeStep);
	Effects::FireFX->SetEyePosW(mEyePosW);
	Effects::FireFX->SetEmitPosW(mEmitPosW);
	Effects::FireFX->SetEmitDirW(mEmitDirW);
	Effects::FireFX->SetTexArray(mTexArraySRV);
	Effects::FireFX->SetRandomTex(mRandomTexSRV);

	// 입력 배치 객체 셋팅
	m_pDeviceContext->IASetInputLayout(InputLayouts::Particle);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(VertexStruct::Particle);
	UINT offset = 0;

	// On the first pass, use the initialization VB.  Otherwise, use
	// the VB that contains the current particle list.
	if (mFirstRun)
		m_pDeviceContext->IASetVertexBuffers(0, 1, &mInitVB, &stride, &offset);
	else
		m_pDeviceContext->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	// Draw the current particle list using stream-out only to update them.  
	// The updated vertices are streamed-out to the target VB. 
	m_pDeviceContext->SOSetTargets(1, &mStreamOutVB, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::FireFX->StreamOutTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		Effects::FireFX->StreamOutTech->GetPassByIndex(p)->Apply(0, m_pDeviceContext);

		if (mFirstRun)
		{
			m_pDeviceContext->Draw(1, 0);
			mFirstRun = false;
		}
		else
		{
			m_pDeviceContext->DrawAuto();
		}
	}

	// done streaming-out--unbind the vertex buffer
	ID3D11Buffer* bufferArray[1] = { 0 };
	m_pDeviceContext->SOSetTargets(1, bufferArray, &offset);

	// ping-pong the vertex buffers
	std::swap(mDrawVB, mStreamOutVB);

	//
	// Draw the updated particle system we just streamed-out. 
	m_pDeviceContext->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	Effects::FireFX->DrawTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		Effects::FireFX->DrawTech->GetPassByIndex(p)->Apply(0, m_pDeviceContext);

		m_pDeviceContext->DrawAuto();
	}
}
