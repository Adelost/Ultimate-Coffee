#include "DXRenderer.h"

DXRenderer::DXRenderer()
{
	dxDevice_ = nullptr;
	dxDeviceContext_ = nullptr;
	dxSwapChain_ = nullptr;

	view_renderTarget = nullptr;
	view_depthStencil = nullptr;
	tex_depthStencil = nullptr;
}

DXRenderer::~DXRenderer()
{
	ReleaseCOM(pixelShader_);
	ReleaseCOM(vertexShader_);
	ReleaseCOM(view_renderTarget);
	ReleaseCOM(view_depthStencil);
	ReleaseCOM(dxSwapChain_);
	ReleaseCOM(tex_depthStencil);
	if(dxDeviceContext_)
		dxDeviceContext_->ClearState();
	ReleaseCOM(dxDeviceContext_);
	ReleaseCOM(dxDevice_);
}

bool DXRenderer::init(HWND windowHandle)
{
	windowHandle_ = windowHandle;

	clientWidth_ = 800;
	clientHeight_ = 600;

	return initDX();
}

void DXRenderer::renderFrame()
{
	//mSmap->BindDsvAndSetNullRenderTarget(dxDeviceContext_);
	//DrawSceneToShadowMap();
	//dxDeviceContext_->RSSetState(0);

	// Restore the back and depth buffer to the OM stage.
	ID3D11RenderTargetView* renderTargets[1] = {view_renderTarget};
	dxDeviceContext_->OMSetRenderTargets(1, renderTargets, view_depthStencil);
	dxDeviceContext_->RSSetViewports(1, &viewport_screen);

	// Clear render target & depth/stencil
	float color[] = {0.14f, 0.42f, 0.56f, 1.0f};
	dxDeviceContext_->ClearRenderTargetView(view_renderTarget, reinterpret_cast<const float*>(color));
	dxDeviceContext_->ClearDepthStencilView(view_depthStencil, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set per frame constants.
	//FXStandard* fx = shaderManager->effects.fx_standard;
	//fx->SetEyePosW(mCam.GetPosition());
	//fx->SetCubeMap(mSky->CubeMapSRV());
	//fx->SetShadowMap(mSmap->DepthMapSRV());
	//
	// Tessellation settings
	//fx->SetHeightScale(tess_heightScale);
	//fx->SetMaxTessDistance(tess_maxTessDistance);
	//fx->SetMinTessDistance(tess_minTessDistance);
	//fx->SetMinTessFactor(tess_minTessFactor);
	//fx->SetMaxTessFactor(tess_maxTessFactor);
	//
	//dxDeviceContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	//dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	//drawManager->prepareFrame();
	//
	//if(wireframe_enable)
	//	dxDeviceContext->RSSetState(shaderManager->states.WireframeRS);
	//
	// Draw
	//
	//drawGame();
	//
	//
	//if(drawTerrain)
	//	mTerrain.draw(dxDeviceContext, &mCam);
	//
	//dxDeviceContext_->RSSetState(0);

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	dxDeviceContext_->HSSetShader(0, 0, 0);
	dxDeviceContext_->DSSetShader(0, 0, 0);
	dxDeviceContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Debug view depth buffer.
	//if(GetAsyncKeyState('Z') & 0x8000)
	//{
	//	DrawScreenQuad(mSmap->DepthMapSRV());
	//}
	//
	//if(drawSky)
	//	mSky->Draw(dxDeviceContext_, &mCam);

	// restore default states, as the SkyFX changes them in the effect file.
	dxDeviceContext_->RSSetState(0);
	dxDeviceContext_->OMSetDepthStencilState(0, 0);

	// Unbind shadow map as a shader input because we are going to render to it next frame.
	// The shadow might be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	dxDeviceContext_->PSSetShaderResources(0, 16, nullSRV);

	// Show the finished frame
	HR(dxSwapChain_->Present(0, 0));
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
		0,							// use primary Dispslay adapter
		D3D_DRIVER_TYPE_HARDWARE,	// use hardware acceleration
		0,							// no software device
		createDeviceFlags,			// set debug flag
		0,							// feature level array
		0,							// feature level array size     
		D3D11_SDK_VERSION,			// SDK version
		&dxDevice_,					// returns d3dDevice
		&featureLevel,				// returns feature level
		&dxDeviceContext_);			// returns device context
	if(FAILED(hr))
	{
		//QMessageBox::information(0, "Error", "D3D11CreateDevice Failed.");
		return false;
	}
	if(featureLevel != D3D_FEATURE_LEVEL_11_0 )
	{
		//QMessageBox::information(0, "Error", "Direct3D Feature Level 11 unsupported.");
		return false;
	}

	// Describe swap chain.
	DXGI_SWAP_CHAIN_DESC desc_sc;
	// buffer description = back buffer properties
	desc_sc.BufferDesc.Width = clientWidth_;					// width
	desc_sc.BufferDesc.Height = clientHeight_;					// height
	desc_sc.BufferDesc.RefreshRate.Numerator = 60;
	desc_sc.BufferDesc.RefreshRate.Denominator = 1;
	desc_sc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// set pixelformat
	desc_sc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc_sc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// sample description, used to set MSAA
	HR(dxDevice_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaa_quality));
	assert(msaa_quality > 0);
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
	desc_sc.OutputWindow = windowHandle_;					// set output window
	desc_sc.Windowed = true;								// set window mode/fullscreen
	desc_sc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;			// let display driver select most efficient presentation method
	desc_sc.Flags = 0;										// optional flags, such as DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH

	// Use IDXGIFactory to create swap chain
	IDXGIDevice* dxgiDevice = 0;
	HR(dxDevice_->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));
	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));
	// finally the IDXGIFactory interface
	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));
	// create swap chain
	HR(dxgiFactory->CreateSwapChain(dxDevice_, &desc_sc, &dxSwapChain_));
	// release acquired COM interfaces since we're done with them
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	ID3DBlob *PS_Buffer, *VS_Buffer;

	//hr = D3DReadFileToBlob(L"PixelShader.cso", &PS_Buffer);
	HR(D3DCompileFromFile(L"PixelShader.hlsl", NULL, NULL, "main", "ps_5_0", NULL, NULL, &PS_Buffer, NULL));
	HR(dxDevice_->CreatePixelShader( PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &pixelShader_));

	HR(D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "main", "vs_5_0", NULL, NULL, &VS_Buffer, NULL));
	HR(dxDevice_->CreateVertexShader( VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &vertexShader_));

	dxDeviceContext_->PSSetShader(pixelShader_, 0, 0);
	dxDeviceContext_->VSSetShader(vertexShader_, 0, 0);

	// Resize
	resizeDX();

	return true;
}

