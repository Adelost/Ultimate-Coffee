#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
typedef long HRESULT;

class Buffer
{
public:
	enum BufferType
	{
		VERTEX_BUFFER,
		INDEX_BUFFER,
		VS_CONSTANT_BUFFER,
		PS_CONSTANT_BUFFER,
		CONSTANT_BUFFER,
		UNINITIALIZED,
	};

	Buffer();
	~Buffer();

	HRESULT init(BufferType p_type, unsigned int p_elementSize, unsigned int p_count, const void* p_data, ID3D11Device* p_device);
	void setDeviceContextBuffer(ID3D11DeviceContext* p_deviceContext, unsigned int p_constantBufferIndex = 0);
	ID3D11Buffer* getBuffer();

private:
	ID3D11Buffer* m_buffer;
	BufferType m_type;
	unsigned int m_count;
	unsigned int m_elementSize;

public:
	unsigned int count()
	{
		return m_count;
	}
};