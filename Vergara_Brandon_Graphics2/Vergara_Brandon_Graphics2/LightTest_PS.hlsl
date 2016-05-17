#pragma pack_matrix(row_major)

struct INPUT_PIXEL
{
	float4 projectedCoordinate : SV_POSITION;
	float4 texOut : TEXTURE_COORDINATES;
	float4 normOut : NORMALS;
};

Texture2D baseTexture : register(t0);

SamplerState filters[1] : register(s0);

cbuffer Light : register(b0)
{
	float4 diffuse;
	float3 dir;
	float pad;
};

float4 main(INPUT_PIXEL fromVS) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	textureColor = baseTexture.Sample(filters[0], fromVS.texOut) * float4(1, 1, 1, 1);

	lightDir = -dir;

	lightIntensity = saturate(dot(lightDir, fromVS.normOut));

	color = saturate(diffuse*lightIntensity);

	color = color * textureColor;

	return color;
}