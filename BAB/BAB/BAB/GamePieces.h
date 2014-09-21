#pragma once

class GamePieces
{
public:
	ID3D11Buffer *mVB2;
	ID3D11Buffer* mIB2;
	int mGridIndexCount;
	void buildGridPosition(ID3D11Device *md3dDevice);


	int numExplosionIndices;
	int	numExplosionVertices;
public:
	GamePieces( );


	~GamePieces(void);

	ID3D11Buffer *GetGamePiecesVertex();
	ID3D11Buffer *GetGamePiecesIndices();
	UINT GetSizeOfVertex(); 
	UINT GetHoleVertexOffset();
	UINT GetMoleVertexOffset();
	UINT GetCylinderVertexOffset();
	UINT GetGridVertexOffset();
	UINT GetExplosionVertexOffset();

	UINT GetHoleIndiceOffset();
	UINT GetMoleIndiceOffset();
	UINT GetCylinderIndiceOffset();
	UINT GetGridIndiceOffset();
	UINT GetExplosionIndiceOffset();

	UINT GetHoleVertexCount();
	UINT GetMoleVertexCount();
	UINT GetCylinderVertexCount();
	UINT GetGridVertexCount();
	UINT GetExplosionVertexCount();

	UINT GetHoleIndiceCount();
	UINT GetMoleIndiceCount();
	UINT GetCylinderIndiceCount();
	UINT GetGridIndiceCount();
	UINT GetExplosionIndiceCount();

	bool Init( ID3D11Device *md3dDevice,
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
				XMFLOAT4 newGridColor);
private:
	void CreateGeometryBuffers(ID3D11Device *md3dDevice);

private:
	ID3D11Buffer *mVB;
	ID3D11Buffer *mIB;
	
	UINT moleIndicesCount;
	UINT holeIndicesCount;
	UINT cylinderIndicesCount;	
	UINT gridIndicesCount;	
	UINT explosionIndicesCount;	

	UINT moleIndicesOffset;
	UINT holeIndicesOffset;
	UINT cylinderIndicesOffset;	
	UINT gridIndicesOffset;	
	UINT explosionIndicesOffset;	

	UINT moleVertexCount;
	UINT holeVertexCount;
	UINT cylinderVertexCount;	
	UINT gridVertexCount;	
	UINT explosionVertexCount;	

	UINT moleVertexOffset;
	UINT holeVertexOffset;
	UINT cylinderVertexOffset;
	UINT gridVertexOffset;	
	UINT explosionVertexOffset;	

	float holeRadius;
	XMFLOAT4 holeColor;
	float moleRadius;
	XMFLOAT4 moleColor;
	float moleSlices;
	float moleStacks;
	float cylinderHeight; 
	float cylinderBottom;
	float cylinderTop;
	XMFLOAT4 cylinderColor;
	float cylinderSlices;
	float cylinderStacks;
	float gridDepth;
	float gridWidth;
	float gridColumns;
	float gridRows;
	XMFLOAT4 gridColor;

};
