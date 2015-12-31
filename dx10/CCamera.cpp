// Includes --------------------------------
#include "CCamera.h"
// -----------------------------------------

CCamera::CCamera()
{
	posX	 = 0;
	posY	 = 0;
	posZ	 = 0;
	rotX	 = 0;
	rotY	 = 0;
	rotZ	 = 0;
}

CCamera::CCamera(const CCamera& a)
{
} 

CCamera::~CCamera()
{
}

void CCamera::InitCamera(int w, int h, float gCamNear, float gCamDepth)
{
	D3DXVECTOR3 Eye, Up, At;
	eyeX = Eye.x = 0.0f;
	eyeY = Eye.y = 4.0f;
	eyeZ = Eye.z = -50.0f;

	At.x = 0.0f;
	At.y = 1.0f;
	At.z = 0.0f;

	Up.x = 0.0f;
	Up.y = 1.0f;
	Up.z = 0.0f;

	D3DXMatrixLookAtLH(&viewMatrix, &Eye, &At, &Up);

	D3DXMatrixPerspectiveFovLH( &projectionMatrix, static_cast<float>(D3DX_PI) / 0.25, static_cast<float>(w / h), gCamNear, gCamDepth);

	D3DXMatrixIdentity(&this->worldMatrix);

	return;
}

void CCamera::RotateCameraIncrement(float x, float y, float z)
{
	CTimer timer;

	timer.StartTicking();

	rotX	= x;
	rotY	= y;
	rotZ	= z;

	float elapsed	= static_cast<float>(timer.StopAndGetElapsed());

	D3DXMATRIX	rotate;

	D3DXMatrixIdentity(&rotate);
	
	D3DXMatrixRotationX(&rotate, static_cast<float>(D3DXToRadian(x * elapsed)));
	D3DXMatrixMultiply(&worldMatrix, &rotate, &worldMatrix);
	
	D3DXMatrixIdentity(&rotate);

	D3DXMatrixRotationY(&rotate, static_cast<float>(D3DXToRadian(y * elapsed)));
	D3DXMatrixMultiply(&worldMatrix, &rotate, &worldMatrix);

	D3DXMatrixIdentity(&rotate);

	D3DXMatrixRotationZ(&rotate, static_cast<float>(D3DXToRadian(z * elapsed)));
	D3DXMatrixMultiply(&worldMatrix, &rotate, &worldMatrix);

	return;
}

void CCamera::MoveCameraTo(float x, float y, float z)
{
	posX = x;
	posY = y;
	posZ = z;
	D3DXMatrixTranslation(&worldMatrix, x, y, z);
	return;
}

void CCamera::MoveCameraIncrement(float x, float y, float z)
{
	CTimer timer;

	timer.StartTicking();

	posX	= x;
	posY	= y;
	posZ	= z;

	float elapsed = static_cast<float>(timer.StopAndGetElapsed());

	D3DXMATRIX	translate;

	D3DXMatrixIdentity(&translate);
	
	D3DXMatrixTranslation(&translate, static_cast<float>(D3DXToRadian(x * elapsed)), static_cast<float>(D3DXToRadian(y * elapsed)),
		static_cast<float>(D3DXToRadian(z * elapsed)));
	D3DXMatrixMultiply(&worldMatrix, &translate, &worldMatrix);

	return;
}

