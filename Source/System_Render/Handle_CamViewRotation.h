#ifndef HANDLE_CAMVIEWROTATION_H
#define HANDLE_CAMVIEWROTATION_H

#include <Core/Math.h>

#include "IHandle.h"
#include "xnacollision.h"

#include "Handle_RotationPlane.h"

class CamViewRotation : public IHandle
{
private:
	RotationPlane *camViewRotationPlane;
public:
	CamViewRotation();
	~CamViewRotation();

};

#endif