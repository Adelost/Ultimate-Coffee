#pragma once

#include <Core/Util.h>
#include <Core/Data.h>
#include <Core/DataMapper.h>
#include <Core/Data_Camera.h>
#include <Core/Entity.h>
#include <Core/Enums.h>
#include <Core/EventManager.h>
#include <Core/Events.h>
#include <Core/Factory_Entity.h>
#include <Core/Settings.h>
#include <Core/World.h>
#include <Core/Converter.h>

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#include <assert.h>


//
// Macros
//

// DirectX error checking macro
//#if defined(DEBUG) | defined(_DEBUG)
//	#ifndef HR
//	#define HR(x)												\
//	{															\
///*		HRESULT hr = (x);										\
//		if(FAILED(hr))											\
//		{														\
//			DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true);	\
//		}	*/													\
//	}
//	#endif
//#else
#ifndef HR
#define HR(x) (x)
#endif
//#endif 

// Convenience macros
#define ReleaseCOM(x){if(x){x->Release(); x = 0;}}
#define SafeDelete(x) { delete x; x = 0; }



//namespace Colors
//{
//	XMGLOBALCONST XMVECTORF32 White     = {1.0f, 1.0f, 1.0f, 1.0f};
//	XMGLOBALCONST XMVECTORF32 Black     = {0.0f, 0.0f, 0.0f, 1.0f};
//	XMGLOBALCONST XMVECTORF32 Red       = {1.0f, 0.0f, 0.0f, 1.0f};
//	XMGLOBALCONST XMVECTORF32 Green     = {0.0f, 1.0f, 0.0f, 1.0f};
//	XMGLOBALCONST XMVECTORF32 Blue      = {0.0f, 0.0f, 1.0f, 1.0f};
//	XMGLOBALCONST XMVECTORF32 Yellow    = {1.0f, 1.0f, 0.0f, 1.0f};
//	XMGLOBALCONST XMVECTORF32 Cyan      = {0.0f, 1.0f, 1.0f, 1.0f};
//	XMGLOBALCONST XMVECTORF32 Magenta   = {1.0f, 0.0f, 1.0f, 1.0f};
//
//	XMGLOBALCONST XMVECTORF32 Silver    = {0.75f, 0.75f, 0.75f, 1.0f};
//	XMGLOBALCONST XMVECTORF32 LightSteelBlue = {0.69f, 0.77f, 0.87f, 1.0f};
//	XMGLOBALCONST XMVECTORF32 DeepBlue      = {0.14f, 0.42f, 0.56f, 1.0f};
//}