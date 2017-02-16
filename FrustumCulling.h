#pragma once
#include "BufferComponents.h";
struct Plane {
	XMFLOAT3 Top;
	XMFLOAT3 Bot;
	XMFLOAT3 Right;
	XMFLOAT3 Left;
	XMFLOAT3 NearP;
	XMFLOAT3 FarP;
};
class FCull
{
public:
	FCull();
	~FCull();

	Plane frustum[6];
	


private:

};

