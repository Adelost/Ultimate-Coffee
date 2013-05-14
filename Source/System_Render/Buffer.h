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

		UNINITIALIZED,
	};

	Buffer();
	~Buffer();

	HRESULT init(BufferType p_type, unsigned int p_elementSize, unsigned int p_count, const void* p_data, ID3D11Device* p_device);
	void setDeviceContextBuffer(ID3D11DeviceContext* p_deviceContext);
	ID3D11Buffer* getBuffer();

private:
	ID3D11Buffer* m_buffer;
	BufferType m_type;
	unsigned int m_count;
	unsigned int m_elementSize;

	// Make a subclass or a manager?

	// Constant buffers
	static unsigned int m_numberOfVSConstantBuffers;
	static unsigned int m_numberOfPSConstantBuffers;
	unsigned int m_constantBufferNumber;

public:
	unsigned int count()
	{
		return m_count;
	}
};