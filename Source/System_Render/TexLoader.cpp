#include "stdafx.h"
#include "TexLoader.h"

#include <Core/Converter.h>
#include "DDSTextureLoader.h"



HRESULT createTexFromFile( ID3D11Device* device, std::string file, ID3D11ShaderResourceView** srv )
{
	wchar_t* wstr = Converter::StrlToWstr(file);
	HRESULT hr = DirectX::CreateDDSTextureFromFile(
		device,
		wstr,
		nullptr,
		srv);

	delete[] wstr;
	return hr;
}
