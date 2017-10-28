#include "Camera.h"

Camera::Camera()
{
	_positionX = 0.0f;
	_positionY = 0.0f;
	_positionZ = 0.0f;

	_rotationX = 0.0f;
	_rotationY = 0.0f;
	_rotationZ = 0.0f;
}

Camera::Camera(const Camera& other)
{
}


Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	_positionX = x;
	_positionY = y;
	_positionZ = z;
	return;
}

//void Camera::SetRelativePosition(float x, float y, float z)
//{
//	XMFLOAT3 desiredPos(x, y, z);
//	XMVECTOR desiredPosCamSpace = XMLoadFloat3(&desiredPos);
//
//	XMMATRIX lookatMatrix = DirectX::XMMatrixRotationRollPitchYaw(_rotationX*0.0174532925f, _rotationX*0.0174532925f, _rotationX*0.0174532925f);
//	
//	// take inverse of current orientation (use actual inverse if this doesnt work)
//	//XMMATRIX inverseOrientation = XMMatrixTranspose(lookatMatrix);
//	XMFLOAT3 det(1, 1, 1);
//	XMVECTOR determinant = XMLoadFloat3(&det);
//	DirectX::XMVector3Normalize(determinant);
//	XMMATRIX inverseOrientation = XMMatrixInverse(&determinant, lookatMatrix);
//
//	// Unrotate the desired position
//	XMVECTOR desiredPosWorldSpace = XMVector3TransformCoord(desiredPosCamSpace, inverseOrientation);
//
//	// set these world values
//	_positionX = XMVectorGetX(desiredPosWorldSpace);
//	_positionY = XMVectorGetY(desiredPosWorldSpace);
//	_positionZ = XMVectorGetZ(desiredPosWorldSpace);
//}
void Camera::SetRelativePosition(float x, float y, float z)
{
	XMFLOAT3 displacement(x, y, z);
	XMVECTOR desiredPosCamSpace = XMLoadFloat3(&desiredDisplacement);

	XMMATRIX lookatMatrix = DirectX::XMMatrixRotationRollPitchYaw(_rotationX*0.0174532925f, _rotationX*0.0174532925f, _rotationX*0.0174532925f);

	// take inverse of current orientation (use actual inverse if this doesnt work)
	//XMMATRIX inverseOrientation = XMMatrixTranspose(lookatMatrix);
	XMFLOAT3 det(1, 1, 1);
	XMVECTOR determinant = XMLoadFloat3(&det);
	DirectX::XMVector3Normalize(determinant);
	XMMATRIX inverseOrientation = XMMatrixInverse(&determinant, lookatMatrix);

	// Unrotate the desired position
	XMVECTOR desiredPosWorldSpace = XMVector3TransformCoord(desiredPosCamSpace, inverseOrientation);

	// set these world values
	_positionX = XMVectorGetX(desiredPosWorldSpace);
	_positionY = XMVectorGetY(desiredPosWorldSpace);
	_positionZ = XMVectorGetZ(desiredPosWorldSpace);
}

void Camera::SetRotation(float x, float y, float z)
{
	_rotationX = x;
	_rotationY = y;
	_rotationZ = z;
	return;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(_positionX, _positionY, _positionZ);
}

XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(_rotationX, _rotationY, _rotationZ);
}

void Camera::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	position.x = _positionX;
	position.y = _positionY;
	position.z = _positionZ;

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	// convert yaw (Y axis), pitch (X axis), and roll (Z axis) to radians.
	pitch = _rotationX * 0.0174532925f;
	yaw = _rotationY * 0.0174532925f;
	roll = _rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	_viewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = _viewMatrix;
	return;
}