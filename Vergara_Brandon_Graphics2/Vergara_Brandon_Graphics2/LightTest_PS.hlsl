#pragma pack_matrix(row_major)

struct INPUT_PIXEL
{
	float4 projectedCoordinate : SV_POSITION;
	float4 texOut : TEXTURE_COORDINATES;
	float4 normOut : NORMALS;
};

Texture2D baseTexture : register(t0);

SamplerState filters[1] : register(s0);

cbuffer LIGHT : register(b0)
{
	float4 color;
	float3 dir;
	float pad;
};

float4 main(INPUT_PIXEL fromVS) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 col;

	textureColor = baseTexture.Sample(filters[0], fromVS.texOut);

	//lightDir = -dir;
	//
	//lightIntensity = saturate(dot(lightDir, fromVS.normOut));
	//
	//col = saturate(color*lightIntensity);
	//
	//col = col * textureColor;

	return textureColor;
}