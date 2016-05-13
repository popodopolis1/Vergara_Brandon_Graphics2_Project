#pragma pack_matrix(row_major)

struct INPUT_PIXEL
{
	float4 projectedCoordinate : SV_POSITION;
	float4 texOut : TEXTURE_COORDINATES;
	float4 normOut : NORMALS;
};

//struct Light
//{
//	float3 dir;
//	float4 ambient;
//	float4 diffuse;
//};
//
//cbuffer cbPerFrame : register(b1)
//{
//	Light light;
//};

Texture2D baseTexture : register(t0);

SamplerState filters[1] : register(s0);

float4 main(INPUT_PIXEL fromVS) : SV_TARGET
{
	fromVS.normOut = normalize(fromVS.normOut);

	float4 diffuse = baseTexture.Sample(filters[0], fromVS.texOut) * float4(1, 1, 1, 1);

	float4 newColor;

	//newColor = diffuse * light.ambient;
	//newColor += saturate(dot(light.dir, fromVS.normOut) * light.diffuse * diffuse);

	newColor[0] = diffuse[1];
	newColor[1] = diffuse[0];
	newColor[2] = diffuse[3];
	newColor[3] = diffuse[2];

	return newColor, diffuse.a;
}