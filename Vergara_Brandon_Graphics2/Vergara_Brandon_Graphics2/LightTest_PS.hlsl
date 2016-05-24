#pragma pack_matrix(row_major)

struct INPUT_PIXEL
{
	float4 projCoor : SV_POSITION;
	float4 texOut : TEXTURE_COORDINATES;
	float4 normOut : NORMALS;
	float4 worldPos : POS;
};

Texture2D baseTexture : register(t0);

SamplerState filters[1] : register(s0);

cbuffer LIGHT : register(b0)
{
	float4 color;
	float3 dir;
	float pad;

	float4 color2;
	float3 dir2;
	float lightRadius;
	float3 pos;
	float pad2;

	float4 color3;
	float3 dir3;
	float coneRatio;
	float3 pos2;
	float pad3;

	float4 camPos;
};

float4 main(INPUT_PIXEL fromVS) : SV_TARGET
{
	float4 textureColor;
	textureColor = baseTexture.Sample(filters[0], fromVS.texOut);

	//Directional Light
	float dRatio = saturate(dot(-dir, fromVS.normOut));
	float4 dResult = dRatio * color;

	//Point Light
	float3 lightDir = normalize(pos.xyz - fromVS.worldPos.xyz);
	float pRatio = saturate(dot(-lightDir, fromVS.normOut));
	float atten = 1.0f - saturate(length(pos.xyz - fromVS.worldPos.xyz) / lightRadius);
	float4 pResult = pRatio * atten * color2;

	//Spotlight
	//float inRat = coneRatio / 2.0f;
	float3 lightDir2 = normalize(pos2.xyz - fromVS.worldPos.xyz);
	float surfaceRatio = saturate(dot(-lightDir2.xyz, dir3.xyz));
	//float spotFactor = (surfaceRatio > coneRatio) ? 1 : 0;
	float sRatio = saturate(dot(lightDir2, fromVS.normOut));
	float inCone = cos(20.0f * (3.1415f / 180.0f));
	float outCone = cos(30.0f * (3.1415f / 180.0f));
	float attenz = 1.0f - saturate((inCone - surfaceRatio) / (inCone - outCone));
	float4 sResult = sRatio * color3 * attenz;

	//Point light specular
	float3 specDir = normalize(camPos.xyz - fromVS.worldPos.xyz);
	float3 halfVec = normalize((lightDir) + specDir);
	float pIntensity = pow(saturate(dot(fromVS.normOut, normalize(halfVec))), 50.0f);
	float4 specResult = color2 * atten * pIntensity;

	//Add different lights to saturate and multiply EX. saturate(result + result2)
	return textureColor * saturate(dResult + pResult + sResult + specResult);

}