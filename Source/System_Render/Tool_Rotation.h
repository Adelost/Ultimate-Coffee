#ifndef TOOL_ROTATION_H
#define TOOL_ROTATION_H

#include "ITool_Transformation.h"
#include "Handle_RotationSphere.h"

#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"

#include "GeometryGenerator.h"

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11PixelShader;
struct ID3D11VertexShader;

class Tool_Rotation : public ITool_Transformation
{
private:
	struct ConstantBuffer2
	{
		XMMATRIX WVP;
	};

	ID3D11PixelShader*			m_pixelShader;
	ID3D11VertexShader*			m_vertexShader;

	ID3D11Buffer *m_WVPBuffer;
	ID3D11InputLayout*			m_inputLayout;

	//

	ID3D11Device *md3dDevice;
	ID3D11DeviceContext *md3dImmediateContext;

	ID3D11Buffer* mMeshRotTool_circle_VB;
	ID3D11Buffer* mMeshRotTool_Xcircle_VB;
	ID3D11Buffer* mMeshRotTool_Ycircle_VB;
	ID3D11Buffer* mMeshRotTool_Zcircle_VB;

	///////////////////////////////

	XMFLOAT3 position;
	XMFLOAT4X4 world;
	XMFLOAT4X4 world2;
	float scale;

	bool isSelected;
	bool isVisible;

	int activeEntityId;

	bool relateToActiveObjectWorld;

	XMFLOAT4X4 originalWorldOfActiveObject;
	XMFLOAT4 originalRotationQuatOfActiveObject;

	XMFLOAT4X4 world_viewPlaneTranslationControl_logical;
	XMFLOAT4X4 world_viewPlaneTranslationControl_visual;

	IHandle *currentlySelectedHandle;
	Handle_RotationSphere *omniRotateSphereHandle;

public:
	Tool_Rotation(/*HWND windowHandle*/);
	~Tool_Rotation();
	void setIsVisible(bool &isVisible);

	/* Called for an instance of picking, possibly resulting in the tool being selected. */
	bool tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, POINT &mouseCursorPoint);

	/* Called to see if the mouse cursor is hovering over the tool, and what part of it, if any, and sets the cursor accordingly. */
	void tryForHover(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView);

	/* Called to bind the translatable object to the tool, so its translation can be modified. */
	void setActiveObject(int entityId);

	/* Called to set the entity at whose pivot the tool is to be displayed, when a selection of one or more entities has been made. */
	void setEntityAtWhosePivotTheToolIsToBeDisplayed(int entityId);

	/* Transform all controls to the local coord. sys. of the active object. */
	void setRelateToActiveObjectWorld(bool relateToActiveObjectWorld);

	/* Called to see if the translation tool is (still) active. */
	bool getIsSelected();

	/* Called to send updated parameters to the translation tool, if it is still active. */
	void update(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint);

	/* Called for current translation delta made by picking. */
	void translateObject();

	/* Called when the translation tool is unselected, which makes any hitherto made translation final. */
	void unselect();

	/* Called, amongst other times, when un-selecting, to update the translation tool's point of operation for any future selection. */
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

	int getActiveObject();
	
	void init(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
	void draw(XMMATRIX &camView, XMMATRIX &camProj, ID3D11DepthStencilView *depthStencilView);
};

#endif