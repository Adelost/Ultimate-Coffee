#ifndef TOOL_SCALING_H
#define TOOL_SCALING_H

#include "ITool_Transformation.h"
#include "Handle_ScalingAxis.h"
#include "Handle_ScalingPlane.h"
#include "Handle_OmniDimensionalScaling.h"

#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11PixelShader;
struct ID3D11VertexShader;

class Tool_Scaling : public ITool_Transformation
{
private:
	struct ConstantBuffer2
	{
		XMMATRIX WVP;
	};

	ID3D11PixelShader*			m_pixelShader;
	ID3D11VertexShader*			m_vertexShader;

	ID3D11Buffer *m_ToolPS_PerFrame_Buffer;
	ID3D11Buffer *m_ColorSchemeIdBuffer;
	ID3D11Buffer *m_WVPBuffer;
	ID3D11InputLayout*			m_inputLayout;

	//

	ID3D11Device *md3dDevice;
	ID3D11DeviceContext *md3dImmediateContext;

	// Test mesh for translation tool.
	ID3D11Buffer* mMeshTransToolVB;

	// Debug:
	ID3D11Buffer* mMeshTransTool_xAxisBox_VB;
	ID3D11Buffer* mMeshTransTool_yAxisBox_VB;
	ID3D11Buffer* mMeshTransTool_zAxisBox_VB;
	ID3D11Buffer* mMeshTransTool_xAxisBox2_VB;
	ID3D11Buffer* mMeshTransTool_yAxisBox2_VB;
	ID3D11Buffer* mMeshTransTool_zAxisBox2_VB;
	ID3D11Buffer* mMeshTransTool_omniAxisBox_VB;
	ID3D11Buffer* mMeshTransTool_axisBox_IB;

	ID3D11Buffer* mMeshTransTool_yzPlane_VB;
	ID3D11Buffer* mMeshTransTool_zxPlane_VB;
	ID3D11Buffer* mMeshTransTool_xyPlane_VB;
	ID3D11Buffer* mMeshTransTool_yzPlane2_VB;
	ID3D11Buffer* mMeshTransTool_zxPlane2_VB;
	ID3D11Buffer* mMeshTransTool_xyPlane2_VB;
	ID3D11Buffer* mMeshTransTool_viewPlane_VB;

	//ID3D11Buffer* mMeshTransTool_yzTriangleListRectangle_VB;
	//ID3D11Buffer* mMeshTransTool_zxTriangleListRectangle_VB;
	//ID3D11Buffer* mMeshTransTool_xyTriangleListRectangle_VB;
	//ID3D11Buffer* mMeshTransTool_viewPlaneTriangleListRectangle_VB;

	ID3D11Buffer* mMeshTransTool_obscuringRectangles_VB;

	////////////////////////////

	XMFLOAT3 position;
	XMFLOAT4X4 world;
	XMFLOAT4X4 world2;
	float scale;

	IHandle *currentlySelectedHandle;

	Handle_OmniDimensionalScaling *omniScalingAxisHandle;

	Handle_ScalingAxis	*xScalingAxisHandle,
						*yScalingAxisHandle,
						*zScalingAxisHandle,
						*xScalingAxisHandle2,
						*yScalingAxisHandle2,
						*zScalingAxisHandle2;

	Handle_ScalingPlane *xyScalingPlane,
						*yzScalingPlane,
						*zxScalingPlane,
						*xyScalingPlane2,
						*yzScalingPlane2,
						*zxScalingPlane2;
	
	//Handle_CamViewPlane *camViewPlane;
	
	Handle_ScalingAxis *currentlySelectedAxis;
	Handle_ScalingPlane *currentlySelectedPlane;

	bool isSelected;
	bool isVisible;

	EntityPointer activeEntity;

	bool relateToActiveObjectWorld;

	XMFLOAT4X4 originalWorldOfActiveObject;

	XMFLOAT4X4 world_viewPlaneTranslationControl_logical;
	XMFLOAT4X4 world_viewPlaneTranslationControl_visual;

	bool shouldFlipMouseCursor;

	//ID3D11Buffer *xyPlaneBuffer;
	//ID3D11Buffer *yzPlaneBuffer;
	//ID3D11Buffer *zxPlaneBuffer;

	bool transformInEntityLocalSpace;

	std::vector<XMFLOAT3> originalScalesOfSelectedEntities;

	ID3D11BlendState *m_blendState;



public:
	Tool_Scaling(/*HWND windowHandle*/);
	~Tool_Scaling();
	void setIsVisible(bool &isVisible);

	/* Called for an instance of picking, possibly resulting in the tool being selected. */
	bool tryForSelection(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, POINT &mouseCursorPoint);

	/* Called to see if the mouse cursor is hovering over the tool, and what part of it, if any. */
	void tryForHover(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView);

	/* Called to bind the translatable object to the tool, so its translation can be modified. */
	void setActiveObject(int entityId);

	/* Transform all controls to the local coord. sys. of the active object. */
	void setRelateToActiveObjectWorld(bool relateToActiveObjectWorld);

	/* Called to see if the translation tool is (still) active. */
	bool getIsSelected();

	/* Called to set the entity at whose pivot the tool is to be displayed, when a selection of one or more entities has been made. */
	void setEntityAtWhosePivotTheToolIsToBeDisplayed(int entityId);

	/* Called to send updated parameters to the translation tool, if it is still active. */
	void update(MyRectangle &selectionRectangle, XMVECTOR &rayOrigin, XMVECTOR &rayDir, XMMATRIX &camView, XMMATRIX &camProj, D3D11_VIEWPORT &theViewport, POINT &mouseCursorPoint);

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

	void setShouldFlipMouseCursor(bool shouldFlipMouseCursor);

	float getScale();

	XMFLOAT4X4 getWorld_visual();

	//

	void updateViewPlaneTranslationControlWorld(XMFLOAT3 &camViewVector, XMFLOAT3 &camUpVector);

	XMFLOAT4X4 getWorld_viewPlaneTranslationControl_logical();

	XMFLOAT4X4 getWorld_viewPlaneTranslationControl_visual();

	XMFLOAT4X4 getWorld_visual_objectRelative();

	EntityPointer getActiveObject();
	
	void init(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
	void draw(XMMATRIX &camView, XMMATRIX &camProj, ID3D11DepthStencilView *depthStencilView);
};

#endif