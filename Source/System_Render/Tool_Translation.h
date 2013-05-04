#ifndef TOOL_TRANSLATION_H
#define TOOL_TRANSLATION_H

#include "ITool_Transformation.h"
#include "Handle_TranslationAxis.h"
#include "Handle_TranslationPlane.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11PixelShader;
struct ID3D11VertexShader;

struct ConstantBuffer2
{
	Matrix WVP;
};

class Tool_Translation : public ITool_Transformation
{
private:
	ID3D11PixelShader*			m_pixelShader;
	ID3D11VertexShader*			m_vertexShader;

	ID3D11Buffer *m_WVPBuffer;
	ID3D11InputLayout*			m_inputLayout;

	//

	ID3D11Device *md3dDevice;
	ID3D11DeviceContext *md3dImmediateContext;

	// Test mesh for translation tool.
	ID3D11Buffer* mMeshTransToolVB;

	// Debug:
	ID3D11Buffer* mMeshTransTool_yzPlane_VB;
	ID3D11Buffer* mMeshTransTool_zxPlane_VB;
	ID3D11Buffer* mMeshTransTool_xyPlane_VB;
	ID3D11Buffer* mMeshTransTool_viewPlane_VB;

	////////////////////////////

	XMFLOAT3 position;
	XMFLOAT4X4 world;
	XMFLOAT4X4 world2;
	float scale;

	Handle_TranslationAxis	*xTranslationAxisHandle,
							*yTranslationAxisHandle,
							*zTranslationAxisHandle;

	Handle_TranslationPlane *xyTranslationPlane,
							*yzTranslationPlane,
							*zxTranslationPlane,
							*xyTranslationPlane2,
							*yzTranslationPlane2,
							*zxTranslationPlane2,
							*camViewTranslationPlane;
	
	Handle_TranslationAxis *currentlySelectedAxis;
	Handle_TranslationPlane *currentlySelectedPlane;

	bool isSelected;
	bool isVisible;

	int activeEntityId;

	bool relateToActiveObjectWorld;

	XMFLOAT4X4 originalWorldOfActiveObject;

	XMFLOAT4X4 world_viewPlaneTranslationControl_logical;
	XMFLOAT4X4 world_viewPlaneTranslationControl_visual;

	//ID3D11Buffer *xyPlaneBuffer;
	//ID3D11Buffer *yzPlaneBuffer;
	//ID3D11Buffer *zxPlaneBuffer;
	

public:
	Tool_Translation();
	~Tool_Translation();
	void setIsVisible(bool &isVisible);

	/* Called for an instance of picking, possibly resulting in the tool being selected. */
	bool tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView);

	/* Called to bind the translatable object to the tool, so its translation can be modified. */
	void setActiveObject(int entityId);

	/* Called to bind the translatable object to the tool, so its translation can be modified. */
	int getActiveObject();

	/* Transform all controls to the local coord. sys. of the active object. */
	void setRelateToActiveObjectWorld(bool relateToActiveObjectWorld);

	/* Called to see if the translation tool is (still) active. */
	bool getIsSelected();

	/* Called to send updated parameters to the translation tool, if it is still active. */
	void update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint);

	/* Called for current translation delta made by picking. */
	void translateObject();

	/* Called when the translation tool is unselected, which makes any hitherto made translation final. */
	void unselect();

	/* Called, amongst other times, when unselecting, to update the translation tool's point of operation for any future selection. */
	void updateWorld();

	/* */
	XMFLOAT4X4 getWorld_logical();

	/* Called to see if the visual component of the translation tool should be rendered. */
	bool getIsVisible();

	/* Called to set the scale as it is adjusted depending on camera distance to the translation tool and affects intersection tests. */
	void setScale(float &scale);

	float getScale();

	XMFLOAT4X4 getWorld_visual();

	//

	void updateViewPlaneTranslationControlWorld(XMFLOAT3 &camViewVector, XMFLOAT3 &camUpVector);

	XMFLOAT4X4 getWorld_viewPlaneTranslationControl_logical();

	XMFLOAT4X4 getWorld_viewPlaneTranslationControl_visual();

	void init(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
	void draw(XMMATRIX &camView, XMMATRIX &camProj, ID3D11DepthStencilView *depthStencilView);
};

#endif

//struct Handle_Cube
//{
//private:
//	float lastSelectHoldThenUnselectDeltaA;
//	float lastSelectHoldThenUnselectDeltaB;
//	bool isSelected;
//
//public:
//	Handle_Cube();
//	~Handle_Cube();
//	bool tryForSelection(D3DXVECTOR3 &pickingRay);
//	void unselect(D3DXVECTOR3 &pickingRay);
//};