//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

#include "LightHelper.fx"

cbuffer cbPerFrame
{
	DirectionalLight gDirLights[3];
	float3 gEyePosW;

	float  gFogStart;
	float  gFogRange;
	float4 gFogColor;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
};

cbuffer cbSkinned
{
	float4x4 gBoneTransforms[96];
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;

SamplerState samAnisotropic
{
	//Filter = ANISOTROPIC;
	Filter = MIN_MAG_MIP_LINEAR;

	MaxAnisotropy = 4;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;

	/// skinning 추가 코드
	float4 Weight1  : WEIGHT;
	float4 Weight2  : WEIGHTS;
	float4 Weight3  : WEIGHTSS;

	uint4 BoneIndices1 : BONEINDICES;
	uint4 BoneIndices2 : BONEINDICESS;
	uint4 BoneIndices3 : BONEINDICESSS;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex     : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to world space space.
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}

// Skinning 추가 코드 
VertexOut SkinnedVS(VertexIn vin)
{
	VertexOut vout;

	float totalWeight = 0.f;

	// Init array or else we get strange warnings about SV_POSITION.
	// 버텍스 가중치 배열을 초기화한다. 아니면 SV_POSITION 값에 대한 이상한 워닝을 볼 것이다.
	float weights1[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights1[0] = vin.Weight1.x;
	weights1[1] = vin.Weight1.y;
	weights1[2] = vin.Weight1.z;
	weights1[3] = vin.Weight1.w;

	float weights2[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights2[0] = vin.Weight2.x;
	weights2[1] = vin.Weight2.y;
	weights2[2] = vin.Weight2.z;
	weights2[3] = vin.Weight2.w;

	float weights3[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights3[0] = vin.Weight3.x;
	weights3[1] = 0.f;
	weights3[2] = 0.f;
	weights3[3] = 0.f;

	for (int i = 0; i < 4; i++)
	{
		totalWeight += weights1[i];
		totalWeight += weights2[i];
		totalWeight += weights3[i];
	}

	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		// Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.S
		// 논유니폼 스케일이 없다고 가정하므로, 노말값을 위한 역행렬의 전치행렬이 필요없다.
		posL += weights1[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices1[i]]).xyz;
		normalL += weights1[i] * mul(vin.NormalL, (float3x3) gBoneTransforms[vin.BoneIndices1[i]]);
	}

	for (int j = 0; j < 4; ++j)
	{
		posL += weights2[j] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices2[j]]).xyz;
		normalL += weights2[j] * mul(vin.NormalL, (float3x3) gBoneTransforms[vin.BoneIndices2[j]]);
	}

	for (int k = 0; k < 4; ++k)
	{
		posL += weights3[k] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices3[k]]).xyz;
		normalL += weights3[k] * mul(vin.NormalL, (float3x3) gBoneTransforms[vin.BoneIndices3[k]]);
	}

	posL /= totalWeight;
	normalL /= totalWeight;


	// Transform to world space space.
	// 월드 스페이스로 변환
	vout.PosW = mul(float4(posL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(normalL, (float3x3)gWorldInvTranspose); // 이때는 역전치행렬을 쓴다.

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(posL, 1.0f), gWorldViewProj);

	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}

float4 PS(VertexOut pin, uniform int gLightCount, uniform bool gUseTexure) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;
	
	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);
	
	// Normalize.
	toEye /= distToEye;
	
	// Default to multiplicative identity.
	float4 texColor = float4(1, 1, 1, 1);
	if (gUseTexure)
	{
		// Sample texture.
		texColor = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
	}
	 
	//
	// Lighting.
	float4 litColor = texColor;
	if (gLightCount > 0)
	{
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
		// Sum the light contribution from each light source.  
		[unroll]
		for (int i = 0; i < gLightCount; ++i)
		{
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEye,
				A, D, S);
	
			ambient += A;
			diffuse += D;
			spec += S;
		}
	
		// Modulate with late add.
		litColor = texColor * (ambient + diffuse) + spec;
	}
	
	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;
	litColor.a = 1.f;
	
	// return float4(pin.NormalW, 1.0f);
	return litColor;
}

technique11 Light1
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(1, false)));
	}
}

technique11 Light2
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(2, false)));
	}
}

technique11 Light3
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(3, false)));
	}
}

technique11 Light0Tex
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(0, true)));
	}
}

technique11 Light1Tex
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(1, true)));
	}
}

technique11 Light2Tex
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(2, true)));
	}
}

technique11 Light3Tex
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(3, true)));
	}
}

// 테크닉 바꿔주기
// ==================== Skinnig 추가 코드 ====================
technique11 Light0TexSkin
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(0, true)));
	}
}

technique11 Light1TexSkin
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(1, true)));
	}
}

technique11 Light2TexSkin
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(2, true)));
	}
}

technique11 Light3TexSkin
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(3, true)));
	}
}