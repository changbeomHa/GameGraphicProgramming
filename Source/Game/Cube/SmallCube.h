#pragma once
#include "Cube/BaseCube.h"

class SmallCube : public BaseCube {
public:

	virtual void Update(_In_ FLOAT deltaTime);
private:
	XMMATRIX mSpin = XMMatrixIdentity();
	XMMATRIX mOrbit = XMMatrixIdentity();
	XMMATRIX mTranslate = XMMatrixIdentity();
	XMMATRIX mScale = XMMatrixIdentity();
	XMMATRIX val = XMMatrixIdentity();

	void Spin(_In_ FLOAT deltaTime) {
		mOrbit *= XMMatrixRotationY(-deltaTime * 2.0f);
		mTranslate *= XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
		val = mSpin * mOrbit;
	}
};

void SmallCube::Update(_In_ FLOAT deltaTime) {
	Spin(deltaTime);
	mSpin = XMMatrixRotationZ(-deltaTime);
	//mOrbit = XMMatrixRotationY(-deltaTime * 2.0f);
	mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
	mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);

	m_world = mScale * mTranslate * mSpin * mOrbit;
}
