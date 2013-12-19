#include "stdafx.h"
#include "OculusManager.h"
#include <Windows.h>
#include "OVR.h"
#include "Util/Util_Render_Stereo.h"
using namespace OVR;
using namespace OVR::Util::Render;


static DeviceManager*	pManager;
static SensorDevice*	pSensor;
static HMDDevice*		pHMD;
static SensorFusion*   fusion;
static HMDInfo			hmd_info;
static StereoConfig	stereoConfig;
class OVRMessageHandler;
static OVRMessageHandler* messageHandler;

struct RendererParams
{
	int  multisample;
	int  fullscreen;
	String monitorName;
	long   displayId;

	RendererParams(int ms = 1)
	{
		multisample = ms;
		multisample = 0;
	}

	bool isDisplaySet()
	{
		bool out = displayId > 0;
		return out;
	}
};

static RendererParams renderParams;

class OVRMessageHandler : public OVR::MessageHandler
{
public:
	void OnMessage(const OVR::Message& msg)
	{
		if (msg.Type == Message_DeviceAdded && msg.pDevice == pManager)
		{
			LogText("DeviceManager reported device added.\n");
		}
		else if (msg.Type == Message_DeviceRemoved && msg.pDevice == pManager)
		{
			LogText("DeviceManager reported device removed.\n");
		}
		else if (msg.Type == Message_DeviceAdded && msg.pDevice == pSensor)
		{
			LogText("Sensor reported device added.\n");
		}
		else if (msg.Type == Message_DeviceRemoved && msg.pDevice == pSensor)
		{
			LogText("Sensor reported device removed.\n");
		}
	}
};

OculusManager::OculusManager()
{
	// Create device manager
	OVR::System::Init(Log::ConfigureDefaultLog(LogMask_All));
	pManager = DeviceManager::Create();

	// Handle messages
	messageHandler = new OVRMessageHandler();
	pManager->SetMessageHandler(messageHandler);

	// Try to detect Oculus device
	pHMD = pManager->EnumerateDevices<HMDDevice>().CreateDevice();
	if(pHMD)
	{
		pSensor = pHMD->GetSensor();

		if(pHMD->GetDeviceInfo(&hmd_info))
		{            
			renderParams.monitorName = hmd_info.DisplayDeviceName;
			renderParams.displayId = hmd_info.DisplayId;
			stereoConfig.SetHMDInfo(hmd_info);
		}
	}
	// Continue anyway, for debugging purpose
	else
	{
		pSensor = pManager->EnumerateDevices<SensorDevice>().CreateDevice();
	}

	// Detect problem
	const char* detectionMessage;
	if (!pHMD && !pSensor)
		detectionMessage = "Oculus Rift not detected.";
	else if (!pHMD)
		detectionMessage = "Oculus Sensor detected; HMD Display not detected.";
	else if (!pSensor)
		detectionMessage = "Oculus HMD Display detected; Sensor not detected.";
	else if (hmd_info.DisplayDeviceName[0] == '\0')
		detectionMessage = "Oculus Sensor detected; HMD display EDID not detected.";
	else
		detectionMessage = nullptr;

	// Attach SensorFusion to enable us to listen to the device
	if(pSensor)
	{
		fusion = new SensorFusion();
		fusion->AttachToSensor(pSensor);
		fusion->SetDelegateMessageHandler(messageHandler);
		fusion->SetPredictionEnabled(true);
	}

	// Update other parameters
	update();
}

Quaternion OculusManager::GetOrientation()
{
	getProjectionLeft();

	Quatf q;
	if(fusion)
	{
		q =	fusion->GetPredictedOrientation();
		q.y *= -1;
		q.x *= -1;
	}
	Quaternion out(q.x, q.y, q.z, q.w);
	return out;



// 	Quatf qf1;
// 	if(true)
// 	{
// 		qf1 =fusion->GetPredictedOrientation();
// 	}
// 	else
// 	{
// 		qf1 = fusion->GetOrientation();
// 	}
// 
// 	Matrix4f matrix = qf1.operator OVR::Matrix4f();
// 
// 	Matrix m1 = matrixRightHandedToLeft(&matrix);
// 
// 	Quaternion q2(0.0f,0.0f,0.0f,1.0f);
// 	Quaternion q3(0.0f,0.0f,0.0f,1.0f);
// 
// 	q2 = Quaternion::CreateFromRotationMatrix(m1);
// 
// 	float pi = 3.1415926535;
// 	q3 = Quaternion::CreateFromYawPitchRoll(0.0f, 180, 0.0f);
// 
// 	q2 = q2 * q3;
// 
// 	Quaternion q1;
// 
// 	q1.x = q2.x;
// 	q1.y = q2.y;
// 	q1.z = q2.z;
// 	q1.w = q2.w;
// 
// 
// 	return q1;
}

