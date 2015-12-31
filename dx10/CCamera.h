#ifndef CCAMERA_H_
#define CCAMERA_H_

// Includes ---------------------------------------
#include "Globals.h"
#include "CTimer.h"
// --------------------------------------------

class CCamera
{
public:
	// Constructors and destructors for this class
	CCamera();
	CCamera(const CCamera&);
	~CCamera();
	// Public members of this class
	void InitCamera(int, int, float, float);
	void RotateCameraIncrement(float x, float y, float z);
	void MoveCameraTo(float x, float y, float z);
	void MoveCameraIncrement(float x, float y, float z);

	inline D3DXVECTOR3 getCamPos()
	{
		return D3DXVECTOR3(posX, posY, posZ);
	}
	inline D3DXVECTOR3 getEye()
	{
		return D3DXVECTOR3(eyeX, eyeY, eyeZ);
	}

	D3DXMATRIX		worldMatrix;
	D3DXMATRIX		worldViewProjectionMatrix;
	D3DXMATRIX		viewMatrix;
	D3DXMATRIX		projectionMatrix;

private:
	// Private members of this class

	float posX, posY, posZ;
	float rotX, rotY, rotZ;
	float eyeX, eyeY, eyeZ;
};

#endif