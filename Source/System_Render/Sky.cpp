#include "stdafx.h"
#include "Sky.h"

#include "TexLoader.h"



Sky::~Sky()
{
	SafeDelete(m_vertexBuffer);
	SafeDelete(m_indexBuffer);
	SafeDelete(m_WVPBuffer);
}

ID3D11ShaderResourceView* Sky::CubeMapSRV()
{
	return m_rv_cubeMap;
}

void Sky::draw( ID3D11DeviceContext* dc )
{
	// Center sky about eye in world space
	// HACK:
	Vector3 eyePos;// = camera->GetPosition();
	Matrix T = XMMatrixTranslation(eyePos.x, eyePos.y+height_offset, eyePos.z);

	Matrix scale = XMMatrixScaling(1.0f, height_scale, 1.0f);
	// HACK
	Matrix viewProj = CAMERA_ENTITY()->fetchData<Data::Camera>()->viewProjection();
	Matrix world = CAMERA_ENTITY()->fetchData<Data::Transform>()->toWorldMatrix();
	Matrix WVP = XMMatrixMultiply(scale*T, viewProj);

	// Set WorldViewProj
	m_cbuffer.WVP = world * viewProj;
	m_cbuffer.WVP = m_cbuffer.WVP.Transpose();
	m_context->UpdateSubresource(m_WVPBuffer->getBuffer(), 0, nullptr, &m_cbuffer, 0, 0);

	// Set cube map
	m_context->PSSetShaderResources(0, 1, &m_rv_cubeMap);

	unsigned int stride = sizeof(Vector3);
	unsigned int  offset = 0;
	/*dc->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	dc->IASetIndexBuffer(mIB, DXGI_FORMAT_R16_UINT, 0);
	dc->IASetInputLayout(shaderManager->layout_pos);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);*/

	//D3DX11_TECHNIQUE_DESC techDesc;
	//fx->SkyTech->GetDesc( &techDesc );

	//for(UINT p = 0; p < techDesc.Passes; ++p)
	//{
	//	ID3DX11EffectPass* pass = fx->SkyTech->GetPassByIndex(p);

	//	pass->Apply(0, dc);

	//	dc->DrawIndexed(mIndexCount, 0, 0);
	//}
}

Sky::Sky( ID3D11Device* device, const std::string& cubemapFilename, float sphereRadius )
{
	// Create SkySphere
	Factory_Geometry::MeshData sphere;
	Factory_Geometry::instance()->createSphere(sphereRadius, 30, 30, sphere);
	std::vector<Vector3> position_list = sphere.positionList();
	std::vector<unsigned int> index_list = sphere.indexList();

	// Create vertex buffer
	m_vertexBuffer = new Buffer();
	HR(m_vertexBuffer->init(Buffer::VERTEX_BUFFER, sizeof(Vector3), position_list.size(),  &position_list, m_device));
	m_vertexBuffer->setDeviceContextBuffer(m_context);

	// Create index buffer
	m_indexBuffer = new Buffer();
	HR(m_indexBuffer->init(Buffer::INDEX_BUFFER, sizeof(unsigned int), index_list.size(), &index_list, m_device));
	m_indexBuffer->setDeviceContextBuffer(m_context);

	// Create constant buffer
	m_WVPBuffer = new Buffer();
	HR(m_WVPBuffer->init(Buffer::VS_CONSTANT_BUFFER, sizeof(float), 16, &m_cbuffer, m_device));
	m_WVPBuffer->setDeviceContextBuffer(m_context);


	// Load texture
	std::string texPath = "";
	std::string texFileName = "";
	std::string fullPath = texPath + texFileName;
	HRESULT hr = createTexFromFile(
		device,
		fullPath,
		&m_rv_cubeMap);

	if(FAILED(hr))
	{
		DEBUGPRINT("ManagementTex::handleTexDesc Could not load texture: " + texFileName);
	}



	//CreateTextureFromDDS(
	//createTextureFromFile(dev,        // the Direct3D device
	//	L"Wood.png",    // load Wood.png in the local folder
	//	nullptr,           // no additional information
	//	nullptr,           // no multi threading
	//	&pTexture,      // address of the shader-resource-view
	//	nullptr);          // no multithreading
}
