#pragma once

#include <Core/IObserver.h>

class ITool_Transformation;
class Tool_Selection;
class Tool_Scaling;
class Tool_Rotation;
class Tool_Translation;

struct ID3D11DepthStencilView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct D3D11_VIEWPORT;

class Manager_3DTools : public IObserver
{
private:
	ITool_Transformation* currentlyChosenTransformTool;
	Tool_Selection* m_theSelectionTool;
	Tool_Scaling* m_theScalingTool;
	Tool_Rotation* m_theRotationTool;
	Tool_Translation* m_theTranslationTool;

	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11DeviceContext *m_deviceContext;

	D3D11_VIEWPORT *m_viewPort;

public:
	Manager_3DTools(ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext, ID3D11DepthStencilView* p_depthStencilView, D3D11_VIEWPORT *m_viewPort);

	~Manager_3DTools();

	void update();
	void draw(ID3D11DepthStencilView* p_depthStencilView);
	void onEvent(Event* p_event);
};