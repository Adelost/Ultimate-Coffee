#include "stdafx.h"
#include "DXRenderer.h"
#include "Box.h"
#include "Manager_3DTools.h"
#include "Buffer.h"
#include "Sky.h"
#include "RenderStates.h"

#include "MeshLoader.h"
#include "TextureLoader.h"

#include "OculusManager.h"

DXRenderer::DXRenderer()
{
	m_oculus = new OculusManager();

	SUBSCRIBE_TO_EVENT(this, EVENT_SET_BACKBUFFER_COLOR);
	SUBSCRIBE_TO_EVENT(this, EVENT_WINDOW_RESIZE);
	SUBSCRIBE_TO_EVENT(this, EVENT_COFFEE);


	m_device = nullptr;
	m_context = nullptr;
	m_swapChain = nullptr;

	m_rtv_renderTarget = nullptr;
	m_dsv_depthStencil = nullptr;
	m_tex_depthStencil = nullptr;
	m_viewport_screen = nullptr;

	m_viewport_stereo.resize(2);
	m_viewport_stereo[0] = nullptr;
	m_viewport_stereo[1] = nullptr;

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
	delete m_oculus;

	// Delete buffers
	std::vector<Data::Render::BufferStore>* b = &Data::Render::manager.buffer_list;
	for(int i=0; i<(int)b->size(); i++)
	{
		delete b->at(i).vertex;
		delete b->at(i).index;
	}

	SafeDelete(m_objectConstantBuffer);
	SafeDelete(m_frameConstantBuffer);
	m_layout.clear();
	ReleaseCOM(m_pixelShader);
	ReleaseCOM(m_vertexShader);
	m_screenQuad.clear();
	ReleaseCOM(m_rtv_renderTarget);
	ReleaseCOM(m_dsv_depthStencil);
	ReleaseCOM(m_swapChain);
	RenderStates::DestroyAll();
	ReleaseCOM(m_tex_depthStencil);
	if(m_context)
		m_context->ClearState();
	
	ReleaseCOM(m_ssDefault);

	ReleaseCOM(m_context);
	ReleaseCOM(m_device);


	delete m_viewport_screen;

	delete m_viewport_stereo[0];
	delete m_viewport_stereo[1];

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

	delete m_viewport_stereo[0];
	delete m_viewport_stereo[1];
	m_viewport_stereo[0] = new D3D11_VIEWPORT();
	m_viewport_stereo[1] = new D3D11_VIEWPORT();

	SETTINGS()->DXViewPortTopLeftX = (int)m_viewport_screen->TopLeftX;
	SETTINGS()->DXViewPortTopLeftY = (int)m_viewport_screen->TopLeftY;
	SETTINGS()->DXViewPortMinDepth = (int)m_viewport_screen->MinDepth;
	SETTINGS()->DXViewPortMaxDepth = (int)m_viewport_screen->MaxDepth;

	result = initDX();
	// Init RenderStates
	RenderStates::InitAll(m_device);

	m_manager_tools = new Manager_3DTools(this->m_device, this->m_context, this->m_screenQuad.tex_dsv, this->m_viewport_screen);

	m_sky = new Sky(m_device, m_context, "skybox_clearsky.dds", 5000.0f);
	m_sky2 = new Sky(m_device, m_context, "skybox_redclouds.dds", 5000.0f);


	// HACK: Testing texture
	std::string texPath = TEXTURES_PATH;
	//std::string texFileName = "test_uv.dds";
	std::string texFileName = "test_uv_stereo.dds";
	//std::string texFileName = "fake.dds";
	std::string fullPath = texPath + texFileName;
	createTextureFromFile(m_device, fullPath, &m_screenQuad.rv_texture);
	
	
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
			Event_WindowResize* e = static_cast<Event_WindowResize*>(p_event);
			m_clientWidth = e->width;
			m_clientHeight = e->height;

			// Resize cameras
			m_oculus->update();
			DataMapper<Data::Camera> map_camera;
			while(map_camera.hasNext())
			{
				Data::Camera* d_camera = map_camera.next();
				float aspectRatio =  static_cast<float>(m_clientWidth)/m_clientHeight;

				float fov = m_oculus->YFOVRadians;
				float as = m_oculus->Aspect;
				d_camera->setLens(fov, as, 0.01f, 3000.0f);
			}

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
	// Fetch orientation from Oculus Rift
	m_oculus->update();
	Quaternion q = m_oculus->GetOrientation();
	SETTINGS()->entity_camera->fetchData<Data::Camera>()->m_rotationOffset = q;

	m_context->PSSetShaderResources(0, 1, &(m_screenQuad.rv_texture));

	// Restore the back and depth buffer to the OM stage.
	m_context->RSSetViewports(1, m_viewport_screen);

	// HACK: Hackish way to render scene multiple time if multiple render target is used
	int renderCount;
	ID3D11DepthStencilView* dsv;
	ID3D11RenderTargetView* rtv;
	if(SETTINGS()->bOculusRift)
	{
		renderCount = 2;
		rtv = m_screenQuad.tex_rtv;
		dsv = m_screenQuad.tex_dsv;
		
	}
	else
	{
		renderCount = 1;
		rtv = m_rtv_renderTarget;
		dsv = m_dsv_depthStencil;
	}
	m_context->OMSetRenderTargets(1, &m_rtv_renderTarget, m_dsv_depthStencil);

	// Clear render target & depth/stencil
	m_context->ClearRenderTargetView(m_rtv_renderTarget, static_cast<const float*>(Color(0.0f, 0.9f, 0.5f)));
	m_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

	while(renderCount > 0)
	{
		renderCount--;

		// Choose correct viewport
		if(SETTINGS()->bOculusRift)
		{
			if(renderCount > 0)
			{
				m_oculus->updateVars(Enum::Direction::Left);
				m_context->RSSetViewports(1, m_viewport_stereo[0]);
			}
			else
			{
				m_oculus->updateVars(Enum::Direction::Right);
				m_context->RSSetViewports(1, m_viewport_stereo[1]);
			}	
		}
		else
		{
			m_context->RSSetViewports(1, m_viewport_screen);
		}

		m_context->IASetInputLayout(m_layout.posColNorm);
		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_context->PSSetShader(m_pixelShader, 0, 0);
		m_context->VSSetShader(m_vertexShader, 0, 0);

		

		m_objectConstantBuffer->setDeviceContextBuffer(m_context);
		m_frameConstantBuffer->setDeviceContextBuffer(m_context);

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
		m_context->UpdateSubresource(m_frameConstantBuffer->getBuffer(), 0, nullptr, &m_CBPerFrame, 0, 0);

		Matrix viewProjection;
		// HACK: Adding camera to renderer
		// don't know where else to put it.
		// Please move to better location.
		{
			Data::Camera* d_camera = CAMERA_ENTITY()->fetchData<Data::Camera>();

			Matrix projection = d_camera->projection();
			Matrix projection2 = m_oculus->Projection;
			Matrix view = d_camera->view();

			Vector4 test1 = Vector4::Transform(Vector4(0.0f, 0.0f, 1.0f, 1.0), projection);
			Vector4 test2 =  Vector4::Transform(Vector4(0.0f, 0.0f, 1.0f, 1.0), projection2);

			viewProjection = view * m_oculus->ViewAdjust * projection;
			
			// Frustum culling
			if(Data::Bounding::s_doFrustumCulling)
			{
				BoundingFrustum frustum = d_camera->toFrustum();
				Data::Bounding::performFrustumCulling(frustum);
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
					vertexBuffer->setDeviceContextBuffer(m_context);
					indexBuffer->setDeviceContextBuffer(m_context);

					Batch<EntityPointer>* b = &manager->renderBatch_list[i];
					while(b->hasNext())
					{
						Entity* e = b->next()->asEntity();
						Data::Transform* d_transform = e->fetchData<Data::Transform>();
						Data::Render* d_render = e->fetchData<Data::Render>();

						bool insideFrustum = true;
						Data::Bounding* d_bounding = e->fetchData<Data::Bounding>();
						if(d_bounding)
						{
							insideFrustum = d_bounding->insideFrustum;
						}


						if(!d_render->invisible && insideFrustum)
						{
							m_CBPerObject.world = d_transform->toWorldMatrix();
							m_CBPerObject.WVP = m_CBPerObject.world * viewProjection;
							m_CBPerObject.WVP = XMMatrixTranspose(m_CBPerObject.WVP);
							m_CBPerObject.world = XMMatrixTranspose(m_CBPerObject.world);
							m_CBPerObject.color = d_render->mesh.color;
							m_context->UpdateSubresource(m_objectConstantBuffer->getBuffer(), 0, nullptr, &m_CBPerObject, 0, 0);
							m_context->DrawIndexed(indexBuffer->count(), 0, 0);
						}
					}
				}
			}

			m_CBPerFrame.drawDebug = 1;
			m_context->RSSetState(RenderStates::WireframeNoCullRS);
			m_context->OMSetDepthStencilState(RenderStates::LessEqualDSS, 0);
			m_context->UpdateSubresource(m_frameConstantBuffer->getBuffer(), 0, nullptr, &m_CBPerFrame, 0, 0);

			// Render all selected meshes
			for(int i=0; i<(int)manager->renderBatch_list.size(); i++)
			{
				// Bind correct buffers
				Buffer* vertexBuffer = manager->buffer_list[i].vertex;
				Buffer* indexBuffer = manager->buffer_list[i].index;
				if(vertexBuffer && indexBuffer)
				{
					vertexBuffer->setDeviceContextBuffer(m_context);
					indexBuffer->setDeviceContextBuffer(m_context);

					Batch<EntityPointer>* b = &manager->renderBatch_list[i];
					while(b->hasNext())
					{
						Entity* e = b->next()->asEntity();
						Data::Transform* d_transform = e->fetchData<Data::Transform>();
						Data::Render* d_render = e->fetchData<Data::Render>();
						Data::Bounding* d_bounding = e->fetchData<Data::Bounding>();

						bool insideFrustum = true;
						if(d_bounding)
						{
							insideFrustum = d_bounding->insideFrustum;
						}

						if(d_render->invisible && d_bounding && insideFrustum)
						{
							m_CBPerObject.world = d_transform->toWorldMatrix();
							m_CBPerObject.WVP = m_CBPerObject.world * viewProjection;
							m_CBPerObject.WVP = XMMatrixTranspose(m_CBPerObject.WVP);
							m_CBPerObject.world = XMMatrixTranspose(m_CBPerObject.world);
							m_CBPerObject.color = d_render->mesh.color;
							m_context->UpdateSubresource(m_objectConstantBuffer->getBuffer(), 0, nullptr, &m_CBPerObject, 0, 0);
							m_context->DrawIndexed(indexBuffer->count(), 0, 0);
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
					vertexBuffer->setDeviceContextBuffer(m_context);
					indexBuffer->setDeviceContextBuffer(m_context);

					Batch<EntityPointer>* b = &manager->renderBatch_list[i];
					while(b->hasNext())
					{
						Entity* e = b->next()->asEntity();
						if(e->fetchData<Data::Selected>())
						{
							Data::Transform* d_transform = e->fetchData<Data::Transform>();
							Data::Render* d_render = e->fetchData<Data::Render>();

							bool insideFrustum = true;
							Data::Bounding* d_bounding = e->fetchData<Data::Bounding>();
							if(d_bounding)
							{
								insideFrustum = d_bounding->insideFrustum;
							}

							if(insideFrustum)
							{
								m_CBPerObject.world = d_transform->toWorldMatrix();
								m_CBPerObject.WVP = m_CBPerObject.world * viewProjection;
								m_CBPerObject.WVP = XMMatrixTranspose(m_CBPerObject.WVP);
								m_CBPerObject.world = XMMatrixTranspose(m_CBPerObject.world);
								m_CBPerObject.color = d_render->mesh.color;
								m_context->UpdateSubresource(m_objectConstantBuffer->getBuffer(), 0, nullptr, &m_CBPerObject, 0, 0);
								m_context->DrawIndexed(indexBuffer->count(), 0, 0);
							}
						}
					}
				}
			}
		}
		m_context->RSSetState(0);
		m_context->OMSetDepthStencilState(0, 0);

		

		// Draw SkyBox
		int skyboxIndex = SETTINGS()->skyboxIndex();
		if(skyboxIndex == 1)
			m_sky->draw();
		if(skyboxIndex == 2)
			m_sky2->draw();

		m_context->RSSetState(0);
		m_context->OMSetDepthStencilState(0, 0);

		// Draw Tools
		m_manager_tools->update();
		m_manager_tools->draw(m_screenQuad.tex_dsv);

		m_context->RSSetState(0);
		m_context->OMSetDepthStencilState(0, 0);
	}

	if(SETTINGS()->bOculusRift)
	{
		rtv = m_rtv_renderTarget;
		dsv = m_dsv_depthStencil;

		m_context->OMSetRenderTargets(1, &m_rtv_renderTarget, m_dsv_depthStencil);
		m_context->RSSetViewports(1, m_viewport_screen);
		drawScreenQuad(m_screenQuad.tex_srv);
	}

	// Show the finished frame
	HR(m_swapChain->Present(0, 0));

	// Restore rtv
	//m_context->OMSetRenderTargets(1, &m_rtv_renderTarget, m_screenQuad.tex_dsv);
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
		&m_device,				// returns d3dDevice
		&featureLevel,				// returns feature level
		&m_context);		// returns device context
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

	// Describe swap chain
	DXGI_SWAP_CHAIN_DESC desc_sc;
	// buffer description = back buffer properties
	desc_sc.BufferDesc.Width = 800;					// width
	desc_sc.BufferDesc.Height = 600;					// height
	desc_sc.BufferDesc.RefreshRate.Numerator = 60;
	desc_sc.BufferDesc.RefreshRate.Denominator = 1;
	desc_sc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// set pixel format
	desc_sc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc_sc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// sample description, used to set MSAA
	HR(m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_msaa_quality));
	assert(m_msaa_quality > 0);
	if(m_msaa_enable && false)
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
	HR(m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));
	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));
	// finally the IDXGIFactory interface
	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));
	// create swap chain
	HR(dxgiFactory->CreateSwapChain(m_device, &desc_sc, &m_swapChain));
	// release acquired COM interfaces since we're done with them
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	////////////////////////////////////////////////////////////////////////////////////////////

	// Compile and set shaders
	{
		ID3DBlob *PS_Buffer, *VS_Buffer;

		//hr = D3DReadFileToBlob(L"PixelShader.cso", &PS_Buffer);
		HR(D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "vertexMain", "vs_4_0", NULL, NULL, &VS_Buffer, NULL));
		HR(m_device->CreateVertexShader( VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &m_vertexShader));
		HR(D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "pixelMain", "ps_4_0", NULL, NULL, &PS_Buffer, NULL));
		HR(m_device->CreatePixelShader( PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &m_pixelShader));

		// Create input layout
		{
			D3D11_INPUT_ELEMENT_DESC inputElementDesc [] = 
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},	
				{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};
			HR(m_device->CreateInputLayout(inputElementDesc, 3, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &m_layout.posColNorm));
		}


		ReleaseCOM(VS_Buffer);
		ReleaseCOM(PS_Buffer);
	}
	{
		ID3DBlob *PS_Buffer, *VS_Buffer;
		HR(D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "VS", "vs_4_0", NULL, NULL, &VS_Buffer, NULL));
		HR(m_device->CreateVertexShader( VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &m_screenQuad.shader_vertex));
		HR(D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "PS", "ps_4_0", NULL, NULL, &PS_Buffer, NULL));
		HR(m_device->CreatePixelShader( PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &m_screenQuad.shader_pixel));

		{
			MeshData quad;
			Factory_Geometry::instance()->createFullscreenQuad(quad);

			std::vector<Vertex::PosNormTex> vertices(quad.vertices.size());
			for(UINT i = 0; i < quad.vertices.size(); ++i)
			{
				vertices[i].Pos    = quad.vertices[i].position;
				vertices[i].Normal = quad.vertices[i].normal;
				vertices[i].Tex    = quad.vertices[i].texureCordinate;
			}

			D3D11_BUFFER_DESC vbd;
			vbd.Usage = D3D11_USAGE_IMMUTABLE;
			vbd.ByteWidth = sizeof(Vertex::PosNormTex) * quad.vertices.size();
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = 0;
			vbd.MiscFlags = 0;
			D3D11_SUBRESOURCE_DATA vinitData;
			vinitData.pSysMem = &vertices[0];
			HR(m_device->CreateBuffer(&vbd, &vinitData, &m_screenQuad.vertexBuffer));

			//
			// Pack the indices of all the meshes into one index buffer.
			//

			D3D11_BUFFER_DESC ibd;
			ibd.Usage = D3D11_USAGE_IMMUTABLE;
			ibd.ByteWidth = sizeof(UINT) * quad.indices.size();
			ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			ibd.CPUAccessFlags = 0;
			ibd.MiscFlags = 0;
			D3D11_SUBRESOURCE_DATA iinitData;
			iinitData.pSysMem = &quad.indices[0];
			HR(m_device->CreateBuffer(&ibd, &iinitData, &m_screenQuad.indexBuffer));

			D3D11_INPUT_ELEMENT_DESC inputElementDesc [] = 
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
			HR(m_device->CreateInputLayout(inputElementDesc, 3, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &m_layout.posNormTex));


			m_screenQuad.cb_oculus = new Buffer();
			m_screenQuad.cb_oculus->init(Buffer::CONSTANT_BUFFER, sizeof(float), sizeof(m_CBOculus)/sizeof(float), &m_CBOculus, m_device);
			m_screenQuad.cb_oculus->setDeviceContextBuffer(m_context, 2);

		}


		ReleaseCOM(VS_Buffer);
		ReleaseCOM(PS_Buffer);
	}
	

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
// 		Factory_Geometry::instance()->loadModel("Al.obj", mesh);
// 		createMeshBuffer(Enum::Mesh_Al, mesh);
// 
// 		// Shuttle
// 		Factory_Geometry::instance()->loadModel("Shuttle.obj", mesh);
// 		createMeshBuffer(Enum::Mesh_Shuttle, mesh);
	}

	// Create per object constant buffer

	m_objectConstantBuffer = new Buffer();
	HR(m_objectConstantBuffer->init(Buffer::VS_CONSTANT_BUFFER, sizeof(float), sizeof(m_CBPerObject)/sizeof(float), &m_CBPerObject, m_device));
	m_objectConstantBuffer->setDeviceContextBuffer(m_context, 0);

	// Create per frame constant buffer

	m_frameConstantBuffer = new Buffer();
	HR(m_frameConstantBuffer->init(Buffer::PS_CONSTANT_BUFFER, sizeof(float), sizeof(m_CBPerFrame)/sizeof(float), &m_CBPerFrame, m_device));
	m_frameConstantBuffer->setDeviceContextBuffer(m_context, 1);

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


	// Create sampler state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter		= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU	= D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV	= D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW	= D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD		= 0;
	sampDesc.MaxLOD		= D3D11_FLOAT32_MAX;
	hr = m_device->CreateSamplerState(&sampDesc, &m_ssDefault);


	m_context->PSSetSamplers(0, 1, &m_ssDefault);

	return true;
}

