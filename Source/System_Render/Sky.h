#pragma once

#include <vector>
#include <Core/Math.h>

#include <d3d11.h>
#include "Buffer.h"
#include "CBuffers.h"
#include "Factory_Geometry.h"
#include "Vertex.h"
#include "DDSTextureLoader.h"
#include "RenderStates.h"

class Sky
{
private:
	bool m_hasInit;
	Buffer*	m_vertexBuffer;
	Buffer*	m_indexBuffer;
	Buffer*	m_WVPBuffer;
	CBPerObject m_cbuffer;
	
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	ID3D11ShaderResourceView* m_rv_cubeMap;

	std::string m_cubemapFilename;
	float m_sphereRadius;

	ID3D11VertexShader*	m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_inputLayout;
	CBSkybox		m_cb_skybox;

	UINT mIndexCount;
public:
	Sky(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& cubemapFilename, float sphereRadius);
	~Sky();
	void init();

	ID3D11ShaderResourceView* CubeMapSRV();

	void draw();
};