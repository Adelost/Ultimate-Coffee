#include "stdafx.h"
#include "DXRenderer.h"
#include "Box.h"
#include "Manager_3DTools.h"

#include <Core/EventManager.h>
#include <Core/Events.h>
#include <Core/World.h>
#include <Core/Settings.h>
#include <Core/Camera.h>

DXRenderer::DXRenderer()
{
	SUBSCRIBE_TO_EVENT(this, EVENT_SET_BACKBUFFER_COLOR);
	SUBSCRIBE_TO_EVENT(this, EVENT_WINDOW_RESIZE);

	m_dxDevice = nullptr;
	m_dxDeviceContext = nullptr;
	m_dxSwapChain = nullptr;

	m_view_renderTarget = nullptr;
	m_view_depthStencil = nullptr;
	m_tex_depthStencil = nullptr;
	m_viewport_screen = nullptr;

	m_CBuffer.WVP.Identity();
}

DXRenderer::~DXRenderer()
{
	ReleaseCOM(m_vertexBuffer);
	ReleaseCOM(m_indexBuffer);
	ReleaseCOM(m_WVPBuffer);
	ReleaseCOM(m_inputLayout);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_vertexShader);
	ReleaseCOM(m_view_renderTarget);
	ReleaseCOM(m_view_depthStencil);
	ReleaseCOM(m_dxSwapChain);
	ReleaseCOM(m_tex_depthStencil);
	if(m_dxDeviceContext)
		m_dxDeviceContext->ClearState();
	ReleaseCOM(m_dxDeviceContext);
	ReleaseCOM(m_dxDevice);
	delete m_viewport_screen;

	delete m_manager_tools;
}

bool DXRenderer::init( HWND p_windowHandle )
{
	bool result = false;

	m_windowHandle = p_windowHandle;

	m_clientWidth = 800;
	m_clientHeight = 600;

	m_viewport_screen = new D3D11_VIEWPORT();

	result = initDX();

	m_manager_tools = new Manager_3DTools(this->m_dxDevice, this->m_dxDeviceContext, this->m_view_depthStencil);

	return result;
}

void DXRenderer::onEvent(IEvent* p_event)
{
	EventType type = p_event->type();
	switch (type) 
	{
	case EVENT_SET_BACKBUFFER_COLOR:
		{
			Event_SetBackBufferColor* backbufferColorEvent = static_cast<Event_SetBackBufferColor*>(p_event);
			SETTINGS()->backBufferColor.x = backbufferColorEvent->x;
			SETTINGS()->backBufferColor.y = backbufferColorEvent->y;
			SETTINGS()->backBufferColor.z = backbufferColorEvent->z;
		}
		break;
	case EVENT_WINDOW_RESIZE:
		{
			Event_WindowResize* windowResizeEvent = static_cast<Event_WindowResize*>(p_event);
			m_clientWidth = windowResizeEvent->width;
			m_clientHeight = windowResizeEvent->height;
			resizeDX();
		}
	default:
		break;
	}
}

void DXRenderer::renderFrame()
{
	// Restore the back and depth buffer to the OM stage.
	ID3D11RenderTargetView* renderTargets[1] = {m_view_renderTarget};
	m_dxDeviceContext->OMSetRenderTargets(1, renderTargets, m_view_depthStencil);
	m_dxDeviceContext->RSSetViewports(1, m_viewport_screen);

	// Clear render target & depth/stencil
	m_dxDeviceContext->ClearRenderTargetView(m_view_renderTarget, static_cast<const float*>(SETTINGS()->backBufferColor));
	m_dxDeviceContext->ClearDepthStencilView(m_view_depthStencil, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

						m_dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

						m_dxDeviceContext->PSSetShader(m_pixelShader, 0, 0);
						m_dxDeviceContext->VSSetShader(m_vertexShader, 0, 0);

						UINT stride = sizeof(VertexPosCol);
						UINT offset = 0;
						m_dxDeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
						m_dxDeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
						m_dxDeviceContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);

						static float delta = 0.0f;

						Camera theCamera = *SETTINGS()->camera;
						theCamera.UpdateViewMatrix();

						XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -15.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
						XMMATRIX proj = XMMatrixPerspectiveFovLH(0.25f * Math::Pi, 755.0f / 426.0f, 1.0f, 3000.0f);
						XMMATRIX viewProj = view * proj;
						XMMATRIX WVP = m_CBuffer.WVP.CreateRotationX(delta) * XMMatrixTranslation(0.0f, 0.0f, 0.0f) * viewProj;

						delta += 0.0001f;
						m_CBuffer.WVP = WVP; //m_CBuffer.WVP.CreateRotationX(delta) * theCamera.View() * theCamera.Proj();

						m_dxDeviceContext->UpdateSubresource(m_WVPBuffer, 0, NULL, &WVP, 0, 0);

						m_dxDeviceContext->DrawIndexed(36, 0, 0);
						//m_dxDeviceContext->Draw(8, 0);

						m_dxDeviceContext->RSSetState(0);

	m_manager_tools->update();
	m_manager_tools->draw(m_view_depthStencil);

	// Show the finished frame
	HR(m_dxSwapChain->Present(0, 0));
}

