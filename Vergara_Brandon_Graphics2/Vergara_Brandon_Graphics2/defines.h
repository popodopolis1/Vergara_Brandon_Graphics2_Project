#include <windows.h>
#include <iostream>
#include <cmath>
#include "XTime.h"
#include "SharedDefines.h"
#include <DirectXMath.h>
using namespace std;
using namespace DirectX;

#define SAFE_RELEASE(p) {if(p{p->Release(); p = nullptr;})}

#define PI 3.14159f

struct Vertex3
{
	XMFLOAT4 e[4];
	XMFLOAT2 uv[2];
	uint color;
};

struct Vertex4
{
	float e[4];
	unsigned int color;
};

struct Matrix3x3
{
	union
	{
		float e[9];

		struct
		{
			float _e11, _e12, _e13;
			float _e21, _e22, _e23;
			float _e31, _e32, _e33;
		};
	};
};

struct Matrix4x4
{

	XMMATRIX e;
	//union
	//{
	//	float e[16];
	//
	//	struct
	//	{
	//		float _e11, _e12, _e13, _e14;
	//		float _e21, _e22, _e23, _e24;
	//		float _e31, _e32, _e33, _e34;
	//		float _e41, _e42, _e43, _e44;
	//	};
	//};
};

struct SCREEN_XY
{
	float x, y, z;
	unsigned int color;
};