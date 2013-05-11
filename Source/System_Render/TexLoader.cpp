#include "stdafx.h"
#include "TexLoader.h"

#include <Core/Converter.h>
#include "DDSTextureLoader.h"

//
//HRESULT TexLoader::createTexFromFile(
//		ID3D11Device*				device,
//		std::string					file,
//		ID3D11ShaderResourceView**	srv)
//{
//	HRESULT hr = S_OK;
//
//	wchar_t* wstr = Converter::StrlToWstr(file);
//	hr = DirectX::CreateDDSTextureFromFile(
//		device,
//		wstr,
//		nullptr,
//		srv);
//
//	delete[] wstr;
//	return hr;
//}