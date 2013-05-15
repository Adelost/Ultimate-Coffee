#include "stdafx.h"

#include "Buffer.h"
#include <Core/EventManager.h>

// Set static variables to zero
// Static counter of constant buffers in use
unsigned int Buffer::m_numberOfVSConstantBuffers = 0;
unsigned int Buffer::m_numberOfPSConstantBuffers = 0;

Buffer::Buffer()
{
	m_buffer = nullptr;
	m_type = BufferType::UNINITIALIZED;
	m_elementSize = 0;
	m_constantBufferNumber = 0;
}

Buffer::~Buffer()
{
	if(m_type == BufferType::VS_CONSTANT_BUFFER)
		m_numberOfVSConstantBuffers--;
	else if(m_type == BufferType::PS_CONSTANT_BUFFER)
		m_numberOfPSConstantBuffers--;

	ReleaseCOM(m_buffer);
	m_buffer = nullptr;
	m_type = BufferType::UNINITIALIZED;
	m_elementSize = 0;
	m_constantBufferNumber = 0;
}

HRESULT Buffer::init(BufferType p_type, unsigned int p_elementSize, unsigned int p_count, const void* p_data, ID3D11Device* p_device)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));

	switch(p_type)
	{
		case BufferType::VERTEX_BUFFER:
		{
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			break;
		}
		case BufferType::INDEX_BUFFER:
		{
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			break;
		}
		case BufferType::VS_CONSTANT_BUFFER:
		{
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			m_constantBufferNumber = m_numberOfVSConstantBuffers;
			m_numberOfVSConstantBuffers++;
			break;
		}
		case BufferType::PS_CONSTANT_BUFFER:
		{
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			m_constantBufferNumber = m_numberOfPSConstantBuffers;
			m_numberOfPSConstantBuffers++;
			break;
		}
		default:
		{
			//MESSAGEBOX("Error: Invalid buffer type");
			return E_INVALIDARG;
			break;
		}
	}
	bufferDesc.ByteWidth = p_elementSize * p_count;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));
	data.pSysMem = p_data;

	HR(p_device->CreateBuffer(&bufferDesc, &data, &m_buffer));

	if(FAILED(hr))
	{
		ReleaseCOM(m_buffer);
		m_buffer = nullptr;
	}

	m_type = p_type;
	m_elementSize = p_elementSize;
	m_count = p_count;

	return S_OK;
}

void Buffer::setDeviceContextBuffer(ID3D11DeviceContext* p_deviceContext)
{
	switch(m_type)
	{
	case BufferType::VERTEX_BUFFER:
		{
			UINT stride = m_elementSize;
			UINT offset = 0;
			p_deviceContext->IASetVertexBuffers(0, 1, &m_buffer, &stride, &offset);
		}
		break;
	case BufferType::INDEX_BUFFER:
		{
			p_deviceContext->IASetIndexBuffer(m_buffer, DXGI_FORMAT_R32_UINT, 0);
		}
		break;
	case BufferType::VS_CONSTANT_BUFFER:
		{
			p_deviceContext->VSSetConstantBuffers(m_constantBufferNumber, 1, &m_buffer);
		}
		break;
	case BufferType::PS_CONSTANT_BUFFER:
		{
			p_deviceContext->PSSetConstantBuffers(m_constantBufferNumber, 1, &m_buffer);
		}
		break;
	case BufferType::UNINITIALIZED:
		{
			//MESSAGEBOX("Tried to set uninitialized buffer.");	
		}
		break;
	default:
		{
			//MESSAGEBOX("Failed to set buffer.");
		}
		break;
	}
}

ID3D11Buffer* Buffer::getBuffer()
{
	return m_buffer;
}