bool DXRenderer::initDX()
{
	// Create Device
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,							// use primary Display adapter
		D3D_DRIVER_TYPE_HARDWARE,	// use hardware acceleration
		0,							// no software device
		createDeviceFlags,			// set debug flag
		0,							// feature level array
		0,							// feature level array size     
		D3D11_SDK_VERSION,			// SDK version
		&m_dxDevice,				// returns d3dDevice
		&featureLevel,				// returns feature level
		&m_dxDeviceContext);		// returns device context
	if(FAILED(hr))
	{
		MESSAGEBOX("Error: D3D11CreateDevice Failed.");
		return false;
	}
	if(featureLevel != D3D_FEATURE_LEVEL_11_0 && featureLevel != D3D_FEATURE_LEVEL_10_1 && featureLevel != D3D_FEATURE_LEVEL_10_0)
	{
		MESSAGEBOX("Direct3D Feature Level 11 unsupported.");
		return false;
	}

	// Describe swap chain.
	DXGI_SWAP_CHAIN_DESC desc_sc;
	// buffer description = back buffer properties
	desc_sc.BufferDesc.Width = m_clientWidth;					// width
	desc_sc.BufferDesc.Height = m_clientHeight;					// height
	desc_sc.BufferDesc.RefreshRate.Numerator = 60;
	desc_sc.BufferDesc.RefreshRate.Denominator = 1;
	desc_sc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// set pixel format
	desc_sc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc_sc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// sample description, used to set MSAA
	HR(m_dxDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_msaa_quality));
	assert(m_msaa_quality > 0);
	//if(msaa_enable)
	//{
	//	desc_sc.SampleDesc.Count   = 4;
	//	desc_sc.SampleDesc.Quality = msaa_quality-1;
	//}
	//else
	{
		desc_sc.SampleDesc.Count   = 1;
		desc_sc.SampleDesc.Quality = 0;
	}
	desc_sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// set backbuffer as render target 
	desc_sc.BufferCount = 1;								// nr of backbuffers
	desc_sc.OutputWindow = m_windowHandle;					// set output window
	desc_sc.Windowed = true;								// set window mode/fullscreen
	desc_sc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;			// let display driver select most efficient presentation method
	desc_sc.Flags = 0;										// optional flags, such as DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH

	// Use IDXGIFactory to create swap chain
	IDXGIDevice* dxgiDevice = 0;
	HR(m_dxDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));
	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));
	// finally the IDXGIFactory interface
	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));
	// create swap chain
	HR(dxgiFactory->CreateSwapChain(m_dxDevice, &desc_sc, &m_dxSwapChain));
	// release acquired COM interfaces since we're done with them
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	////////////////////////////////////////////////////////////////////////////////////////////

	// Compile and set shaders

	ID3DBlob *PS_Buffer, *VS_Buffer;

	//hr = D3DReadFileToBlob(L"PixelShader.cso", &PS_Buffer);
	HR(D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "pixelMain", "ps_5_0", NULL, NULL, &PS_Buffer, NULL));
	HR(m_dxDevice->CreatePixelShader( PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &m_pixelShader));

	HR(D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "vertexMain", "vs_5_0", NULL, NULL, &VS_Buffer, NULL));
	HR(m_dxDevice->CreateVertexShader( VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &m_vertexShader));

	m_dxDeviceContext->PSSetShader(m_pixelShader, 0, 0);
	m_dxDeviceContext->VSSetShader(m_vertexShader, 0, 0);

	// Create input layout

	D3D11_INPUT_ELEMENT_DESC inputElementDesc [] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},	
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HR(m_dxDevice->CreateInputLayout(inputElementDesc, 2, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &m_inputLayout));

	ReleaseCOM(VS_Buffer);
	ReleaseCOM(PS_Buffer);

	m_dxDeviceContext->IASetInputLayout(m_inputLayout);

	// Create vertex buffer

	D3D11_BUFFER_DESC vertexBufferDesc;
	memset(&vertexBufferDesc, 0, sizeof(vertexBufferDesc));
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(VertexPosCol) * 8;
	vertexBufferDesc.StructureByteStride = sizeof(VertexPosCol);
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA vertexData;
	memset(&vertexData, 0, sizeof(vertexData));
	vertexData.pSysMem = Shape::BoxVertices;

	HR(m_dxDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer));

	UINT stride = sizeof(VertexPosCol);
	UINT offset = 0;
	m_dxDeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Create index buffer

	D3D11_BUFFER_DESC indexBufferDesc;
	memset(&indexBufferDesc, 0, sizeof(indexBufferDesc));
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * 36;
	indexBufferDesc.StructureByteStride = sizeof(unsigned int);
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA indexData;
	memset(&indexData, 0, sizeof(indexData));
	indexData.pSysMem = Shape::BoxIndex;

	HR(m_dxDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer));

	m_dxDeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Create constant buffer

	D3D11_BUFFER_DESC WVP_Desc;
	memset(&WVP_Desc, 0, sizeof(WVP_Desc));
	WVP_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	WVP_Desc.ByteWidth = 16 * 4;//sizeof(float) * 4;
	WVP_Desc.StructureByteStride = 0;//sizeof(float);
	WVP_Desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA WVP_Data;
	memset(&WVP_Data, 0, sizeof(WVP_Data));
	WVP_Data.pSysMem = &m_CBuffer;

	HR(m_dxDevice->CreateBuffer(&WVP_Desc, &WVP_Data, &m_WVPBuffer));

	m_dxDeviceContext->VSSetConstantBuffers(0, 1, &m_WVPBuffer);

	////////////////////////////////////////////////////////////////////////////////////////////

	// Resize
	resizeDX();

	return true;
}

