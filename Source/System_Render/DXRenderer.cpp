#include "stdafx.h"
#include "DXRenderer.h"
#include "Box.h"
#include "Manager_3DTools.h"
#include "Buffer.h"
#include "Sky.h"
#include "RenderStates.h"

#include "MeshLoader.h"

DXRenderer::DXRenderer()
{
	SUBSCRIBE_TO_EVENT(this, EVENT_SET_BACKBUFFER_COLOR);
	SUBSCRIBE_TO_EVENT(this, EVENT_WINDOW_RESIZE);
	SUBSCRIBE_TO_EVENT(this, EVENT_COFFEE);


	m_dxDevice = nullptr;
	m_dxDeviceContext = nullptr;
	m_dxSwapChain = nullptr;

	m_view_renderTarget = nullptr;
	m_view_depthStencil = nullptr;
	m_tex_depthStencil = nullptr;
	m_viewport_screen = nullptr;
	m_sky = nullptr;
	m_sky2 = nullptr;
	m_msaa_enable = true;

	m_CBPerObject.world.Identity();
	m_CBPerObject.WVP.Identity();
	m_CBPerObject.WVP.CreateTranslation(0.0f, 0.0f, 1.0f);

	for(unsigned int i = 0; i < MAX_POINTLIGHTS; i++)
	{
		float range = 0.0f;
		m_CBPerFrame.plPosition[i] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
		m_CBPerFrame.plColorAndRange[i] = Vector4(0.0f, 0.0f, 0.0f, range);
	}
}

DXRenderer::~DXRenderer()
{
	// Delete buffers
	std::vector<Data::Render::BufferStore>* b = &Data::Render::manager.buffer_list;
	for(int i=0; i<(int)b->size(); i++)
	{
		delete b->at(i).vertex;
		delete b->at(i).index;
	}

	SafeDelete(m_objectConstantBuffer);
	SafeDelete(m_frameConstantBuffer);
	ReleaseCOM(m_inputLayout);
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_vertexShader);
	ReleaseCOM(m_view_renderTarget);
	ReleaseCOM(m_view_depthStencil);
	ReleaseCOM(m_dxSwapChain);
	RenderStates::DestroyAll();
	ReleaseCOM(m_tex_depthStencil);
	if(m_dxDeviceContext)
		m_dxDeviceContext->ClearState();
	ReleaseCOM(m_dxDeviceContext);
	ReleaseCOM(m_dxDevice);
	delete m_viewport_screen;
	delete m_sky;
	delete m_sky2;
	delete m_manager_tools;
}

bool DXRenderer::init( HWND p_windowHandle )
{
	bool result = false;

	m_windowHandle = p_windowHandle;

	m_clientWidth = 800;
	m_clientHeight = 600;

	m_viewport_screen = new D3D11_VIEWPORT();
	SETTINGS()->DXViewPortTopLeftX = (int)m_viewport_screen->TopLeftX;
	SETTINGS()->DXViewPortTopLeftY = (int)m_viewport_screen->TopLeftY;
	SETTINGS()->DXViewPortMinDepth = (int)m_viewport_screen->MinDepth;
	SETTINGS()->DXViewPortMaxDepth = (int)m_viewport_screen->MaxDepth;

	result = initDX();
	// Init RenderStates
	RenderStates::InitAll(m_dxDevice);

	m_manager_tools = new Manager_3DTools(this->m_dxDevice, this->m_dxDeviceContext, this->m_view_depthStencil, this->m_viewport_screen);

	m_sky = new Sky(m_dxDevice, m_dxDeviceContext, "skybox_clearsky.dds", 5000.0f);
	m_sky2 = new Sky(m_dxDevice, m_dxDeviceContext, "skybox_redclouds.dds", 5000.0f);

	return result;
}

