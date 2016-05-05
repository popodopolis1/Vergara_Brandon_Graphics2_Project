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

	//unsigned int B = (color & 0xFF000000) >> 24;
	//unsigned int G = (color & 0x00FF0000) >> 16;
	//unsigned int R = (color & 0x0000FF00) >> 8;
	//unsigned int A = (color & 0x000000FF);
	//
	float4 newColor;
	newColor[0] = color[1];
	newColor[1] = color[0];
	newColor[2] = color[3];
	newColor[3] = color[2];

	return newColor;
}
