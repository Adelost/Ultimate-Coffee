#include "stdafx.h"
#include "Sky.h"

#include "TextureLoader.h"



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

void Sky::draw( ID3D11DeviceContext* dc )
{
	// Center sky about eye in world space
	Vector3 eyePos;// = camera->GetPosition();
	Matrix T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
	Matrix scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	Matrix viewProj = CAMERA_ENTITY()->fetchData<Data::Camera>()->viewProjection();
	Matrix world = CAMERA_ENTITY()->fetchData<Data::Transform>()->toWorldMatrix();
	Matrix WVP = XMMatrixMultiply(scale*T, viewProj);

	// Set WorldViewProj
	m_cbuffer.WVP = world * viewProj;
	m_cbuffer.WVP = m_cbuffer.WVP.Transpose();
	m_context->UpdateSubresource(m_WVPBuffer->getBuffer(), 0, nullptr, &m_cbuffer, 0, 0);

	// Set shaders
	m_context->VSSetShader(m_vertexShader, 0, 0);
	m_context->PSSetShader(m_pixelShader, 0, 0);
	m_vertexBuffer->setDeviceContextBuffer(m_context);
	m_indexBuffer->setDeviceContextBuffer(m_context);
	m_WVPBuffer->setDeviceContextBuffer(m_context);
	m_context->PSSetShaderResources(0, 1, &m_rv_cubeMap);

	/*m_context->IASetInputLayout(shaderManager->layout_pos);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DX11_TECHNIQUE_DESC techDesc;
	fx->SkyTech->GetDesc( &techDesc );

	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = fx->SkyTech->GetPassByIndex(p);

		pass->Apply(0, dc);

		dc->DrawIndexed(mIndexCount, 0, 0);
	}*/
}

Sky::Sky( ID3D11Device* device, ID3D11DeviceContext* context, const std::string& cubemapFilename, float sphereRadius )
{
	m_device = device;
	m_context = context;

	// Create box
	Factory_Geometry::MeshData sphere;
	Factory_Geometry::instance()->createSphere(2.0f, 30, 30, sphere);
	std::vector<Vector3> vertex_list = sphere.positionList();
	std::vector<unsigned int> index_list = sphere.indexList();

	// Create vertex buffer
	m_vertexBuffer = new Buffer();
	m_indexBuffer = new Buffer();
	m_WVPBuffer = new Buffer();
	HR(m_vertexBuffer->init(Buffer::VERTEX_BUFFER, sizeof(Vector3), vertex_list.size(), &vertex_list[0], m_device));
	HR(m_indexBuffer->init(Buffer::INDEX_BUFFER, sizeof(unsigned int), index_list.size(), &index_list[0], m_device));
	HR(m_WVPBuffer->init(Buffer::CONSTANT_BUFFER, sizeof(float), 16, &m_cbuffer, m_device));
	m_vertexBuffer->setDeviceContextBuffer(m_context);
	m_indexBuffer->setDeviceContextBuffer(m_context);
	m_WVPBuffer->setDeviceContextBuffer(m_context);

	// Load texture
	std::string texPath = TEXTURES_PATH;
	std::string texFileName = "default.dds";
	std::string fullPath = texPath + texFileName;
	createTextureFromFile(device, fullPath, &m_rv_cubeMap);



	// Create shaders

	ID3DBlob *PS_Buffer, *VS_Buffer;

	//hr = D3DReadFileToBlob(L"PixelShader.cso", &PS_Buffer);
	HR(D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "pixelMain", "ps_4_0", NULL, NULL, &PS_Buffer, NULL));
	HR(m_device->CreatePixelShader( PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &m_pixelShader));

	HR(D3DCompileFromFile(L"VertexShader.hlsl", NULL, NULL, "vertexMain", "vs_4_0", NULL, NULL, &VS_Buffer, NULL));
	HR(m_device->CreateVertexShader( VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &m_vertexShader));

	m_context->PSSetShader(m_pixelShader, 0, 0);
	m_context->VSSetShader(m_vertexShader, 0, 0);

	// Create input layout
	D3D11_INPUT_ELEMENT_DESC desc_pos[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HR(m_device->CreateInputLayout(desc_pos, 1, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &m_inputLayout));

	ReleaseCOM(VS_Buffer);
	ReleaseCOM(PS_Buffer);



}
