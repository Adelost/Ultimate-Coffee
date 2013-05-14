#pragma once

#include <vector>
#include <Core/Math.h>

#include <d3d11.h>
#include "Buffer.h"
#include "CBuffers.h"
#include "Factory_Geometry.h"
#include "Vertex.h"
#include "DDSTextureLoader.h"

class Sky
{
private:
	Buffer*	m_vertexBuffer;
	Buffer*	m_indexBuffer;
	Buffer*	m_WVPBuffer;
	CBPerObject m_cbuffer;
	

	float height_scale;
	float height_offset;

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	ID3D11ShaderResourceView* m_rv_cubeMap;

	UINT mIndexCount;
public:
	Sky(ID3D11Device* device, const std::string& cubemapFilename, float sphereRadius);
	~Sky();

	ID3D11ShaderResourceView* CubeMapSRV();

	void draw(ID3D11DeviceContext* dc);
};