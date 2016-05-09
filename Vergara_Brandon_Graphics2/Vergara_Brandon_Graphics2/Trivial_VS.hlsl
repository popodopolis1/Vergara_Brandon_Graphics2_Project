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

	fromVertexBuffer.coordinate.y = fromVertexBuffer.coordinate.y * 2 - 1;;

	float4 localH = { fromVertexBuffer.coordinate, 1 };

	localH = mul(localH, worldMatrix);
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);

	sendToRasterizer.projectedCoordinate = localH;
		
	float4 texTemp = localH;

	float4 normTemp = { fromVertexBuffer.norms, 0 };

	normTemp.y = normTemp.y * 2 - 1;

	float4 inTemp = { fromVertexBuffer.coordinate, 1 };
	sendToRasterizer.texOut = inTemp;
	sendToRasterizer.normOut = normTemp;

	return sendToRasterizer;
}

