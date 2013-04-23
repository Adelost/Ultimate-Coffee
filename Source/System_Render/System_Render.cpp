#include "System_Render.h"

#include "Core/EventManager.h"

//// include the basic windows header files and the Direct3D header files
//#include <windows.h>
//#include <windowsx.h>
//#include <d3d10.h>
//#include <d3dx10.h>
//
//// include the Direct3D Library file
//#pragma comment (lib, "d3d10.lib")
//#pragma comment (lib, "d3dx10.lib")
//
//// define the screen resolution
//#define SCREEN_WIDTH  800
//#define SCREEN_HEIGHT 600
//
//// global declarations
//ID3D10Device* device;    // the pointer to our Direct3D device interface
//ID3D10RenderTargetView* rtv;    // the pointer to the render target view
//IDXGISwapChain* swapchain;    // the pointer to the swap chain class
//ID3D10Buffer* pBuffer;    // the pointer to the vertex buffer
//ID3D10Effect* pEffect;    // the pointer to the effect file interface
//ID3D10EffectTechnique* pTechnique;    // the pointer to the technique interface
//ID3D10EffectPass* pPass;    // the pointer to the pass interface
//ID3D10InputLayout* pVertexLayout;    // the pointer to the input layout interface
//ID3D10EffectMatrixVariable* pTransform;    // the pointer to the effect variable interface
//ID3D10EffectScalarVariable* pTimeElapsed;    // the pointer to the effect variable interface
//D3D10_PASS_DESC PassDesc;    // the pass description struct
//
//struct VERTEX {D3DXVECTOR3 Position; D3DXCOLOR Color;};
//
//// function prototypes
//void render_frame(void);    // renders a single frame
//void cleanD3D(void);    // closes Direct3D and releases memory
//void init_geometry(void);    // creates geometry to render
//void init_pipeline(void);    // sets up the pipeline for rendering


System::Render::Render()
{
	//setupDirectX();
}

void System::Render::setupDirectX()
{
	HWND windowHandle;
	{
		Event_GetWindowHandle e;
		SEND_EVENT(&e);
		windowHandle = e.handle;
	}

	//// this function initializes and prepares Direct3D for use

	//DXGI_SWAP_CHAIN_DESC scd;    // create a struct to hold various swap chain information

	//ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));    // clear out the struct for use

	//scd.BufferCount = 1;    // c to be used by Direct3D
	//scd.SampleDesc.Count = 1;    // set the level of multi-sampling
	//scd.SampleDesc.Quality = 0;    // set the quality of multi-samplingreate two buffers, one for the front, one for the back
	//scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
	//scd.BufferDesc.Width = SCREEN_WIDTH;    // set the back buffer width
	//scd.BufferDesc.Height = SCREEN_HEIGHT;    // set the back buffer height
	//scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // tell how the chain is to be used
	//scd.OutputWindow = windowHandle;    // set the window
	//scd.Windowed = TRUE;    // set to windowed or full-screen mode at startup
	//scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

	//// create a device class and swap chain class using the information in the scd struct
	//D3D10CreateDeviceAndSwapChain(NULL,
	//	D3D10_DRIVER_TYPE_HARDWARE,
	//	NULL,
	//	0,
	//	D3D10_SDK_VERSION,
	//	&scd,
	//	&swapchain,
	//	&device);


	//// get the address of the back buffer and use it to create the render target
	//ID3D10Texture2D* pBackBuffer;
	//swapchain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	//device->CreateRenderTargetView(pBackBuffer, NULL, &rtv);
	//pBackBuffer->Release();

	//// set the back buffer as the render target
	//device->OMSetRenderTargets(1, &rtv, NULL);


	//D3D10_VIEWPORT viewport;    // create a struct to hold the viewport data

	//ZeroMemory(&viewport, sizeof(D3D10_VIEWPORT));    // clear out the struct for use

	//viewport.TopLeftX = 0;    // set the left to 0
	//viewport.TopLeftY = 0;    // set the top to 0
	//viewport.Width = SCREEN_WIDTH;    // set the width to the window's width
	//viewport.Height = SCREEN_HEIGHT;    // set the height to the window's height

	//device->RSSetViewports(1, &viewport);    // set the viewport

	//// create three vertices using the VERTEX struct built earlier
	//VERTEX OurVertices[] =
	//{
	//	{D3DXVECTOR3(0.0f, 0.5f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)},
	//	{D3DXVECTOR3(0.45f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)},
	//	{D3DXVECTOR3(-0.45f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)}
	//};

	//// create the vertex buffer and store the pointer into pBuffer, which is created globally
	//D3D10_BUFFER_DESC bd;
	//bd.Usage = D3D10_USAGE_DYNAMIC;
	//bd.ByteWidth = sizeof(VERTEX) * 3;
	//bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	//bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	//bd.MiscFlags = 0;

	//device->CreateBuffer(&bd, NULL, &pBuffer);

	//void* pVoid;    // the void pointer

	//pBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &pVoid);    // map the vertex buffer
	//memcpy(pVoid, OurVertices, sizeof(OurVertices));    // copy the vertices to the buffer
	//pBuffer->Unmap();

	//// load the effect file
	//D3DX10CreateEffectFromFile(L"effect.fx", 0, 0,
	//	"fx_4_0", 0, 0,
	//	device, 0, 0,
	//	&pEffect, 0, 0);

	//// get the technique and the pass
	//pTechnique = pEffect->GetTechniqueByIndex(0);
	//pPass = pTechnique->GetPassByIndex(0);
	//pPass->GetDesc(&PassDesc);

	//// get the TimeElapsed effect variable
	////pTimeElapsed = pEffect->GetVariableByName("TimeElapsed")->AsScalar();
	//pTransform = pEffect->GetVariableByName("Transform")->AsMatrix();

	//// create the input element descriptions
	//D3D10_INPUT_ELEMENT_DESC Layout[] =
	//{
	//	// first input element: position
	//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,
	//	D3D10_INPUT_PER_VERTEX_DATA, 0},

	//	// second input element: color
	//	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,
	//	D3D10_INPUT_PER_VERTEX_DATA, 0}
	//};

	//// use the input element descriptions to create the input layout
	//device->CreateInputLayout(Layout,
	//	2,
	//	PassDesc.pIAInputSignature,
	//	PassDesc.IAInputSignatureSize,
	//	&pVertexLayout);
	
}

