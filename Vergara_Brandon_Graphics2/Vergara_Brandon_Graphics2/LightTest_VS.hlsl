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


cbuffer OBJECT : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	//fromVertexBuffer.coordinate.y -= 1;

	float4 localH = { fromVertexBuffer.coordinate, 1 };

	localH = mul(localH, worldMatrix);
	localH = mul(localH, viewMatrix);
	localH = mul(localH, projectionMatrix);

	sendToRasterizer.projectedCoordinate = localH;

	float4 texTemp = localH;

	float4 tex = { fromVertexBuffer.texcoords, 1 };

	float4 normTemp = { fromVertexBuffer.norms, 0 };

	//normTemp.y -= 1;

	float4 inTemp = { fromVertexBuffer.coordinate, 1 };
	sendToRasterizer.texOut = tex;
	sendToRasterizer.normOut = normTemp;
	sendToRasterizer.normOut = normalize(sendToRasterizer.normOut);

	return sendToRasterizer;
}