void DXRenderer::resizeDX()
{
	// Release old views, they hold references to buffers we will be destroying; release the old depth/stencil buffer
	ReleaseCOM(m_rtv_renderTarget);
	ReleaseCOM(m_dsv_depthStencil);
	ReleaseCOM(m_tex_depthStencil);
	assert(m_context);
	assert(m_device);
	assert(m_swapChain);

	// Render buffer dimension, should match viewport dimension
	int width = m_clientWidth;
	int height = m_clientHeight;

	// Resize the swap chain
	HR(m_swapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	// Create render target view
	ID3D11Texture2D* backBuffer;
	// get pointer to backbuffer
	HR(m_swapChain->GetBuffer(
		0,											// backbuffer index (in case there is more than one)
		__uuidof(ID3D11Texture2D),					// interface type of the buffer
		reinterpret_cast<void**>(&backBuffer)));	// returns backbuffer pointer
	// create render target view
	HR(m_device->CreateRenderTargetView(
		backBuffer,				// resource to be used as render target
		0,						// pointer to D3D11_RENDER_TARGET_VIEW_DESC (only needed when using typless format)
		&m_rtv_renderTarget));	// returns pointer to render target view
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC desc_depthStencil; 
	desc_depthStencil.Width     = width;					// width
	desc_depthStencil.Height    = height;					// height
	desc_depthStencil.MipLevels = 1;								// nr of mipmap levels
	desc_depthStencil.ArraySize = 1;								// nr of textures in a texture array
	desc_depthStencil.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;	// format
	// set MSSA, settings must match those in swap chain
	if(m_msaa_enable && true)
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
	HR(m_device->CreateTexture2D(&desc_depthStencil, 0, &m_tex_depthStencil));
	HR(m_device->CreateDepthStencilView(m_tex_depthStencil, 0, &m_dsv_depthStencil));

	// Bind render target view and depth/stencil view to pipeline
	m_context->OMSetRenderTargets(
		1,							// nr of render targets
		&m_rtv_renderTarget,		// first element of array of rendertargets
		m_dsv_depthStencil);		// pointer to depth/stencil view

	// Set viewport transform.
	m_viewport_screen->TopLeftX = 0;
	m_viewport_screen->TopLeftY = 0;
	m_viewport_screen->Width    = static_cast<float>(m_clientWidth);
	m_viewport_screen->Height   = static_cast<float>(m_clientHeight);
	m_viewport_screen->MinDepth = 0.0f;
	m_viewport_screen->MaxDepth = 1.0f;
	m_context->RSSetViewports(
		1,							// nr of viewports
		m_viewport_screen);			// viewport array

	// Stereo viewports
	{
		D3D11_VIEWPORT*	port = m_viewport_stereo[0];
		port->TopLeftX = 0;
		port->TopLeftY = 0;
		port->Width    = static_cast<float>(m_clientWidth/2.0f);
		port->Height   = static_cast<float>(m_clientHeight);
		port->MinDepth = 0.0f;
		port->MaxDepth = 1.0f;

		port = m_viewport_stereo[1];
		port->TopLeftX = (float)m_clientWidth/2;
		port->TopLeftY = 0;
		port->Width    = static_cast<float>(m_clientWidth/2.0f);
		port->Height   = static_cast<float>(m_clientHeight);
		port->MinDepth = 0.0f;
		port->MaxDepth = 1.0f;
	}

	SETTINGS()->DXViewPortTopLeftX = (int)m_viewport_screen->TopLeftX;
	SETTINGS()->DXViewPortTopLeftY = (int)m_viewport_screen->TopLeftY;
	SETTINGS()->DXViewPortMinDepth = (int)m_viewport_screen->MinDepth;
	SETTINGS()->DXViewPortMaxDepth = (int)m_viewport_screen->MaxDepth;

	buildScreenQuad();
}

ID3D11Device* DXRenderer::getDevice()
{
	return this->m_device;
}

ID3D11DeviceContext* DXRenderer::getDeviceContext()
{
	return this->m_context;
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
	HR(vertexBuffer->init(Buffer::VERTEX_BUFFER, sizeof(VertexPosColNorm), vertex_list.size(), &vertex_list[0], m_device));
	vertexBuffer->setDeviceContextBuffer(m_context);

	// Create index buffer
	Buffer* indexBuffer = new Buffer();
	HR(indexBuffer->init(Buffer::INDEX_BUFFER, sizeof(unsigned int), index_list.size(), &index_list[0], m_device));
	indexBuffer->setDeviceContextBuffer(m_context);

	// Store
	Data::Render::manager.buffer_list[meshId].vertex = vertexBuffer;
	Data::Render::manager.buffer_list[meshId].index = indexBuffer;
	Data::Render::manager.buffer_list[meshId].largestValue = mesh.largestValue;
}

void DXRenderer::drawScreenQuad( ID3D11ShaderResourceView* resource )
{
	UINT stride = sizeof(Vertex::PosNormTex);
	UINT offset = 0;

	m_context->IASetInputLayout(m_layout.posNormTex);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetVertexBuffers(0, 1, &m_screenQuad.vertexBuffer, &stride, &offset);
	m_context->IASetIndexBuffer(m_screenQuad.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_context->VSSetShader(m_screenQuad.shader_vertex, 0, 0);
	m_context->PSSetShader(m_screenQuad.shader_pixel, 0, 0);
	m_context->PSSetShaderResources(0, 1, &(resource));
	
	m_CBOculus.bStereo = SETTINGS()->bOculusRift;

	
	// Draw single
	if(!SETTINGS()->bOculusRift)
	{
		XMMATRIX world(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		m_CBOculus.world = XMMatrixTranspose(world);
		m_context->UpdateSubresource(m_screenQuad.cb_oculus->getBuffer(), 0, nullptr, &m_CBOculus, 0, 0);
		m_context->DrawIndexed(6, 0, 0);
	}
	// Draw stereo
	else
	{
		// Left Eye
		{
			m_oculus->updateVars(Enum::Direction::Left);
			m_CBOculus.LensCenter	= m_oculus->LensCenter;
			m_CBOculus.ScreenCenter = m_oculus->ScreenCenter;
			m_CBOculus.Scale		= m_oculus->Scale;
			m_CBOculus.ScaleIn		= m_oculus->ScaleIn;
			m_CBOculus.HmdWarpParam = m_oculus->HmdWarpParam;

			XMMATRIX world(
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				-0.5f, 0.0f, 0.0f, 1.0f);
			m_CBOculus.world = XMMatrixTranspose(world);
			m_context->UpdateSubresource(m_screenQuad.cb_oculus->getBuffer(), 0, nullptr, &m_CBOculus, 0, 0);
			m_context->DrawIndexed(6, 0, 0);
		}


		{
			m_oculus->updateVars(Enum::Direction::Right);
			m_CBOculus.LensCenter	= m_oculus->LensCenter;
			m_CBOculus.ScreenCenter = m_oculus->ScreenCenter;
			m_CBOculus.Scale		= m_oculus->Scale;
			m_CBOculus.ScaleIn		= m_oculus->ScaleIn;
			m_CBOculus.HmdWarpParam = m_oculus->HmdWarpParam;

			XMMATRIX world(
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.0f, 0.0f, 1.0f);
			m_CBOculus.world = XMMatrixTranspose(world);
			m_context->UpdateSubresource(m_screenQuad.cb_oculus->getBuffer(), 0, nullptr, &m_CBOculus, 0, 0);
			m_context->DrawIndexed(6, 0, 0);
		}
	}
}

void DXRenderer::buildScreenQuad()
{
	ReleaseCOM(m_screenQuad.tex_rtv);
	ReleaseCOM(m_screenQuad.tex_srv);
	ReleaseCOM(m_screenQuad.tex_dsv);

	//
	// Cubemap is a special texture array with 6 elements.
	//

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = m_clientWidth;
	texDesc.Height = m_clientHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* cubeTex;
	HR(m_device->CreateTexture2D(&texDesc, nullptr, &cubeTex));

	HR(m_device->CreateRenderTargetView(cubeTex, nullptr, &m_screenQuad.tex_rtv));
	HR(m_device->CreateShaderResourceView(cubeTex, nullptr, &m_screenQuad.tex_srv));

	//
	// Create a render target view to each cube map face 
	// (i.e., each element in the texture array).
	// 

// 	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
// 	rtvDesc.Format = texDesc.Format;
// 	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
// 	rtvDesc.Texture2D.MipSlice = 0;


	

	//
	// Create a shader resource view to the cube map.
	//

// 	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
// 	srvDesc.Format = texDesc.Format;
// 	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
// 	srvDesc.Texture2D.MostDetailedMip = 0;
// 	srvDesc.Texture2D.MipLevels = 1;


	

	ReleaseCOM(cubeTex);

	//
	// We need a depth texture for rendering the scene into the cubemap
	// that has the same resolution as the cubemap faces.  
	//

	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = m_clientWidth;
	depthTexDesc.Height = m_clientHeight;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// set MSSA, settings must match those in swap chain
	if(m_msaa_enable  && false)
	{
		depthTexDesc.SampleDesc.Count   = 4;
		depthTexDesc.SampleDesc.Quality = m_msaa_quality-1;
	}
	else
	{
		depthTexDesc.SampleDesc.Count   = 1;
		depthTexDesc.SampleDesc.Quality = 0;
	}
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	ID3D11Texture2D* depthTex = 0;
	HR(m_device->CreateTexture2D(&depthTexDesc, 0, &depthTex));

	// Create the depth stencil view for the entire cube
// 	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
// 	dsvDesc.Format = depthTexDesc.Format;
// 	dsvDesc.Flags  = 0;
// 	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
// 	dsvDesc.Texture2D.MipSlice = 0;
	HR(m_device->CreateDepthStencilView(depthTex, 0, &m_screenQuad.tex_dsv));

	ReleaseCOM(depthTex);
}

void DXRenderer::Manager_InputLayout::clear()
{
	ReleaseCOM(posColNorm);
	ReleaseCOM(posNormTex);
}

void DXRenderer::ScreenQuad::clear()
{
	ReleaseCOM(vertexBuffer);
	ReleaseCOM(indexBuffer);
	ReleaseCOM(indexBuffer);
	SafeDelete(cb_oculus);
 	ReleaseCOM(shader_vertex);
 	ReleaseCOM(shader_pixel);

	ReleaseCOM(tex_rtv);
	ReleaseCOM(tex_dsv);
	ReleaseCOM(tex_srv);
}

DXRenderer::ScreenQuad::ScreenQuad()
{
	tex_rtv = nullptr;
	tex_dsv = nullptr;
	tex_srv = nullptr;
}