OculusManager::~OculusManager()
{
	// Clean up
	if(pSensor)
	{
		pSensor->Release();
		pSensor = NULL;
	}
	if(pHMD)
	{
		pHMD->Release();
		pHMD = NULL;
	}
	if(pManager)
	{
		pManager->Release();
		pManager = NULL;
	}
	if(fusion)
		delete fusion;
	OVR::System::Destroy();

	// 'messageHandler' is registered by ORV and 
	// cannot be deleted until ORV is released.
	if(messageHandler)
		delete messageHandler;
}

void OculusManager::getLeftEye()
{

}

void OculusManager::update()
{
	// Configure stereo mode
	WindowWidth = SETTINGS()->windowSize.x;
	WindowHeight = SETTINGS()->windowSize.y;

	stereoConfig.SetFullViewport(Viewport(0, 0, WindowWidth, WindowHeight));
	if(SETTINGS()->bOculusRift)
	{
		stereoConfig.SetStereoMode(Stereo_LeftRight_Multipass);
	}
	else
	{
		stereoConfig.SetStereoMode(Stereo_None);
	}


	// Configure proper Distortion Fit.
	// For 7" screen, fit to touch left side of the view, leaving a bit of invisible
	// screen on the top (saves on rendering cost).
	// For smaller screens (5.5"), fit to the top.
	if (hmd_info.HScreenSize > 0.0f)
	{
		if (hmd_info.HScreenSize > 0.140f) // 7"
			stereoConfig.SetDistortionFitPointVP(-1.0f, 0.0f);
		else
			stereoConfig.SetDistortionFitPointVP(0.0f, 1.0f);
	}

	DistortionScale = stereoConfig.GetDistortionScale();
	YFOVRadians = stereoConfig.GetYFOVRadians();
	Aspect = stereoConfig.GetAspect();
	//DistortionScale = 1.0f;

	// 
	// 	// Compute Vertical FOV based on distance.
	// 	float halfScreenDistance = (hmd_info.VScreenSize / 2);
	// 	float yfov = 2.0f * atan(halfScreenDistance/hmd_info.EyeToScreenDistance);
	// 	// Post-projection viewport coordinates range from (-1.0, 1.0), with the
	// 	// center of the left viewport falling at (1/4) of horizontal screen size.
	// 	// We need to shift this projection center to match with the lens center.
	// 	// We compute this shift in physical units (meters) to correct
	// 	// for different screen sizes and then rescale to viewport coordinates.
	// 	float viewCenter = hmd_info.HScreenSize * 0.25f;
	// 	float eyeProjectionShift = viewCenter - hmd_info.LensSeparationDistance * 0.5f;
	// 	float projectionCenterOffset = 4.0f * eyeProjectionShift / hmd_info.HScreenSize;
	// 	// Projection matrix for the "center eye", which the left/right matrices are based on.
	// 	Matrix4f projCenter = Matrix4f::PerspectiveLH(yfov, Aspect, 0.3f, 1000.0f);
	// 	Matrix4f projLeft = Matrix4f::Translation(projectionCenterOffset, 0, 0) * projCenter;
	// 	Matrix4f projRight = Matrix4f::Translation(-projectionCenterOffset, 0, 0) * projCenter;
	// 	// View transformation translation in world units.
	// 	float halfIPD = hmd_info.InterpupillaryDistance * 0.5f;
	// 	Matrix4f viewLeft = Matrix4f::Translation(halfIPD, 0, 0) * viewCenter;
	// 	Matrix4f viewRight= Matrix4f::Translation(-halfIPD, 0, 0) * viewCenter;

	updateVars( Enum::Direction::Left);
}

float OculusManager::getDistortionScale()
{
	return stereoConfig.GetDistortionScale();
}

