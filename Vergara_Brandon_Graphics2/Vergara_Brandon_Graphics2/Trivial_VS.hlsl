#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float3 coordinate : POSITION;
	float3 texcoords : TEXTURE_COORDINATES;
	float3 norms : NORMALS;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float4 texOut : TEXTURE_COORDINATES;
	float4 normOut : NORMALS;
};

// TODO: PART 3 STEP 2a

cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	// TODO : PART 4 STEP 4
	/*sendToRasterizer.projectedCoordinate.xy += constantOffset;*/

	// TODO : PART 3 STEP 7
	//sendToRasterizer.colorOut = constantColor;
	// END PART 3
	float4 localH = { fromVertexBuffer.coordinate, 1 };

	localH = mul(localH, worldMatrix);
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);


	sendToRasterizer.projectedCoordinate = localH;

	float4 texTemp = { fromVertexBuffer.texcoords, 0 };
	float4 normTemp = { fromVertexBuffer.norms, 0 };

	sendToRasterizer.texOut = texTemp;
	sendToRasterizer.normOut = normTemp;
	return sendToRasterizer;
}