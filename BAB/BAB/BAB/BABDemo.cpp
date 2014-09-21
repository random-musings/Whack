#include "stdafx.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	BABDemo theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}


BABDemo::BABDemo(HINSTANCE hInstance)
:D3DApp(hInstance),
	mFX(0), 
	mTech(0),
	mfxWorldViewProj(0), 
	mInputLayout(0), 
	mWireframeRS(0),
	mMoleCount(9),
	mMoleRows(3),
	mMoleCols(3),
	mMoleStartPosition(-5.0f,0.0f,-5.0f),
	mMoleSpacing(5.0f,0.0f,5.0f),
	mfxDirLight(0)
{
	mCamera.SetPosition(0,-1,-5);
		// Convert Spherical to Cartesian coordinates.
	float mPhi=0.1f;
	float mTheta =7.8575f;
	float mRadius = 17.0f;
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi) ;

	mEyePosW =XMFLOAT3(x,y,z);
	// Build the view matrix.
	XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	mCamera.SetPosition(0,-1,-5);
	mCamera.LookAt(pos,  //Position
				target, //look at 
				up); //up
	//+		mUp	{x=0.0035008576 y=0.099833421 z=-0.99499804 }	_XMFLOAT3

	// Directional light.
	mDirLight.Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLight.Diffuse  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.Direction = XMFLOAT3(0.0f, 1.0f, 0.0f);

	mGridMat.Ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mGridMat.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mGridMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mMoleMat.Ambient  = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	mMoleMat.Diffuse  = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	mMoleMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);

	mCylinderMat.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mCylinderMat.Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mCylinderMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 96.0f);
}

BABDemo::~BABDemo(void)
{
	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mWireframeRS);

}



void BABDemo::OnResize()
{
	D3DApp::OnResize();
	mCamera.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}


bool BABDemo::Init()
{
	if(!D3DApp::Init())
		return false;

	BuildFX();
	BuildGeometryBuffers();
	SetExplosionArray();
	BuildVertexLayout();
	return true;
}

void BABDemo::BuildFX()
{
	std::ifstream fin("fx/Lighting.fxo", std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 
		0, md3dDevice, &mFX));

	mTech                = mFX->GetTechniqueByName("LightTech");
	mfxWorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxWorld             = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mfxEyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	mfxDirLight          = mFX->GetVariableByName("gDirLight");
	mfxMaterial          = mFX->GetVariableByName("gMaterial");

}

void BABDemo::BuildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3DX11_PASS_DESC passDesc;
    mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout));

}

