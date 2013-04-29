#ifndef TOOL_ROTATION_H
#define TOOL_ROTATION_H

#include "ITool_Transformation.h"
#include "Object_Basic.h"
#include "Renderable_Basic.h"
#include "Handle_RotationSphere.h"

class Tool_Rotation : public ITool_Transformation
{
private:
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

	IObject *activeObject;

	bool relateToActiveObjectWorld;

	XMFLOAT4X4 originalWorldOfActiveObject;
	XMFLOAT4 originalRotationQuatOfActiveObject;

	XMFLOAT4X4 world_viewPlaneTranslationControl_logical;
	XMFLOAT4X4 world_viewPlaneTranslationControl_visual;

	IHandle *currentlySelectedHandle;
	Handle_RotationSphere *omniRotateSphereHandle;

public:
	Tool_Rotation(HWND windowHandle);
	~Tool_Rotation();
	void setIsVisible(bool &isVisible);

	/* Called for an instance of picking, possibly resulting in the tool being selected. */
	bool tryForSelection(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera);

	/* Called to bind the translatable object to the tool, so its translation can be modified. */
	void setActiveObject(IObject *object);

	/* Transform all controls to the local coord. sys. of the active object. */
	void setRelateToActiveObjectWorld(bool relateToActiveObjectWorld);

	/* Called to see if the translation tool is (still) active. */
	bool getIsSelected();

	/* Called to send updated parameters to the translation tool, if it is still active. */
	void update(XMVECTOR &rayOrigin, XMVECTOR &rayDir, Camera &theCamera, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint);

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

	IObject *getActiveObject();
	
	void init(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
	void draw();
};

#endif