Matrix OculusManager::getProjectionLeft()
{
	StereoEyeParams leftEye = stereoConfig.GetEyeRenderParams(StereoEye_Left);
	StereoEyeParams rightEye = stereoConfig.GetEyeRenderParams(StereoEye_Right);

	// Left eye rendering parameters
	Viewport leftVP = leftEye.VP;
	Matrix4f leftProjection = leftEye.Projection;
	Matrix4f leftViewAdjust = leftEye.ViewAdjust;

	Matrix m = *(Matrix*)&leftProjection;
	return m;
}

void OculusManager::updateVars( Enum::Direction direction )
{
	if(SETTINGS()->bOculusRift == false)
		return;

	StereoEyeParams params;
	if( direction  == Enum::Direction::Left)
		params = stereoConfig.GetEyeRenderParams(StereoEye_Left);
	else
		params = stereoConfig.GetEyeRenderParams(StereoEye_Right);

	Viewport VP = params.VP;
	DistortionConfig Distortion = *params.pDistortion;

	float w = float(VP.w) / float(WindowWidth),
		h = float(VP.h) / float(WindowHeight),
		x = float(VP.x) / float(WindowWidth),
		y = float(VP.y) / float(WindowHeight);

	float as = float(VP.w) / float(VP.h);

	// HACK: XCenterOffset needs to be inverted for right eye, this is done
	// automatically in the samples, but not here for some reason, wasted 
	// 5 hours of my life finding it
	float xCenterOffset = Distortion.XCenterOffset;
	if( direction  == Enum::Direction::Right)
		xCenterOffset *= -1;
	LensCenter = SimpleMath::Vector2(x + (w + xCenterOffset * 0.5f)*0.5f, y + h*0.5f);
	ScreenCenter = SimpleMath::Vector2(x + w*0.5f, y + h*0.5f);
	float scaleFactor = 1.0f / Distortion.Scale;

	Scale = SimpleMath::Vector2( (w/2) * scaleFactor, (h/2) * scaleFactor * as);
	ScaleIn = SimpleMath::Vector2( (2/w), (2/h) / as);
	HmdWarpParam = SimpleMath::Vector4(Distortion.K[0], Distortion.K[1], Distortion.K[2], Distortion.K[3]);

	// Get view adjustments
	ViewAdjust = reinterpret_cast<Matrix*>(&params.ViewAdjust)->Transpose();
	//ViewAdjust._41 *= 100;
	Projection = reinterpret_cast<Matrix*>(&params.Projection)->Transpose();
}

DirectX::SimpleMath::Matrix OculusManager::matrixRightHandedToLeft( Matrix4f* matrix )
{

	//-------------------------------------------------------------
	Matrix m1,m2;
	//-------------------------------------------------------------
	//convert row major to column major
	m2.m[0][0] = matrix->M[0][0];
	m2.m[1][0] = matrix->M[0][1];
	m2.m[2][0] = matrix->M[0][2];
	m2.m[3][0] = matrix->M[0][3];

	m2.m[0][1] = matrix->M[1][0];
	m2.m[1][1] = matrix->M[1][1];
	m2.m[2][1] = matrix->M[1][2];
	m2.m[3][1] = matrix->M[1][3];

	m2.m[0][2] = matrix->M[2][0];
	m2.m[1][2] = matrix->M[2][1];
	m2.m[2][2] = matrix->M[2][2];
	m2.m[3][2] = matrix->M[2][3];

	m2.m[0][3] = matrix->M[3][0];
	m2.m[1][3] = matrix->M[3][1];
	m2.m[2][3] = matrix->M[3][2];
	m2.m[3][3] = matrix->M[3][3];
	//-------------------------------------------------------------
	//convert right handed to left handed as row major 
	m1.m[0][0] = -m2.m[0][0];
	m1.m[0][1] = m2.m[0][1];
	m1.m[0][2] = m2.m[0][2];
	m1.m[0][3] = 0.0f;

	m1.m[1][0] = -m2.m[1][0];
	m1.m[1][1] = m2.m[1][1];
	m1.m[1][2] = m2.m[1][2];
	m1.m[1][3] = 0.0f;

	m1.m[2][0] = m2.m[2][0];
	m1.m[2][1] = -m2.m[2][1];
	m1.m[2][2] = -m2.m[2][2];
	m1.m[2][3] = 0.0f;

	m1.m[3][0] = 0.0f;
	m1.m[3][1] = 0.0f;
	m1.m[3][2] = 0.0f;
	m1.m[3][3] = 1.0f;

	return m1;
}
