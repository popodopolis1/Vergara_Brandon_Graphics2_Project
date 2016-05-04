//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <iostream>
#include <ctime>
#include "XTime.h"
#include <string.h>
#include "defines.h"
#include "DDSTextureLoader.h"

using namespace std;

// BEGIN PART 1
// TODO: PART 1 STEP 1a
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
// TODO: PART 1 STEP 1b
#include "math.h"
// TODO: PART 2 STEP 6
#include "Trivial_PS.csh"
#include "Trivial_VS.csh"

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class DEMO_APP
{	
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2
	IDXGISwapChain *swapChain;
	ID3D11Device *device;
	ID3D11DeviceContext *context;
	ID3D11RenderTargetView *pView;
	D3D11_VIEWPORT viewport;
	ID3D11DepthStencilView *pDSV;
	ID3D11Texture2D* pDepthStencil = NULL;
	// TODO: PART 2 STEP 2
	ID3D11Buffer *buff;
	ID3D11Buffer *vertBuff;
	ID3D11Buffer *ibuff;
	UINT numVerts = 0;
	ID3D11InputLayout *layout;
	// BEGIN PART 5
	// TODO: PART 5 STEP 1
	
	// TODO: PART 2 STEP 4
	ID3D11VertexShader *vShade;
	ID3D11PixelShader *pShade;
	// BEGIN PART 3
	// TODO: PART 3 STEP 1
	ID3D11Buffer *ncBuff;
	XTime timer;

public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1
	struct SIMPLE_VERTEX
	{
		//float x, y, z, w;
		XMFLOAT4 pos;
		XMFLOAT4 col;
		//float r, g, b, a;
	};

	struct OBJECT
	{
		XMMATRIX worldMatrix;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
	};

	OBJECT toObject;

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst; 
	appWndProc = proc; 

	WNDCLASSEX  wndClass;
    ZeroMemory( &wndClass, sizeof( wndClass ) );
    wndClass.cbSize         = sizeof( WNDCLASSEX );             
    wndClass.lpfnWndProc    = appWndProc;						
    wndClass.lpszClassName  = L"DirectXApplication";            
	wndClass.hInstance      = application;		               
    wndClass.hCursor        = LoadCursor( NULL, IDC_ARROW );    
    wndClass.hbrBackground  = ( HBRUSH )( COLOR_WINDOWFRAME ); 
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
    RegisterClassEx( &wndClass );

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(	L"DirectXApplication", L"Lab 1a Line Land",	WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME|WS_MAXIMIZEBOX), 
							CW_USEDEFAULT, CW_USEDEFAULT, window_size.right-window_size.left, window_size.bottom-window_size.top,					
							NULL, NULL,	application, this );												

    ShowWindow( window, SW_SHOW );
	//********************* END WARNING ************************//

	// TODO: PART 1 STEP 3a
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = window;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// TODO: PART 1 STEP 3b
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, 0, D3D11_SDK_VERSION, &scd, &swapChain, &device, NULL, &context);
	// TODO: PART 1 STEP 4
	ID3D11Texture2D *backBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	device->CreateRenderTargetView(backBuffer, NULL, &pView);
	backBuffer->Release();
	// TODO: PART 1 STEP 5
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.Height = BACKBUFFER_HEIGHT;
	viewport.Width = BACKBUFFER_WIDTH;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	SIMPLE_VERTEX star[12];
#pragma region Star verts

	for (int i = 0; i < 12; ++i)
	{
		if (i == 0 || i == 11)
		{
			star[i].pos.x = 0.0f;
			star[i].pos.y = 0.0f;

			if (i == 0)
				star[i].pos.z = -0.2f;
			else if (i == 11)
				star[i].pos.z = 0.2f;
			star[i].pos.w = 1.0f;
			star[i].col.x = 1.0f;
			star[i].col.y = 1.0f;
			star[i].col.z = 0.0f;
			star[i].col.w = 1.0f;
		}
		else
		{
			star[i].pos.x = cos(XMConvertToRadians(i) * 36);
			star[i].pos.y = sin(XMConvertToRadians(i) * 36);
			star[i].pos.z = 0.0f;
			star[i].pos.w = 1.0f;
			star[i].col.x = 1.0f;
			star[i].col.y = 0.0f;
			star[i].col.z = 0.0f;
			star[i].col.w = 1.0f;

			if (i%2 == 0)
			{
				star[i].pos.x = cos(XMConvertToRadians(i) * 36)/2;
				star[i].pos.y = sin(XMConvertToRadians(i) * 36)/2;
			}
			
		}
	}

