#pragma once

#include <Core/IObserver.h>
class ITool_Transformation;
class Tool_Translation;
struct ID3D11DepthStencilView;
struct ID3D11Device;
struct ID3D11DeviceContext;

class Manager_3DTools : public IObserver
{
private:
	ITool_Transformation* currentlyChosenTransformTool;
	Tool_Translation* m_theTranslationTool;

	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11DeviceContext *m_deviceContext;

public:
	Manager_3DTools(ID3D11Device* p_device, ID3D11DeviceContext* p_deviceContext, ID3D11DepthStencilView* p_depthStencilView);

	~Manager_3DTools();

	void update();
	void draw(ID3D11DepthStencilView* p_depthStencilView);
	void onEvent(IEvent* p_event);
};