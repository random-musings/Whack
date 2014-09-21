#pragma once

class BABDemo :
	public D3DApp
{
public:
	BABDemo(HINSTANCE hInstance);
	~BABDemo(void);

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void BuildGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();

private:
	void Pick(int sx, int sy);
	void SetExplosionArray();
	void UpdateExplosions(float dt);

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
	ID3D11InputLayout* mInputLayout;
	ID3D11RasterizerState* mWireframeRS;
	ID3DX11EffectVariable* mfxDirLight;
	ID3DX11EffectVariable* mfxMaterial;
	XMFLOAT3 mEyePosW;
	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxWorldInvTranspose;
	ID3DX11EffectVectorVariable* mfxEyePosW;


	DirectionalLight mDirLight;
	Material mMoleMat;
	Material mCylinderMat;
	Material mGridMat;

	XMFLOAT4X4 mMolePositions[10];
	XMFLOAT4X4 mHolePositions[10];
	XMFLOAT4X4 mCylinderPositions[10];
	XMFLOAT4X4 mGridPosition;
	float mExplosions[10];
	

	UINT mMoleCount;
	Camera mCamera;
	Moles mMoles;
	GamePieces gamePieces;
	POINT mLastMousePosition;
	XMFLOAT3 mMoleStartPosition;
	XMFLOAT3 mMoleSpacing;
	float mMaxExplosionTime;

	int mMoleRows;
	int mMoleCols;
	

};