void BABDemo::BuildGeometryBuffers()
{

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridPosition, I);

	gamePieces.Init(	md3dDevice,
						1.0f,//float newHoleRadius, 
						XMFLOAT4(0.0f,0.0f,0.0f,1.0f)	,//XMFLOAT4 newHoleColor,
						0.5f,//float newMoleRadius,
						XMFLOAT4(0.8f,0.8f,1.0f,1.0f),//XMFLOAT4 newMoleColor,
						3.0f,//float newCylinderHeight, 
						0.5f,//float newCylinderBottom, 
						0.3f,//float newCylinderTop
						XMFLOAT4(0.3f,0.3f,1.0f,1.0f),//XMFLOAT4 newCylinderColor,
						5.0f,//float newGridDepth,
						5.0f,//float newGridWidth,
						XMFLOAT4(0.6f,0.6f,1.0f,1.0f));//XMFLOAT4 newGridColor)
	
	mMoles.Init(		XMFLOAT3(0.0f,0.0f,0.0f),//XMFLOAT3 bottomPosition,
						XMFLOAT3(0.0f,4.0f,0.0f),//XMFLOAT3 topPosition,
						XMFLOAT3(0.0f,0.5f,0.0f),//XMFLOAT3 speed UP,
						XMFLOAT3(0.0f,-0.3f,0.0f),//XMFLOAT3 speed DOwn,
						XMFLOAT3(0.0f,-1.0f,0.0f),//XMFLOAT3 speed Hit,
						3.0f,//float restAtTopTime,
						3,//float maxMolesMoving,
						0.02f,//float timeBetweenUpdates,
						0.5f,//float timeBetweenMolePop,
						XMFLOAT3(0.0f,0.0f,0.0f),//XMFLOAT3 startMolePosition,
						XMFLOAT3(0.0f,0.0f,0.0f),//XMFLOAT3 moleSpacing,
						mMoleCols,//UINT moleColumns,
						mMoleRows,//UINT moleRows,
						0.5f);//float moleRadius);

	float cylinderHeight = -3.0f;
	float holeHeight = -0.1f;
	float moleHeight = -1.0f;
	
	//set initial positions for moles/holes/cylinders	
	for(int i=0;i<mMoleCols;i++)
	{
		for(int j = 0; j<mMoleRows;j++)
		{
			XMMATRIX currPosition  = XMMatrixTranslation(mMoles.GetMolePosition(i).x,mMoles.GetMolePosition(i).y,mMoles.GetMolePosition(i).z);
			XMStoreFloat4x4(&mMolePositions[i*mMoleCols +j],
							XMMatrixTranslation(mMoleStartPosition.x + mMoleSpacing.x * j,
												mMoleStartPosition.y + moleHeight + mMoleSpacing.y,
												mMoleStartPosition.z + mMoleSpacing.z *  i)
												);
			XMStoreFloat4x4(&mHolePositions[i*mMoleCols +j],
							XMMatrixTranslation(mMoleStartPosition.x + mMoleSpacing.x * j,
												mMoleStartPosition.y + holeHeight + mMoleSpacing.y,
												mMoleStartPosition.z + mMoleSpacing.z *  i)
												);
			XMStoreFloat4x4(&mCylinderPositions[i*mMoleCols +j],
							XMMatrixTranslation(mMoleStartPosition.x + mMoleSpacing.x * j,
												mMoleStartPosition.y + cylinderHeight + mMoleSpacing.y,
												mMoleStartPosition.z + mMoleSpacing.z *  i)
												);
		}
	}
}


void BABDemo::OnMouseDown(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		Pick(x,y);
	}
	mLastMousePosition.x = x;
	mLastMousePosition.y = y;
}

void BABDemo::OnMouseUp(WPARAM btnState, int x, int y){}

void BABDemo::OnMouseMove(WPARAM btnState, int x, int y){}

void BABDemo::Pick(int screenX, int screenY)
{
	XMMATRIX P = mCamera.Proj();

	// Compute picking ray in view space.
	float vx = (+2.0f*screenX/mClientWidth  - 1.0f)/P(0,0);
	float vy = (-2.0f*screenY/mClientHeight + 1.0f)/P(1,1);

	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir    = XMVectorSet(vx, vy, 1.0f, 0.0f);

	// Tranform ray to local space of Mesh.
	XMMATRIX V = mCamera.View();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	XMMATRIX W = XMLoadFloat4x4(&mGridPosition);
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the intersection tests.
	rayDir = XMVector3Normalize(rayDir);

	for(int i=0;i<mMoleCols;i++)
	{
		for(int j=0;j<mMoleRows;j++)
			{
			XMFLOAT3 currMolePosition = mMoles.GetMolePosition(i*mMoleCols+j);
			if(currMolePosition.y> mMoleStartPosition.y)
			{	//only check if popping
				XNA::Sphere tmpSphere;
				tmpSphere.Center =XMFLOAT3(	mMoleStartPosition.x + mMoleSpacing.x * j + currMolePosition.x ,
											mMoleStartPosition.y + mMoleSpacing.y + currMolePosition.y, 
											mMoleStartPosition.z + mMoleSpacing.z * i  + currMolePosition.z);
				tmpSphere.Radius =mMoles.GetMoleAtPosition(i).mRadius + 0.5f*mMoles.GetMoleAtPosition(i).mRadius ;
				float tmpDist;
				if(XNA::IntersectRaySphere(rayOrigin,rayDir,&tmpSphere,&tmpDist))
				{
					mMoles.HitMole(i*mMoleCols +j);
					mExplosions[i*mMoleCols+j]=1;
				}
			}
		}
	}
}