void DXRenderer::resizeDX()
{
	// Release old views, they hold references to buffers we will be destroying; release the old depth/stencil buffer
	ReleaseCOM(m_view_renderTarget);
	ReleaseCOM(m_view_depthStencil);
	ReleaseCOM(m_tex_depthStencil);
	assert(m_dxDeviceContext);
	assert(m_dxDevice);
	assert(m_dxSwapChain);

	// Resize the swap chain
	HR(m_dxSwapChain->ResizeBuffers(1, m_clientWidth, m_clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	// Create render target view
	ID3D11Texture2D* backBuffer;
	// get pointer to backbuffer
	HR(m_dxSwapChain->GetBuffer(
		0,											// backbuffer index (in case there is more than one)
		__uuidof(ID3D11Texture2D),					// interface type of the buffer
		reinterpret_cast<void**>(&backBuffer)));	// returns backbuffer pointer
	// create render target view
	HR(m_dxDevice->CreateRenderTargetView(
		backBuffer,				// resource to be used as render target
		0,						// pointer to D3D11_RENDER_TARGET_VIEW_DESC (only needed when using typless format)
		&m_view_renderTarget));	// returns pointer to render target view
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC desc_depthStencil; 
	desc_depthStencil.Width     = m_clientWidth;					// width
	desc_depthStencil.Height    = m_clientHeight;					// height
	desc_depthStencil.MipLevels = 1;								// nr of mipmap levels
	desc_depthStencil.ArraySize = 1;								// nr of textures in a texture array
	desc_depthStencil.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;	// format
	// set MSSA, settings must match those in swap chain
	//if(msaa_enable)
	//{
	//	desc_depthStencil.SampleDesc.Count   = 4;
	//	desc_depthStencil.SampleDesc.Quality = msaa_quality-1;
	//}
	//else
	{
		desc_depthStencil.SampleDesc.Count   = 1;
		desc_depthStencil.SampleDesc.Quality = 0;
	}
	desc_depthStencil.Usage          = D3D11_USAGE_DEFAULT;			// specify how the depth buffer will be used
	desc_depthStencil.BindFlags      = D3D11_BIND_DEPTH_STENCIL;	// specify where resource will be bound to pipeline
	desc_depthStencil.CPUAccessFlags = 0;							// specify if cpu will read resource
	desc_depthStencil.MiscFlags      = 0;							// flags; not appliable to depth buffer
	HR(m_dxDevice->CreateTexture2D(&desc_depthStencil, 0, &m_tex_depthStencil));
	HR(m_dxDevice->CreateDepthStencilView(m_tex_depthStencil, 0, &m_view_depthStencil));

	// Bind render target view and depth/stencil view to pipeline
	m_dxDeviceContext->OMSetRenderTargets(
		1,							// nr of render targets
		&m_view_renderTarget,		// first element of array of rendertargets
		m_view_depthStencil);		// pointer to depth/stencil view

	// Set viewport transform.
	m_viewport_screen->TopLeftX = 0;
	m_viewport_screen->TopLeftY = 0;
	m_viewport_screen->Width    = static_cast<float>(m_clientWidth);
	m_viewport_screen->Height   = static_cast<float>(m_clientHeight);
	m_viewport_screen->MinDepth = 0.0f;
	m_viewport_screen->MaxDepth = 1.0f;
	m_dxDeviceContext->RSSetViewports(
		1,								// nr of viewports
		m_viewport_screen);			// viewport array
}

ID3D11Device* DXRenderer::getDevice()
{
	return this->m_dxDevice;
}

ID3D11DeviceContext* DXRenderer::getDeviceContext()
{
	return this->m_dxDeviceContext;
}

ID3D11DepthStencilView* DXRenderer::getDepthStencilView()
{
	return this->m_view_depthStencil;
}