void DXRenderer::resizeDX()
{
	// Release old views, they hold references to buffers we will be destroying; release the old depth/stencil buffer
	ReleaseCOM(view_renderTarget);
	ReleaseCOM(view_depthStencil);
	ReleaseCOM(tex_depthStencil);
	assert(dxDeviceContext_);
	assert(dxDevice_);
	assert(dxSwapChain_);

	// Resize the swap chain
	HR(dxSwapChain_->ResizeBuffers(1, clientWidth_, clientHeight_, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	// Create render target view
	ID3D11Texture2D* backBuffer;
	// get pointer to backbuffer
	HR(dxSwapChain_->GetBuffer(
		0,											// backbuffer index (in case there is more than one)
		__uuidof(ID3D11Texture2D),					// interface type of the buffer
		reinterpret_cast<void**>(&backBuffer)));	// returns backbuffer pointer
	// create render target view
	HR(dxDevice_->CreateRenderTargetView(
		backBuffer,				// resource to be used as render target
		0,						// pointer to D3D11_RENDER_TARGET_VIEW_DESC (only needed when using typless format)
		&view_renderTarget));	// returns pointer to render target view
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC desc_depthStencil; 
	desc_depthStencil.Width     = clientWidth_;						// width
	desc_depthStencil.Height    = clientHeight_;					// height
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
	desc_depthStencil.BindFlags      = D3D11_BIND_DEPTH_STENCIL;	// specify where resource will be bound to pipline
	desc_depthStencil.CPUAccessFlags = 0;							// specify if cpu will read resource
	desc_depthStencil.MiscFlags      = 0;							// flags; not appliable to depth buffer
	HR(dxDevice_->CreateTexture2D(&desc_depthStencil, 0, &tex_depthStencil));
	HR(dxDevice_->CreateDepthStencilView(tex_depthStencil, 0, &view_depthStencil));

	// Bind render target view and depth/stencil view to pipeline
	dxDeviceContext_->OMSetRenderTargets(
		1,						// nr of render targets
		&view_renderTarget,		// first element of array of rendertargets
		view_depthStencil);		// pointer to depth/stencil view

	// Set viewport transform.
	viewport_screen.TopLeftX = 0;
	viewport_screen.TopLeftY = 0;
	viewport_screen.Width    = static_cast<float>(clientWidth_);
	viewport_screen.Height   = static_cast<float>(clientHeight_);
	viewport_screen.MinDepth = 0.0f;
	viewport_screen.MaxDepth = 1.0f;
	dxDeviceContext_->RSSetViewports(
		1,								// nr of viewports
		&viewport_screen);				// viewport array
}