void DXRenderer::onEvent(Event* p_event)
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
		break;
	case EVENT_COFFEE:
		{
		}
		break;
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

	m_dxDeviceContext->IASetInputLayout(m_inputLayout);
	m_dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_dxDeviceContext->PSSetShader(m_pixelShader, 0, 0);
	m_dxDeviceContext->VSSetShader(m_vertexShader, 0, 0);

	m_objectConstantBuffer->setDeviceContextBuffer(m_dxDeviceContext);
	m_frameConstantBuffer->setDeviceContextBuffer(m_dxDeviceContext);

	// Start rendering
	updatePointLights();

	// Update direction light
	DataMapper<Data::DirLight> map_dirLight;
	if(map_dirLight.hasNext())
	{
		Entity* e = map_dirLight.nextEntity();
		Data::Transform* d_transform = e->fetchData<Data::Transform>();
		Data::DirLight* d_dirLight = e->fetchData<Data::DirLight>();
		Data::Render* d_render = e->fetchData<Data::Render>();
		
		m_CBPerFrame.dlColor = Vector4(d_render->mesh.color.x, d_render->mesh.color.y, d_render->mesh.color.z, d_dirLight->ambient);
		Vector3 dir = Math::directionFromQuaterion(d_transform->rotation);
		Vector4 dirAmb = Vector4(dir);
		dirAmb.w = 0.0f;
		m_CBPerFrame.dlDirectionAndAmbient = dirAmb;
	}
	
	m_CBPerFrame.drawDebug = 0;
	m_dxDeviceContext->UpdateSubresource(m_frameConstantBuffer->getBuffer(), 0, nullptr, &m_CBPerFrame, 0, 0);

	Matrix viewProjection;
	// HACK: Adding camera to renderer
	// don't know where else to put it.
	// Please move to better location.
	{
		Data::Camera* d_camera = CAMERA_ENTITY()->fetchData<Data::Camera>();

		Matrix projection = d_camera->projection();
		Matrix view = d_camera->view();
		viewProjection = view * projection;
	}
	
	static std::vector<Entity*> wireframe_list;
	wireframe_list.clear();
	{
		Data::Render::Manager* manager = &Data::Render::manager;

		// Loop through each render batch
		for(int i=0; i<(int)manager->renderBatch_list.size(); i++)
		{
			// Bind correct buffers
			Buffer* vertexBuffer = manager->buffer_list[i].vertex;
			Buffer* indexBuffer = manager->buffer_list[i].index;
			if(vertexBuffer && indexBuffer)
			{
				vertexBuffer->setDeviceContextBuffer(m_dxDeviceContext);
				indexBuffer->setDeviceContextBuffer(m_dxDeviceContext);

				Batch<EntityPointer>* b = &manager->renderBatch_list[i];
				while(b->hasNext())
				{
					Entity* e = b->next()->asEntity();
					Data::Transform* d_transform = e->fetchData<Data::Transform>();
					Data::Render* d_render = e->fetchData<Data::Render>();

					if(!d_render->invisible)
					{
						m_CBPerObject.world = d_transform->toWorldMatrix();
						m_CBPerObject.WVP = m_CBPerObject.world * viewProjection;
						m_CBPerObject.WVP = XMMatrixTranspose(m_CBPerObject.WVP);
						m_CBPerObject.world = XMMatrixTranspose(m_CBPerObject.world);
						m_CBPerObject.color = d_render->mesh.color;
						m_dxDeviceContext->UpdateSubresource(m_objectConstantBuffer->getBuffer(), 0, nullptr, &m_CBPerObject, 0, 0);
						m_dxDeviceContext->DrawIndexed(indexBuffer->count(), 0, 0);
					}
				}
			}
		}

		m_CBPerFrame.drawDebug = 1;
		m_dxDeviceContext->RSSetState(RenderStates::WireframeNoCullRS);
		m_dxDeviceContext->OMSetDepthStencilState(RenderStates::LessEqualDSS, 0);
		m_dxDeviceContext->UpdateSubresource(m_frameConstantBuffer->getBuffer(), 0, nullptr, &m_CBPerFrame, 0, 0);

		// Render all selected meshes
		for(int i=0; i<(int)manager->renderBatch_list.size(); i++)
		{
			// Bind correct buffers
			Buffer* vertexBuffer = manager->buffer_list[i].vertex;
			Buffer* indexBuffer = manager->buffer_list[i].index;
			if(vertexBuffer && indexBuffer)
			{
				vertexBuffer->setDeviceContextBuffer(m_dxDeviceContext);
				indexBuffer->setDeviceContextBuffer(m_dxDeviceContext);

				Batch<EntityPointer>* b = &manager->renderBatch_list[i];
				while(b->hasNext())
				{
					Entity* e = b->next()->asEntity();
					Data::Transform* d_transform = e->fetchData<Data::Transform>();
					Data::Render* d_render = e->fetchData<Data::Render>();

					if(d_render->invisible)
					{
						m_CBPerObject.world = d_transform->toWorldMatrix();
						m_CBPerObject.WVP = m_CBPerObject.world * viewProjection;
						m_CBPerObject.WVP = XMMatrixTranspose(m_CBPerObject.WVP);
						m_CBPerObject.world = XMMatrixTranspose(m_CBPerObject.world);
						m_CBPerObject.color = d_render->mesh.color;
						m_dxDeviceContext->UpdateSubresource(m_objectConstantBuffer->getBuffer(), 0, nullptr, &m_CBPerObject, 0, 0);
						m_dxDeviceContext->DrawIndexed(indexBuffer->count(), 0, 0);
					}
				}
			}
		}
	}

	{
		Data::Render::Manager* manager = &Data::Render::manager;

		// Loop through each render batch
		for(int i=0; i<(int)manager->renderBatch_list.size(); i++)
		{
			// Bind correct buffers
			Buffer* vertexBuffer = manager->buffer_list[i].vertex;
			Buffer* indexBuffer = manager->buffer_list[i].index;
			if(vertexBuffer && indexBuffer)
			{
				vertexBuffer->setDeviceContextBuffer(m_dxDeviceContext);
				indexBuffer->setDeviceContextBuffer(m_dxDeviceContext);

				Batch<EntityPointer>* b = &manager->renderBatch_list[i];
				while(b->hasNext())
				{
					Entity* e = b->next()->asEntity();
					if(e->fetchData<Data::Selected>())
					{
						Data::Transform* d_transform = e->fetchData<Data::Transform>();
						Data::Render* d_render = e->fetchData<Data::Render>();

						m_CBPerObject.world = d_transform->toWorldMatrix();
						m_CBPerObject.WVP = m_CBPerObject.world * viewProjection;
						m_CBPerObject.WVP = XMMatrixTranspose(m_CBPerObject.WVP);
						m_CBPerObject.world = XMMatrixTranspose(m_CBPerObject.world);
						m_CBPerObject.color = d_render->mesh.color;
						m_dxDeviceContext->UpdateSubresource(m_objectConstantBuffer->getBuffer(), 0, nullptr, &m_CBPerObject, 0, 0);
						m_dxDeviceContext->DrawIndexed(indexBuffer->count(), 0, 0);
					}
				}
			}
		}
	}
	m_dxDeviceContext->RSSetState(0);
	m_dxDeviceContext->OMSetDepthStencilState(0, 0);


	// Draw SkyBox
	int skyboxIndex = SETTINGS()->skyboxIndex();
	if(skyboxIndex == 1)
		m_sky->draw();
	if(skyboxIndex == 2)
		m_sky2->draw();

	// Draw Tools
	m_manager_tools->update();
	m_manager_tools->draw(m_view_depthStencil);

	m_dxDeviceContext->RSSetState(0);
	m_dxDeviceContext->OMSetDepthStencilState(0, 0);

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
	if(m_msaa_enable)
	{
		desc_sc.SampleDesc.Count   = 4;
		desc_sc.SampleDesc.Quality = m_msaa_quality-1;
	}
	else
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
	HR(D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "pixelMain", "ps_4_0", NULL, NULL, &PS_Buffer, NULL));
	HR(m_dxDevice->CreatePixelShader( PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &m_pixelShader));

	HR(D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "vertexMain", "vs_4_0", NULL, NULL, &VS_Buffer, NULL));
	HR(m_dxDevice->CreateVertexShader( VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &m_vertexShader));

	m_dxDeviceContext->PSSetShader(m_pixelShader, 0, 0);
	m_dxDeviceContext->VSSetShader(m_vertexShader, 0, 0);

	// Create input layout

	D3D11_INPUT_ELEMENT_DESC inputElementDesc [] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},	
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HR(m_dxDevice->CreateInputLayout(inputElementDesc, 3, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &m_inputLayout));

	ReleaseCOM(VS_Buffer);
	ReleaseCOM(PS_Buffer);

	// Create meshes
	MeshData mesh;
	{
		// Box
		Factory_Geometry::instance()->createBox(1.0f, 1.0f, 1.0f, mesh);
		createMeshBuffer(Enum::Mesh_Box, mesh);

		// Sphere
		Factory_Geometry::instance()->createSphere(0.5f, 20, 20, mesh);
		createMeshBuffer(Enum::Mesh_Sphere, mesh);
		Factory_Geometry::instance()->createSphere(0.5f, 4, 4, mesh);
		createMeshBuffer(Enum::Mesh_Sphere_LowPoly, mesh);

		// Cylinder
		Factory_Geometry::instance()->createCylinder(0.5f, 0.5f, 1.0f, 20, 20, mesh);
		createMeshBuffer(Enum::Mesh_Cylinder, mesh);

		// Cone
		Factory_Geometry::instance()->createCylinder(0.5f, 0.0f, 1.0f, 20, 20, mesh);
		createMeshBuffer(Enum::Mesh_Cone, mesh);

		// Pyramid
		Factory_Geometry::instance()->createPyramid(1.0f, 1.0f, 1.0f, mesh);
		createMeshBuffer(Enum::Mesh_Pyramid, mesh);

		// Asteroid
		Factory_Geometry::instance()->createSphere(0.5f, 3, 3, mesh);
		createMeshBuffer(Enum::Mesh_Asteroid, mesh);

		// Al
		Factory_Geometry::instance()->loadModel("Al.obj", mesh);
		createMeshBuffer(Enum::Mesh_Al, mesh);
	}

	// Create per object constant buffer

	m_objectConstantBuffer = new Buffer();
	HR(m_objectConstantBuffer->init(Buffer::VS_CONSTANT_BUFFER, sizeof(float), sizeof(m_CBPerObject)/sizeof(float), &m_CBPerObject, m_dxDevice));
	m_objectConstantBuffer->setDeviceContextBuffer(m_dxDeviceContext);

	// Create per frame constant buffer

	m_frameConstantBuffer = new Buffer();
	HR(m_frameConstantBuffer->init(Buffer::PS_CONSTANT_BUFFER, sizeof(float), sizeof(m_CBPerFrame)/sizeof(float), &m_CBPerFrame, m_dxDevice));
	m_frameConstantBuffer->setDeviceContextBuffer(m_dxDeviceContext);

	//Factory_Geometry::MeshData sphere;
	//Factory_Geometry::instance()->createSphere(10.0f, 30, 30, sphere);

	//// Fetch positions
	//std::vector<Vector3> vertex_list(sphere.vertices.size());
	//for(size_t i = 0; i < sphere.vertices.size(); ++i)
	//{
	//	vertex_list[i] = sphere.vertices[i].position;
	//}

	//// Create vertex buffer
	//Buffer* m_vertexBuffer = new Buffer();
	//HR(m_vertexBuffer->init(Buffer::VERTEX_BUFFER, sizeof(Vector3), vertex_list.size(),  &vertex_list, m_dxDevice));
	//m_vertexBuffer->setDeviceContextBuffer(m_dxDeviceContext);

	//////////////////////////////////////////////////////////////////////////////////////////////

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
	if(m_msaa_enable)
	{
		desc_depthStencil.SampleDesc.Count   = 4;
		desc_depthStencil.SampleDesc.Quality = m_msaa_quality-1;
	}
	else
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

	SETTINGS()->DXViewPortTopLeftX = (int)m_viewport_screen->TopLeftX;
	SETTINGS()->DXViewPortTopLeftY = (int)m_viewport_screen->TopLeftY;
	SETTINGS()->DXViewPortMinDepth = (int)m_viewport_screen->MinDepth;
	SETTINGS()->DXViewPortMaxDepth = (int)m_viewport_screen->MaxDepth;
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