void System::Render::process()
{

}

System::Render::~Render()
{
	//cleanD3D();
}

void System::Render::update()
{
	while(m_mapper_position.hasNext())
	{
		Data::Position* position = m_mapper_position.next();
	}

	//// clear the window to a deep blue
	//device->ClearRenderTargetView(rtv, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

	//// select which input layout we are using
	//device->IASetInputLayout(pVertexLayout);

	//// select which primtive type we are using
	//device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//// select which vertex buffer to display
	//UINT stride = sizeof(VERTEX);
	//UINT offset = 0;
	//device->IASetVertexBuffers(0, 1, &pBuffer, &stride, &offset);

	//// increase the time variable and send to the effect
	//static float Time = 0.0f; Time += 0.001f;

	//D3DXMATRIX matRotate, matView, matProjection, matFinal;

	//// create a rotation matrix
	//D3DXMatrixRotationY(&matRotate, Time);

	//// create a view matrix
	//D3DXMatrixLookAtLH(&matView,
	//	&D3DXVECTOR3(0.0f, 0.0f, 2.0f),    // the camera position
	//	&D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
	//	&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction

	//// create a projection matrix
	//D3DXMatrixPerspectiveFovLH(&matProjection,
	//	(float)D3DXToRadian(45),    // the horizontal field of view
	//	(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
	//	1.0f,    // the near view-plane
	//	100.0f);    // the far view-plane

	//// combine and set the final transform
	//matFinal = matRotate * matView * matProjection;
	//pTransform->SetMatrix(&matFinal._11); 

	//// apply the appropriate pass
	//pPass->Apply(0);

	//// draw the vertex buffer to the back buffer
	//device->Draw(3, 0);

	//// display the rendered frame
	//swapchain->Present(0, 0);

	////Sleep(10);
}

void cleanD3D( void )
{
	//swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode
	//pBuffer->Release();    // close and release the vertex buffer
	//pVertexLayout->Release();    // close and release the input layout object
	//swapchain->Release();    // close and release the swap chain
	//rtv->Release();    // close and release the render target view
	//device->Release();    // close and release the 3D device
}
