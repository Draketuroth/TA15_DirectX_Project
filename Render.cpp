
#include "Render.h"

void Render(GraphicComponents &gHandler, BufferComponents &bHandler, TextureComponents &tHandler, FbxImport &fbxImporter, Terrain &terrain) {

	// clear the back buffer to a deep blue
	float clearColor[] = { 0, 0, 1, 1 };	// Back buffer clear color as an array of floats (rgba)
	gHandler.gDeviceContext->ClearRenderTargetView(gHandler.gBackbufferRTV, clearColor);	// Clear the render target view using the specified color
	gHandler.gDeviceContext->ClearDepthStencilView(bHandler.depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);	// Clear the depth stencil view

	//----------------------------------------------------------------------------------------------------------------------------------//
	// SKELETAL ANIMATION RENDER
	//----------------------------------------------------------------------------------------------------------------------------------//

	gHandler.gDeviceContext->VSSetShader(gHandler.gVertexBoneShader, nullptr, 0);	// Setting the Vertex Shader 
	gHandler.gDeviceContext->GSSetShader(gHandler.gGeometryBoneShader, nullptr, 0); // Setting the Geometry Shader 
	gHandler.gDeviceContext->PSSetShader(gHandler.gPixelBoneShader, nullptr, 0); // Setting the Pixel Shader 

	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer); // Setting the Constant Buffer for the Vertex Shader
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &fbxImporter.gBoneBuffer);
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.boneResource);

	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	// The stride and offset must be stored in variables as we need to provide pointers to these when setting the vertex buffer
	UINT32 vertexSize = sizeof(Vertex_Bone);
	UINT32 offset = 0;
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &fbxImporter.gBoneVertexBuffer, &vertexSize, &offset);

	// The input assembler will now recieve the vertices and the vertex layout

	// The vertices should be interpreted as parts of a triangle in the input assembler
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gVertexBoneLayout);

	gHandler.gDeviceContext->Draw(fbxImporter.vertices.size(), 0);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// STANDARD PIPELINE (FOR SHADOW MAPPING)
	//----------------------------------------------------------------------------------------------------------------------------------//

	//gHandler.gDeviceContext->VSSetShader(gHandler.gVertexShader, nullptr, 0);	// Setting the Vertex Shader 
	//gHandler.gDeviceContext->GSSetShader(gHandler.gGeometryShader, nullptr, 0); // Setting the Geometry Shader 
	//gHandler.gDeviceContext->PSSetShader(gHandler.gPixelShader, nullptr, 0); // Setting the Pixel Shader 
	//gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer); // Setting the Constant Buffer for the Vertex Shader
	//gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &bHandler.gVertexBuffer);
	//gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.standardResource);

	//gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	//// The stride and offset must be stored in variables as we need to provide pointers to these when setting the vertex buffer
	//vertexSize = sizeof(TriangleVertex);
	//offset = 0;
	//gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.gVertexBuffer, &vertexSize, &offset);

	//// The input assembler will now recieve the vertices and the vertex layout

	//// The vertices should be interpreted as parts of a triangle in the input assembler
	//gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//gHandler.gDeviceContext->IASetInputLayout(gHandler.gVertexLayout);

	//gHandler.gDeviceContext->Draw(6, 0);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// OBJ PARSER PIPELINE
	//----------------------------------------------------------------------------------------------------------------------------------//

	gHandler.gDeviceContext->VSSetShader(gHandler.gVertexTerrainShader, nullptr, 0);	// Setting the Vertex Shader 
	gHandler.gDeviceContext->GSSetShader(gHandler.gGeometryTerrainShader, nullptr, 0); // Setting the Geometry Shader 
	gHandler.gDeviceContext->PSSetShader(gHandler.gPixelTerrainShader, nullptr, 0); // Setting the Pixel Shader 
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer); // Setting the Constant Buffer for the Vertex Shader
	//gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, );
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.standardResource);

	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	// The stride and offset must be stored in variables as we need to provide pointers to these when setting the vertex buffer
	vertexSize = sizeof(OBJStruct);
	offset = 0;
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.gTerrainBuffer, &vertexSize, &offset);

	// The input assembler will now recieve the vertices and the vertex layout

	// The vertices should be interpreted as parts of a triangle in the input assembler
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gVertexTerrainLayout);

	gHandler.gDeviceContext->Draw(bHandler.ImportStruct.size(), 0);

}