void BABDemo::UpdateScene(float dt)
{
	mMoles.UpdateMoles(dt);
	UpdateExplosions(dt);
}

void BABDemo::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//md3dImmediateContext->RSSetState(mWireframeRS);

	mfxDirLight->SetRawValue(&mDirLight, 0, sizeof(mDirLight));
	mfxEyePosW->SetRawValue(&mEyePosW, 0, sizeof(mEyePosW));

	UINT stride = sizeof(Vertex);
    UINT offset = 0;
	ID3D11Buffer* tmpBuffer =gamePieces.GetGamePiecesVertex();
    md3dImmediateContext->IASetVertexBuffers(0, 1,&tmpBuffer, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer( gamePieces.GetGamePiecesIndices(), DXGI_FORMAT_R32_UINT, 0);

	mCamera.UpdateViewMatrix();
	
	XMMATRIX view  = mCamera.View();
	XMMATRIX proj  = mCamera.Proj();
	XMMATRIX viewProj = mCamera.ViewProj();

	D3DX11_TECHNIQUE_DESC techDesc;

    mTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		//draw grid
		XMMATRIX world = XMLoadFloat4x4(&mGridPosition);
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		mfxMaterial->SetRawValue(&mGridMat, 0, sizeof(mGridMat));
		md3dImmediateContext->DrawIndexed(gamePieces.GetGridIndiceCount(),
										  gamePieces.GetGridIndiceOffset(),
										  gamePieces.GetGridVertexOffset());
/*	
		
		for(UINT i = 0; i <mMoleCount; i++)
		{
			XMMATRIX translation = XMMatrixTranslation(mMoles.GetMolePosition(i).x, mMoles.GetMolePosition(i).y, mMoles.GetMolePosition(i).z);//XMMatrixTranslation(mMoles.GetMolePosition(i).x, mMoles.GetMolePosition(i).y, mMoles.GetMolePosition(i).z);

			//draw Cylinders
			world = XMLoadFloat4x4(&mCylinderPositions[i]);
			world = world * translation;
			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
			mfxMaterial->SetRawValue(&mCylinderMat, 0, sizeof(mCylinderMat));
			mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(	gamePieces.GetCylinderIndiceCount(),
												gamePieces.GetCylinderIndiceOffset(),
												gamePieces.GetCylinderVertexOffset());
			
			//draw Moles
			world = XMLoadFloat4x4(&mMolePositions[i]);
			world = world * translation;
			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
			mfxMaterial->SetRawValue(&mMoleMat, 0, sizeof(mMoleMat));
			mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(	gamePieces.GetMoleIndiceCount(),
												gamePieces.GetMoleIndiceOffset(),
												gamePieces.GetMoleVertexOffset());


			//draw Holes
			world = XMLoadFloat4x4(&mHolePositions[i]);
			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*viewProj)));
			mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(	gamePieces.GetHoleIndiceCount(),
												gamePieces.GetHoleIndiceOffset(),
												gamePieces.GetHoleVertexOffset());	
						
			if(mExplosions[i]>0 )
			{
				//draw Explosion		
				md3dImmediateContext->DrawIndexed(	gamePieces.GetExplosionIndiceCount(),
														gamePieces.GetExplosionIndiceOffset(),
														gamePieces.GetExplosionVertexOffset());	
			}
		}

	*/
		
	}
	HR(mSwapChain->Present(0,0));
}



void BABDemo::SetExplosionArray()
{
	mMaxExplosionTime=1.025f;
	for(UINT i =0; i<mMoleCount;i++)
		mExplosions[i]=0;
}


void BABDemo::UpdateExplosions(float dt)
{
	 for(UINT i=0;i<mMoleCount;i++)
	 {
		 if(mExplosions[i]>0)
		 {
			if(mExplosions[i]>mMaxExplosionTime)
				mExplosions[i]=0;
			else
				mExplosions[i]+=  dt;
		 }
	 }
}