#ifndef ITOOL_TRANSFORMATION_H
#define ITOOL_TRANSFORMATION_H

#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <Core/Entity.h>
#include <Core/Camera.h>

struct D3D11_VIEWPORT;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11DepthStencilView;

class ITool_Transformation
{
private:
public:
	virtual bool tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView) = 0;
	virtual void update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint) = 0;
	virtual void unselect() = 0;
	virtual XMFLOAT4X4 getWorld_logical() = 0;
	virtual XMFLOAT4X4 getWorld_visual() = 0;
	virtual void setScale(float &scale) = 0;
	virtual bool getIsSelected() = 0;
	virtual void setActiveObject(int entityId) = 0;
	virtual int getActiveObject() = 0;
	virtual void init(ID3D11Device *device, ID3D11DeviceContext *deviceContext) = 0;
	virtual void draw(XMMATRIX &camView, XMMATRIX &camProj, ID3D11DepthStencilView *depthStencilView) = 0;
};

#endif