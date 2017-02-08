#include "GeometryGenarator.h"
void GeometryGenarator::Grid(float width, float depth, int m, int n, MechData& mechData)
{
	int vertexCount = m*n; 
	int faceCount = (m-1)*(n-1)*2; 

	float halfWidth = 0.5f*width; 
	float halfDepth = 0.5f*depth; 

	float dx = width / (n-1); 
	float dz = depth / (m-1); 

	float du = 1.0f/ (n-1); 
	float dv = 1.0f / (m-1); 
	mechData.Vertices.resize(vertexCount); 

	for(int i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dz; 
		for(int j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx; 
			mechData.Vertices[i*n+j].Position = XMFLOAT3(x, 0.0f, z); 

			//används för lightning
			//mechData.Vertices[i*n+j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f); 

			////används för texturing
			//mechData.Vertices[i*n+j].TexC.x = j*du; 
			//mechData.Vertices[i*n+j].TexC.y = i*dv; 

		}

	}
}