void DXRenderer::updatePointLights()
{
	DataMapper<Data::PointLight> map_pointLight;
	for(unsigned int i = 0; i < MAX_POINTLIGHTS; i++)
	{
		if(map_pointLight.hasNext())
		{
			Entity* e = map_pointLight.nextEntity();

			Data::Transform* transform =  e->fetchData<Data::Transform>();
			Data::PointLight* pointLight =  e->fetchData<Data::PointLight>();
			Data::Render* d_render =  e->fetchData<Data::Render>();
			
			float range = pointLight->range;
			m_CBPerFrame.plPosition[i] = Vector4(transform->position.x, transform->position.y, transform->position.z, 1.0f);
			m_CBPerFrame.plColorAndRange[i] = Vector4(d_render->mesh.color.x, d_render->mesh.color.y, d_render->mesh.color.z, range);
		}
		else
		{
			float range = 0.0f;
			m_CBPerFrame.plPosition[i] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
			m_CBPerFrame.plColorAndRange[i] = Vector4(0.0f, 0.0f, 0.0f, range);
		}
	}
}

void DXRenderer::createMeshBuffer( int meshId, MeshData &mesh )
{
	std::vector<VertexPosColNorm> vertex_list = mesh.createVertexList_posColNorm();
	std::vector<unsigned int> index_list = mesh.indexList();

	// Create vertex buffer
	Buffer* vertexBuffer = new Buffer();
	HR(vertexBuffer->init(Buffer::VERTEX_BUFFER, sizeof(VertexPosColNorm), vertex_list.size(), &vertex_list[0], m_dxDevice));
	vertexBuffer->setDeviceContextBuffer(m_dxDeviceContext);

	// Create index buffer
	Buffer* indexBuffer = new Buffer();
	HR(indexBuffer->init(Buffer::INDEX_BUFFER, sizeof(unsigned int), index_list.size(), &index_list[0], m_dxDevice));
	indexBuffer->setDeviceContextBuffer(m_dxDeviceContext);

	// Store
	Data::Render::manager.buffer_list[meshId].vertex = vertexBuffer;
	Data::Render::manager.buffer_list[meshId].index = indexBuffer;
}
