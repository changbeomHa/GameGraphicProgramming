#pragma once
#include "Cube/BaseCube.h"

class ThirdCube : public BaseCube {
public:

	virtual void Update(_In_ FLOAT deltaTime);
private:
	XMMATRIX mSpin = XMMatrixIdentity();
	XMMATRIX mOrbit = XMMatrixIdentity();
	XMMATRIX mTranslate = XMMatrixIdentity();
	XMMATRIX mScale = XMMatrixIdentity();
	XMMATRIX val = XMMatrixIdentity();

	void Spin(_In_ FLOAT deltaTime) {
		mOrbit *= XMMatrixRotationY(-deltaTime * 1.5f);
		mTranslate *= XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
		val = mSpin * mOrbit;
	}
};

void ThirdCube::Update(_In_ FLOAT deltaTime) {
	Spin(deltaTime);
	mSpin = XMMatrixRotationZ(-deltaTime);
	//mOrbit = XMMatrixRotationY(-deltaTime * 2.0f);
	mTranslate = XMMatrixTranslation(-2.0f, 1.0f, 0.0f);
	mScale = XMMatrixScaling(0.5f, 1.5f, 0.5f);

	m_world = mScale * mTranslate * mSpin * mOrbit;
}
