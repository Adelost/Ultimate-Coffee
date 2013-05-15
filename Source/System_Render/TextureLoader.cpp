#include "stdafx.h"
#include "TextureLoader.h"

#include <Core/Converter.h>
#include "DDSTextureLoader.h"



void createTextureFromFile( ID3D11Device* device, std::string file, ID3D11ShaderResourceView** srv )
{
	wchar_t* wstr = Converter::StrlToWstr(file);
	HRESULT hr = DirectX::CreateDDSTextureFromFile(
		device,
		wstr,
		nullptr,
		srv);

	delete[] wstr;

	if(FAILED(hr))
	{
		DEBUGPRINT("ManagementTex::handleTexDesc Could not load texture: " + file);
	}
}
