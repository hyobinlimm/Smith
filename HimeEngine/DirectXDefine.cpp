#include "DirectXDefine.h"

ID3D11ShaderResourceView* d3dHelper::CreateTexture2DArraySRV(ID3D11Device* device, ID3D11DeviceContext* context, std::vector<std::wstring>& filenames)
{
	//
	// Load the texture elements individually from file.  These textures
	// won't be used by the GPU (0 bind flags), they are just used to 
	// load the image data from file.  We use the STAGING usage so the
	// CPU can read the resource.
	UINT size = filenames.size();

	std::vector<ID3D11Resource*> srcTex(size);
	for (UINT i = 0; i < size; ++i)
	{
		ID3D11ShaderResourceView* shaderResourceView;
		HR(CreateDDSTextureFromFile(device,
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
	HR(device->CreateTexture2D(&texArrayDesc, 0, &texArray));

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
			context->CopySubresourceRegion(texArray,
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
	HR(device->CreateShaderResourceView(texArray, &viewDesc, &texArraySRV));

	// Cleanup--we only need the resource view.
	ReleaseCOM(texArray);

	for (UINT i = 0; i < size; ++i)
		ReleaseCOM(srcTex[i]);

	return texArraySRV;
}

ID3D11ShaderResourceView* d3dHelper::CreateRandomTexture1DSRV(ID3D11Device* device)
{
	// Create the random data.
	//
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

	//
	// Create the texture.
	//
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
	HR(device->CreateTexture1D(&texDesc, &initData, &randomTex));

	//
	// Create the resource view.
	//
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* randomTexSRV = 0;
	HR(device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV));

	ReleaseCOM(randomTex);

	return randomTexSRV;
}
