#pragma pack_matrix(row_major)

struct INPUT_PIXEL
{
	float4 projectedCoordinate : SV_POSITION;
	float4 texOut : TEXTURE_COORDINATES;
	float4 normOut : NORMALS;
};

TextureCube baseTexture : register(t0);

SamplerState filters[1] : register(s0);

float4 main(INPUT_PIXEL fromVS) : SV_TARGET
{
	float4 color = baseTexture.Sample(filters[0], fromVS.texOut) * float4(1, 1, 1, 1);

	//Color = tex2D(ColoredTextureSampler, input.textureCoordinates.xy);
	//Color += tex2D(ColoredTextureSampler, input.textureCoordinates.xy + (0.01));
	//Color += tex2D(ColoredTextureSampler, input.textureCoordinates.xy - (0.01));
	//Color = Color / 3;

	float4 newColor;
	newColor[0] = color[0];
	newColor[1] = color[1];
	newColor[2] = color[2];
	newColor[3] = color[3];

	return newColor;
}
