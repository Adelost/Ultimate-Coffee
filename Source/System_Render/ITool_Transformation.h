#ifndef ITOOL_TRANSFORMATION_H
#define ITOOL_TRANSFORMATION_H

#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "IObject.h"



class ITool_Transformation
{
private:
public:
	virtual bool tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera) = 0;
	virtual void update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint) = 0;
	virtual void unselect() = 0;
	virtual XMFLOAT4X4 getWorld_logical() = 0;
	virtual XMFLOAT4X4 getWorld_visual() = 0;
	virtual void setScale(float &scale) = 0;
	virtual bool getIsSelected() = 0;
	virtual void setActiveObject(IObject *object) = 0;
	virtual IObject *getActiveObject() = 0;
	virtual void init(ID3D11Device *device, ID3D11DeviceContext *deviceContext) = 0;
	virtual void draw() = 0;
};

#endif