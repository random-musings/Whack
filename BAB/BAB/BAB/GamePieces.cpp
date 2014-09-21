#include "stdafx.h"

GamePieces::GamePieces():
	holeRadius(0),
	holeColor(0.0f,0.0f,0.0f,1.0f),
	moleRadius(0),
	moleColor(0.0f,0.0f,0.0f,1.0f),
	cylinderHeight(0),
	cylinderBottom(0),
	cylinderTop(0),
	cylinderColor(0.0f,0.0f,0.0f,1.0f),
	mVB(0),
	mIB(0),
	mVB2(0),
	mIB2(0),
	moleIndicesCount(0),
	holeIndicesCount(0),
	cylinderIndicesCount(0),	
	moleIndicesOffset(0),
	holeIndicesOffset(0),
	cylinderIndicesOffset(0),	
	moleVertexCount(0),
	holeVertexCount(0),
	cylinderVertexCount(0),	
	moleVertexOffset(0),
	holeVertexOffset(0),
	cylinderVertexOffset(0)
{
}




GamePieces::~GamePieces(void)
{
	if(mVB){mVB->Release();}
	if(mIB){mIB->Release();}
	if(mIB2){mIB2->Release();}
	if(mVB2){mVB2->Release();}

}


ID3D11Buffer* GamePieces::GetGamePiecesVertex(){return mVB;}
ID3D11Buffer* GamePieces::GetGamePiecesIndices(){return mIB;}

	UINT GamePieces::GetSizeOfVertex(){return sizeof(Vertex);}
	UINT GamePieces::GetHoleVertexOffset(){return holeVertexOffset;}
	UINT GamePieces::GetMoleVertexOffset(){return moleVertexOffset;}
	UINT GamePieces::GetCylinderVertexOffset(){return cylinderVertexOffset;}
	UINT GamePieces::GetGridVertexOffset(){return gridVertexOffset;}
	UINT GamePieces::GetExplosionVertexOffset(){return explosionVertexOffset;}

	UINT GamePieces::GetHoleIndiceOffset(){return holeIndicesOffset;}
	UINT GamePieces::GetMoleIndiceOffset(){return moleIndicesOffset;}
	UINT GamePieces::GetCylinderIndiceOffset(){return cylinderIndicesOffset;}
	UINT GamePieces::GetGridIndiceOffset(){return gridIndicesOffset;}
	UINT GamePieces::GetExplosionIndiceOffset(){return explosionIndicesOffset;}

	UINT GamePieces::GetHoleVertexCount(){return holeVertexCount;}
	UINT GamePieces::GetMoleVertexCount(){return moleVertexCount;}
	UINT GamePieces::GetCylinderVertexCount(){return cylinderVertexCount;}
	UINT GamePieces::GetGridVertexCount(){return gridVertexCount;}
	UINT GamePieces::GetExplosionVertexCount(){return explosionVertexCount;}

	UINT GamePieces::GetHoleIndiceCount(){return holeIndicesCount;}
	UINT GamePieces::GetMoleIndiceCount(){return moleIndicesCount;}
	UINT GamePieces::GetCylinderIndiceCount(){return cylinderIndicesCount;}
	UINT GamePieces::GetGridIndiceCount(){return gridIndicesCount;}
	UINT GamePieces::GetExplosionIndiceCount(){return explosionIndicesCount;}

bool GamePieces::Init( ID3D11Device *md3dDevice,
				float newHoleRadius, 
				XMFLOAT4 newHoleColor,
				float newMoleRadius,
				XMFLOAT4 newMoleColor,
				float newCylinderHeight, 
				float newCylinderBottom, 
				float newCylinderTop,
				XMFLOAT4 newCylinderColor,
				float newGridDepth,
				float newGridWidth,
				XMFLOAT4 newGridColor)
{
	cylinderSlices=20;
	cylinderStacks=20;
	gridColumns=40;
	gridRows =60;
	moleSlices = 20;
	moleStacks = 20;
	
	gridDepth = newGridDepth;
	gridWidth = newGridWidth;
	gridColor = newGridColor;
	holeRadius = newHoleRadius;
	holeColor = newHoleColor;
	moleRadius = newMoleRadius;
	moleColor = newMoleColor;
	cylinderHeight = newCylinderHeight;
	cylinderTop = newCylinderTop;
	cylinderBottom = newCylinderBottom;
	cylinderColor = newCylinderColor;


	CreateGeometryBuffers(md3dDevice);
	
	return true;
}

