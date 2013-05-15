#pragma once

typedef long HRESULT;

struct ID3D11Device;
struct ID3D11ShaderResourceView;

#include <string>


/**
Creates an ID3D11ShaderResourceView-type object.
\param device	Handle to D3D-device.
\param file		File to load.
\param srv		inout-var to which the Shader Resource View will be loaded.
*/
void createTextureFromFile( ID3D11Device* device, std::string file, ID3D11ShaderResourceView**	srv);