#pragma endregion

	uint indices[60] = { 0, 2, 1, 0, 3, 2, 0, 4, 3, 0, 5, 4, 0, 6, 5, 0, 7, 6, 0, 8, 7, 0, 9, 8, 0, 10, 9, 0, 1, 10, 11, 1, 2, 11, 2, 3, 11, 3, 4, 11, 4, 5, 11, 5, 6, 11, 6, 7, 11, 7, 8, 11, 8, 9, 11, 9, 10, 11, 10, 1 };

	// BEGIN PART 4
	// TODO: PART 4 STEP 1
	D3D11_BUFFER_DESC vb;
	ZeroMemory(&vb, sizeof(vb));

	vb.Usage = D3D11_USAGE_IMMUTABLE;
	vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb.CPUAccessFlags = NULL;
	vb.ByteWidth = sizeof(SIMPLE_VERTEX) * 12;
	vb.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA inDat;
	inDat.pSysMem = star;
	inDat.SysMemPitch = 0;
	inDat.SysMemSlicePitch = 0;

	device->CreateBuffer(&vb, &inDat, &vertBuff);

	D3D11_BUFFER_DESC iB;
	ZeroMemory(&iB, sizeof(iB));

	iB.Usage = D3D11_USAGE_IMMUTABLE;
	iB.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iB.ByteWidth = sizeof(unsigned int) * 60;
	iB.MiscFlags = 0;
	//iB.StructureByteStride = sizeof(float);

	D3D11_SUBRESOURCE_DATA intDat;
	intDat.pSysMem = indices;
	intDat.SysMemPitch = 0;
	intDat.SysMemSlicePitch = 0;

	device->CreateBuffer(&iB, &intDat, &ibuff);

	// TODO: PART 2 STEP 3b
	//D3D11_BUFFER_DESC bd;
	//ZeroMemory(&bd, sizeof(bd));
	//
	//bd.Usage = D3D11_USAGE_IMMUTABLE;
	//bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.CPUAccessFlags = NULL;
	//bd.ByteWidth = sizeof(SIMPLE_VERTEX) * 361;
	//bd.MiscFlags = 0;
    //// TODO: PART 2 STEP 3c
	//D3D11_SUBRESOURCE_DATA initData;
	//initData.pSysMem = circle;
	//initData.SysMemPitch = 0;
	//initData.SysMemSlicePitch = 0;
	//// TODO: PART 2 STEP 3d
	//device->CreateBuffer(&bd, &initData, &buff);
	// TODO: PART 5 STEP 2a
	
	// TODO: PART 5 STEP 2b
	
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = BACKBUFFER_WIDTH;
	descDepth.Height = BACKBUFFER_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	device->CreateTexture2D(&descDepth, NULL, &pDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	memset(&descDSV, 0, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	device->CreateDepthStencilView(pDepthStencil, &descDSV, &pDSV);

	// TODO: PART 5 STEP 3
		
	// TODO: PART 2 STEP 5
	// ADD SHADERS TO PROJECT, SET BUILD OPTIONS & COMPILE

	// TODO: PART 2 STEP 7
	device->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &vShade);
	device->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &pShade);
	// TODO: PART 2 STEP 8a
	D3D11_INPUT_ELEMENT_DESC vLayout[2] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	// TODO: PART 2 STEP 8b
	device->CreateInputLayout(vLayout,ARRAYSIZE(vLayout),Trivial_VS,sizeof(Trivial_VS), &layout);
	// TODO: PART 3 STEP 3

	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(cb));

	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.ByteWidth = sizeof(OBJECT);
	cb.MiscFlags = 0;
	cb.StructureByteStride = sizeof(float);

	device->CreateBuffer(&cb, NULL, &ncBuff);

	// TODO: PART 3 STEP 4b

	toObject.worldMatrix = XMMatrixMultiply(XMMatrixTranslation(0, 0, 4), XMMatrixIdentity());
	toObject.viewMatrix = XMMatrixIdentity();
	toObject.projectionMatrix = XMMatrixPerspectiveFovLH(65.0f, BACKBUFFER_WIDTH / BACKBUFFER_HEIGHT, 0.1f, 20.0f);
	timer.Signal();
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
	// TODO: PART 4 STEP 2

	XMMATRIX rotation = XMMatrixRotationY((float)timer.Delta() * 0.01f);
	toObject.worldMatrix = XMMatrixMultiply(rotation, toObject.worldMatrix);
	// TODO: PART 4 STEP 3
	
	// TODO: PART 4 STEP 5
	
	// END PART 4

	// TODO: PART 1 STEP 7a
	context->OMSetRenderTargets(1, &pView, pDSV);
	// TODO: PART 1 STEP 7b
	context->RSSetViewports(1, &viewport);
	// TODO: PART 1 STEP 7c
	float colors[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	context->ClearRenderTargetView(pView, colors);
	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1, 0);
	// TODO: PART 5 STEP 4

	// TODO: PART 5 STEP 5
	
	// TODO: PART 5 STEP 6
	
	// TODO: PART 5 STEP 7
	
	// END PART 5
	
	// TODO: PART 3 STEP 5
	D3D11_MAPPED_SUBRESOURCE sub;
	context->Map(ncBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &sub);
	memcpy(sub.pData, &toObject, sizeof(toObject));
	context->Unmap(ncBuff, 0);

	// TODO: PART 3 STEP 6
	context->VSSetConstantBuffers(0, 1, &ncBuff);

	// TODO: PART 2 STEP 9a
	UINT offset = 0;
	UINT stride = sizeof(SIMPLE_VERTEX);

	context->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);
	context->IASetIndexBuffer(ibuff, DXGI_FORMAT_R32_UINT, offset);

	// TODO: PART 2 STEP 9b
	context->VSSetShader(vShade, NULL, 0);
	context->PSSetShader(pShade, NULL, 0);
	// TODO: PART 2 STEP 9c
	context->IASetInputLayout(layout);
	// TODO: PART 2 STEP 9d
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TODO: PART 2 STEP 10
	context->DrawIndexed(60, 0, 0);
	//context->Draw(12, 0);
	// END PART 2

	// TODO: PART 1 STEP 8
	swapChain->Present(0, 0);
	// END OF PART 1
	return true; 
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6
	swapChain->Release();
	pView->Release();
	device->Release();
	context->Release();
	//buff->Release();
	vertBuff->Release();
	ibuff->Release();
	layout->Release();
	vShade->Release();
	pShade->Release();
	ncBuff->Release();
	pDSV->Release();
	pDepthStencil->Release();
	
	UnregisterClass( L"DirectXApplication", application ); 
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!
	
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,	int nCmdShow );						   
LRESULT CALLBACK WndProc(HWND hWnd,	UINT message, WPARAM wparam, LPARAM lparam );		
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance,(WNDPROC)WndProc);	
    MSG msg; ZeroMemory( &msg, sizeof( msg ) );
    while ( msg.message != WM_QUIT && myApp.Run() )
    {	
	    if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        { 
            TranslateMessage( &msg );
            DispatchMessage( &msg ); 
        }
    }
	myApp.ShutDown(); 
	return 0; 
}
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if(GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
    switch ( message )
    {
        case ( WM_DESTROY ): { PostQuitMessage( 0 ); }
        break;
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}
//********************* END WARNING ************************//