void GamePieces::CreateGeometryBuffers(ID3D11Device *md3dDevice)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData cylinder;
	GeometryGenerator::MeshData mole;
	GeometryGenerator::MeshData hole;
	GeometryGenerator::MeshData grid;
	
	geoGen.CreateCylinder((float)cylinderBottom,(float)cylinderTop,(float)cylinderHeight,(UINT)cylinderSlices,(UINT)cylinderStacks,cylinder);
	geoGen.CreateSphere((float)moleRadius,(UINT)moleSlices,(UINT)moleStacks,mole);
	geoGen.CreateCylinder(holeRadius,holeRadius,1.0f,20,20,hole);
	geoGen.CreateGrid((float)gridWidth,(float)gridDepth,(UINT)gridColumns,(UINT)gridRows,grid);

	cylinderVertexOffset	= 0;
	cylinderIndicesCount	= cylinder.Indices.size();
	cylinderIndicesOffset	= 0;
	cylinderVertexCount		= cylinder.Vertices.size();

	moleIndicesCount		= mole.Indices.size();
	moleVertexCount			= mole.Vertices.size();
	moleVertexOffset		= cylinderVertexCount;
	moleIndicesOffset		= cylinderIndicesCount; 

	holeIndicesCount		= hole.Indices.size();
	holeVertexCount			= hole.Vertices.size();
	holeVertexOffset		= moleVertexOffset + moleVertexCount;
	holeIndicesOffset		= moleIndicesOffset + moleIndicesCount; 

	gridIndicesCount		= grid.Indices.size();
	gridVertexCount			= grid.Vertices.size();
	gridVertexOffset		= holeVertexOffset + holeVertexCount;
	gridIndicesOffset		= holeIndicesOffset + holeIndicesCount; 

	explosionIndicesCount	= 9;
	explosionVertexCount	= 9;
	explosionVertexOffset	= gridVertexOffset + gridVertexCount;
	explosionIndicesOffset	= gridIndicesOffset + gridIndicesCount; 


	int k=0;
	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 dkRed(1.0f, 0.3f, 0.3f, 1.0f);
	XMFLOAT4 Orange(1.0f,0.8f,0.0f,1.0f);
	XMFLOAT4 Red(1.0f,0.2f,0.0f,1.0f);
	XMFLOAT4 Yellow(1.0f,1.0f,0.0f,1.0f);

	std::vector<Vertex> vertices(cylinder.Vertices.size() + mole.Vertices.size() +  hole.Vertices.size() + grid.Vertices.size() + explosionVertexCount);
	for(size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos		= cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
	}

	for(size_t i = 0; i < mole.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos   = mole.Vertices[i].Position;
		vertices[k].Normal = mole.Vertices[i].Normal;
	}

	for(size_t i = 0; i < hole.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos   = hole.Vertices[i].Position;
		vertices[k].Normal = hole.Vertices[i].Normal;
	}

	for(size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos   = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
	}


	//EXPLOSION
	vertices[k].Pos		= XMFLOAT3(0.0f,3.5f,-0.25f);
	vertices[k++].Normal = XMFLOAT3(0,1,0);	
	vertices[k].Pos		= XMFLOAT3(-0.25f,3.5f,0.25f);
	vertices[k++].Normal = XMFLOAT3(0,1,0);
	vertices[k].Pos		= XMFLOAT3( 0.25f,3.0f, 0.0f);
	vertices[k++].Normal = XMFLOAT3(0,1,0);

	vertices[k].Pos		= XMFLOAT3(0.1f,3.0f, 0.0f);
	vertices[k++].Normal = XMFLOAT3(0,1,0);
	vertices[k].Pos		= XMFLOAT3(0.25f,3.0f, 0.25f);
	vertices[k++].Normal = XMFLOAT3(0,1,0);
	vertices[k].Pos		= XMFLOAT3( 0.25f,3.0f,-0.5f);
	vertices[k++].Normal = XMFLOAT3(0,1,0);

	vertices[k].Pos		= XMFLOAT3(-0.25f,3.0f,-0.25f);
	vertices[k++].Normal = XMFLOAT3(0,1,0);
	vertices[k].Pos		= XMFLOAT3(-0.25f,3.0f, 0.5f);
	vertices[k++].Normal = XMFLOAT3(0,1,0);
	vertices[k].Pos		= XMFLOAT3( 0.5f,3.0f, -0.25f);
	vertices[k++].Normal = XMFLOAT3(0,1,0);


    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	std::vector<UINT> indices;
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());
	indices.insert(indices.end(), mole.Indices.begin(), mole.Indices.end());
	indices.insert(indices.end(), hole.Indices.begin(), hole.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(5);
	indices.push_back(3);
	indices.push_back(4);
	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(8);


	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));

}

