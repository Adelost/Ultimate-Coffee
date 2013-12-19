#include "stdafx.h"
#include "Sky.h"

#include "TextureLoader.h"

#include "MeshData.h"


Sky::~Sky()
{
	SafeDelete(m_vertexBuffer);
	SafeDelete(m_indexBuffer);
	SafeDelete(m_WVPBuffer);
	ReleaseCOM(m_inputLayout);
}

ID3D11ShaderResourceView* Sky::CubeMapSRV()
{
	return m_rv_cubeMap;
}

void Sky::draw()
{
	if(!m_hasInit)
	{
		init();
		m_hasInit = true;
	}

	m_context->VSSetShader(m_vertexShader, 0, 0);
	m_context->PSSetShader(m_pixelShader, 0, 0);
	m_context->PSSetShader(m_pixelShader, 0, 0);

	m_context->IASetInputLayout(m_inputLayout);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->RSSetState(RenderStates::NoCullRS);
	m_context->OMSetDepthStencilState(RenderStates::LessEqualDSS, 0);


	//((DeviceContext)GraphicsDevice).PixelShader.SetSampler(0, GraphicsDevice.SamplerStates.LinearWrap);


	// Find world
	Matrix viewProj = CAMERA_ENTITY()->fetchData<Data::Camera>()->viewProjection();
	Matrix mat_pos = CAMERA_ENTITY()->fetchData<Data::Transform>()->toPosMatrix();

	DataMapper<Data::Sky> map_sky;
	if(map_sky.hasNext())
	{
		Entity* e = map_sky.nextEntity();
		Data::Transform* d_transform = e->fetchData<Data::Transform>();
		Data::Render* d_render = e->fetchData<Data::Render>();
		Matrix world = d_transform->toRotMatrix() * mat_pos;

		// Set Shaders
		m_vertexBuffer->setDeviceContextBuffer(m_context);
		m_indexBuffer->setDeviceContextBuffer(m_context);
		m_WVPBuffer->setDeviceContextBuffer(m_context);
		m_context->PSSetShaderResources(0, 1, &(m_rv_cubeMap));
		m_cbuffer.WVP = world * viewProj;
		m_cbuffer.WVP = m_cbuffer.WVP.Transpose();
		m_cbuffer.color = d_render->mesh.color;
		m_context->UpdateSubresource(m_WVPBuffer->getBuffer(), 0, nullptr, &m_cbuffer, 0, 0);
		
		// Draw
		m_context->DrawIndexed(m_indexBuffer->count(), 0, 0);
	}

	// Restore states
	m_context->RSSetState(0);
	m_context->OMSetDepthStencilState(0, 0);
}

Sky::Sky( ID3D11Device* device, ID3D11DeviceContext* context, const std::string& cubemapFilename, float sphereRadius )
{
	m_hasInit = false;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_WVPBuffer = nullptr;
	m_inputLayout = nullptr;

	m_device = device;
	m_context = context;
	m_cubemapFilename = cubemapFilename;
	m_sphereRadius = sphereRadius;
}

void Sky::init()
{
	// Create box
	MeshData sphere;
	Factory_Geometry::instance()->createSphere(100.0f, 30, 30, sphere);
	std::vector<Vector3> vertex_list = sphere.positionList();
	std::vector<unsigned int> index_list = sphere.indexList();

	// Create vertex buffer
	m_vertexBuffer = new Buffer();
	m_indexBuffer = new Buffer();
	m_WVPBuffer = new Buffer();
	HR(m_vertexBuffer->init(Buffer::VERTEX_BUFFER, sizeof(Vector3), vertex_list.size(), &vertex_list[0], m_device));
	HR(m_indexBuffer->init(Buffer::INDEX_BUFFER, sizeof(unsigned int), index_list.size(), &index_list[0], m_device));
	HR(m_WVPBuffer->init(Buffer::VS_CONSTANT_BUFFER, sizeof(float), sizeof(m_cbuffer)/sizeof(float), &m_cbuffer, m_device));
	m_vertexBuffer->setDeviceContextBuffer(m_context);
	m_indexBuffer->setDeviceContextBuffer(m_context);
	m_WVPBuffer->setDeviceContextBuffer(m_context);

	// Load texture
	std::string texPath = TEXTURES_PATH;
	std::string texFileName = m_cubemapFilename;
	std::string fullPath = texPath + texFileName;
	createTextureFromFile(m_device, fullPath, &m_rv_cubeMap);

	// Create Shaders
	ID3DBlob *PS_Buffer, *VS_Buffer;

	//hr = D3DReadFileToBlob(L"PixelShader.cso", &PS_Buffer);
	HR(D3DCompileFromFile(L"Shader_Sky.hlsl", NULL, NULL, "VS", "vs_4_0", NULL, NULL, &VS_Buffer, NULL));
	HR(m_device->CreateVertexShader( VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &m_vertexShader));

	HR(D3DCompileFromFile(L"Shader_Sky.hlsl", NULL, NULL, "PS", "ps_4_0", NULL, NULL, &PS_Buffer, NULL));
	HR(m_device->CreatePixelShader( PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &m_pixelShader));

	m_context->VSSetShader(m_vertexShader, 0, 0);
	m_context->PSSetShader(m_pixelShader, 0, 0);

	// Create input layout
	D3D11_INPUT_ELEMENT_DESC desc_pos[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HR(m_device->CreateInputLayout(desc_pos, 1, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &m_inputLayout));

	ReleaseCOM(VS_Buffer);
	ReleaseCOM(PS_Buffer);
}
