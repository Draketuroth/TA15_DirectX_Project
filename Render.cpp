
#include "Render.h"

void Render(GraphicComponents &gHandler, BufferComponents &bHandler, TextureComponents &tHandler, FbxImport &fbxImporter, Terrain &terrain) {

	static bool blurFilter = false;
	UINT message;

	// clear the back buffer to a deep blue
	float clearColor[] = { 0, 0, 0, 1 };	// Back buffer clear color as an array of floats (rgba)
	gHandler.gDeviceContext->ClearRenderTargetView(gHandler.gBackbufferRTV, clearColor);	// Clear the render target view using the specified color
	gHandler.gDeviceContext->ClearRenderTargetView(tHandler.geometryTextureRTV, clearColor);
	gHandler.gDeviceContext->ClearDepthStencilView(bHandler.depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);	// Clear the depth stencil view

	//----------------------------------------------------------------------------------------------------------------------------------//
	// SHADOW MAP PIPELINE (FOR SHADOW MAPPING)
	//----------------------------------------------------------------------------------------------------------------------------------//

	gHandler.gDeviceContext->OMSetRenderTargets(0, 0, tHandler.pSmDepthView);
	gHandler.gDeviceContext->ClearDepthStencilView(tHandler.pSmDepthView, D3D11_CLEAR_DEPTH, 1.0f, 0);


	gHandler.gDeviceContext->VSSetShader(gHandler.gShadowVS, nullptr, 0);	// Setting the Vertex Shader 
	gHandler.gDeviceContext->GSSetShader(nullptr, nullptr, 0); // Setting the Geometry Shader 
	gHandler.gDeviceContext->PSSetShader(nullptr, nullptr, 0); // Setting the Pixel Shader 
	gHandler.gDeviceContext->RSSetState(bHandler.gRasteriserState);
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 0, nullptr); // Setting the Constant Buffer for the Vertex Shader
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer);

	// The stride and offset must be stored in variables as we need to provide pointers to these when setting the vertex buffer
	UINT32 vertexSize = sizeof(OBJStruct);
	UINT32 offset = 0;
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.gTerrainBuffer, &vertexSize, &offset);

	// The input assembler will now recieve the vertices and the vertex layout

	// The vertices should be interpreted as parts of a triangle in the input assembler
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gVertexTerrainLayout);

	gHandler.gDeviceContext->Draw(bHandler.ImportStruct.size(), 0);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// FIRST PASS TO DRAW GEOMETRY (Change render target view to render geometry to a texture that is separate from the back buffer texture)
	//----------------------------------------------------------------------------------------------------------------------------------//

	gHandler.gDeviceContext->OMSetDepthStencilState(bHandler.depthState, 1);
	gHandler.gDeviceContext->OMSetRenderTargets(1, &tHandler.geometryTextureRTV, bHandler.depthView);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// SKELETAL ANIMATION RENDER
	//----------------------------------------------------------------------------------------------------------------------------------//

	gHandler.gDeviceContext->VSSetShader(gHandler.gVertexBoneShader, nullptr, 0);	// Setting the Vertex Shader 
	gHandler.gDeviceContext->GSSetShader(gHandler.gGeometryBoneShader, nullptr, 0); // Setting the Geometry Shader 
	gHandler.gDeviceContext->PSSetShader(gHandler.gPixelBoneShader, nullptr, 0); // Setting the Pixel Shader 
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer); // Setting the Constant Buffer for the Vertex Shader
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &fbxImporter.gBoneBuffer);
	gHandler.gDeviceContext->RSSetState(bHandler.gRasteriserState);
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.boneResource);

	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	// The stride and offset must be stored in variables as we need to provide pointers to these when setting the vertex buffer
	vertexSize = sizeof(Vertex_Bone);
	offset = 0;
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &fbxImporter.gBoneVertexBuffer, &vertexSize, &offset);

	//// The input assembler will now recieve the vertices and the vertex layout

	// The vertices should be interpreted as parts of a triangle in the input assembler
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gVertexBoneLayout);

	gHandler.gDeviceContext->Draw(fbxImporter.vertices.size(), 0);




	//----------------------------------------------------------------------------------------------------------------------------------//
	// OBJ PARSER PIPELINE
	//----------------------------------------------------------------------------------------------------------------------------------//
	ID3D11ShaderResourceView* resourceArr[2];
	resourceArr[0] = tHandler.grassResource;
	resourceArr[1] = tHandler.pSmSRView;

	if (bHandler.fileFound == true)
	{
		//Array of resourceviews for shadow map and textures
		ID3D11ShaderResourceView* resourceArr[2];
		resourceArr[0] = tHandler.terrainResource;
		resourceArr[1] = tHandler.pSmSRView;
		//Array of samplerstates for texture and shadowmap
		ID3D11SamplerState* samplerArr[2];
		samplerArr[0] = tHandler.texSampler;
		samplerArr[1] = tHandler.shadowSampler;

		gHandler.gDeviceContext->VSSetShader(gHandler.gVertexTerrainShader, nullptr, 0);	// Setting the Vertex Shader 
		gHandler.gDeviceContext->GSSetShader(gHandler.gGeometryTerrainShader, nullptr, 0); // Setting the Geometry Shader 
		gHandler.gDeviceContext->PSSetShader(gHandler.gPixelTerrainShader, nullptr, 0); // Setting the Pixel Shader 
		gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer); // Setting the Constant Buffer for the Vertex Shader
		gHandler.gDeviceContext->PSSetConstantBuffers(0, 1, &bHandler.gMTLBuffer);
		gHandler.gDeviceContext->RSSetState(bHandler.gRasteriserState);
		gHandler.gDeviceContext->PSSetShaderResources(0, 2, resourceArr);
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

	////Terrain 
	////___________________________________________________________________________________________________


	gHandler.gDeviceContext->VSSetShader(gHandler.gVertexTerrainShader, nullptr, 0);	// Setting the Vertex Shader 
	gHandler.gDeviceContext->GSSetShader(gHandler.gGeometryTerrainShader, nullptr, 0); // Setting the Geometry Shader 
	gHandler.gDeviceContext->PSSetShader(gHandler.gPixelTerrainShader, nullptr, 0); // Setting the Pixel Shader 
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer); // Setting the Constant Buffer for the Vertex Shader
																					//gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, );
	gHandler.gDeviceContext->PSSetShaderResources(0, 2, resourceArr);
	gHandler.gDeviceContext->RSSetState(bHandler.gRasteriserState);

	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	// The stride and offset must be stored in variables as we need to provide pointers to these when setting the vertex buffer
	vertexSize = sizeof(OBJStruct);
	offset = 0;
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &terrain.mQuadPatchVB, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetIndexBuffer(terrain.mQuadPatchIB, DXGI_FORMAT_R32_UINT, offset);

	// The input assembler will now recieve the vertices and the vertex layout

	// The vertices should be interpreted as parts of a triangle in the input assembler
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gVertexTerrainLayout);

	gHandler.gDeviceContext->DrawIndexed(terrain.indexCounter, 0, 0);

	ID3D11ShaderResourceView* nullResource[2] = { nullptr };
	//resourceArr[1] = nullptr;
	gHandler.gDeviceContext->PSSetShaderResources(0, 2, nullResource);

	////----------------------------------------------------------------------------------------------------------------------------------//
	//// CUBE PIPELINE (FOR NORMAL MAPPING)
	////----------------------------------------------------------------------------------------------------------------------------------//

	ID3D11Buffer* nullGeometryShader[1] = { nullptr };

	gHandler.gDeviceContext->VSSetShader(gHandler.gCylinderVertexShader, nullptr, 0);	// Setting the Vertex Shader 
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer); // Setting the Constant Buffer for the Vertex Shader
	gHandler.gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, nullGeometryShader);
	gHandler.gDeviceContext->PSSetShader(gHandler.gCylinderFragmentShader, nullptr, 0); // Setting the Pixel Shader 
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.standardResource);
	gHandler.gDeviceContext->PSSetShaderResources(1, 1, &tHandler.normalMap);
	gHandler.gDeviceContext->PSSetSamplers(1, 1, &tHandler.texSampler);

	// The stride and offset must be stored in variables as we need to provide pointers to these when setting the vertex buffer
	vertexSize = sizeof(PosNormalTexTan);	// TriangleVertex struct has a total of 5 floats
	offset = 0;
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.gCylinderBuffer, &vertexSize, &offset);
	gHandler.gDeviceContext->IASetIndexBuffer(bHandler.gCylinderIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// The input assembler will now recieve the vertices and the vertex layout

	// The vertices should be interpreted as parts of a triangle in the input assembler
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gCylinderLayout);

	gHandler.gDeviceContext->DrawIndexed(bHandler.cylinderIndicesCount, 0, 0);

	////----------------------------------------------------------------------------------------------------------------------------------//
	//// STANDARD PIPELINE (NOT FOR SHADOW MAPPING)
	////----------------------------------------------------------------------------------------------------------------------------------//

	gHandler.gDeviceContext->OMSetBlendState(tHandler.blendState,0, 0xffffffff);

	gHandler.gDeviceContext->VSSetShader(gHandler.gVertexShader, nullptr, 0);	// Setting the Vertex Shader 
	gHandler.gDeviceContext->GSSetShader(gHandler.gGeometryShader, nullptr, 0); // Setting the Geometry Shader 
	gHandler.gDeviceContext->PSSetShader(gHandler.gPixelShader, nullptr, 0); // Setting the Pixel Shader 
	gHandler.gDeviceContext->RSSetState(bHandler.gRasteriserState);
	gHandler.gDeviceContext->VSSetConstantBuffers(0, 1, &bHandler.gVertexConstantBuffer);
	gHandler.gDeviceContext->GSSetConstantBuffers(0, 1, &bHandler.gConstantBuffer); // Setting the Constant Buffer for the Vertex Shader
	
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.fireflyResource);
	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	// The stride and offset must be stored in variables as we need to provide pointers to these when setting the vertex buffer
	vertexSize = sizeof(TriangleVertex);
	offset = 0;
	gHandler.gDeviceContext->IASetVertexBuffers(0, 1, &bHandler.gVertexBuffer, &vertexSize, &offset);

	// The input assembler will now recieve the vertices and the vertex layout

	// The vertices should be interpreted as parts of a triangle in the input assembler
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	gHandler.gDeviceContext->IASetInputLayout(gHandler.gVertexLayout);

	gHandler.gDeviceContext->Draw(1000, 0);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// COMPUTE SHADER HORIZONTAL BLUR (SECOND PASS)
	//----------------------------------------------------------------------------------------------------------------------------------//

	ID3D11BlendState* nullBlend = nullptr;
	gHandler.gDeviceContext->OMSetBlendState(nullptr, 0, 0xffffffff);

	ID3D11Buffer* nullVBuffer[] = { NULL };
	gHandler.gDeviceContext->IASetVertexBuffers(0, 0, nullVBuffer, NULL, NULL);
	gHandler.gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gHandler.gDeviceContext->IASetInputLayout(NULL);

	gHandler.gDeviceContext->OMSetDepthStencilState(bHandler.depthState, 1);
	gHandler.gDeviceContext->OMSetRenderTargets(1, &gHandler.gBackbufferRTV, bHandler.depthView);

	gHandler.gDeviceContext->CSSetShader(gHandler.gComputeShaderHB, nullptr, 0);
	gHandler.gDeviceContext->CSSetUnorderedAccessViews(0, 1, &tHandler.blurredResultUAV_HB, nullptr);
	gHandler.gDeviceContext->CSSetShaderResources(0, 1, &tHandler.geometryTextureSRV); // <----- Texture we just rendered to in the first pass

	UINT numGroupsX = (UINT)ceilf(WIDTH / 256.0f);
	gHandler.gDeviceContext->Dispatch(numGroupsX, HEIGHT, 1);

	// Unbind output from compute shader
	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	gHandler.gDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Unbind the input textures from the CS
	ID3D11ShaderResourceView* nullSRV[] = { NULL };
	gHandler.gDeviceContext->CSSetShaderResources(0, 1, nullSRV);

	//----------------------------------------------------------------------------------------------------------------------------------//
	// COMPUTE SHADER VERTICAL BLUR (SECOND PASS)
	//----------------------------------------------------------------------------------------------------------------------------------//

	gHandler.gDeviceContext->CSSetShader(gHandler.gComputeShaderHV, nullptr, 0);
	gHandler.gDeviceContext->CSSetUnorderedAccessViews(0, 1, &tHandler.blurredResultUAV_HV, nullptr);
	gHandler.gDeviceContext->CSSetShaderResources(0, 1, &tHandler.blurredResultSRV_HB); // <----- Texture we just rendered to in the first pass

	UINT numGroupsY = (UINT)ceilf(HEIGHT / 256.0f);
	gHandler.gDeviceContext->Dispatch(WIDTH, numGroupsY, 1);

	gHandler.gDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	gHandler.gDeviceContext->CSSetShaderResources(0, 1, nullSRV);
	gHandler.gDeviceContext->CSSetShader(nullptr, nullptr, 0);

	gHandler.gDeviceContext->VSSetShader(gHandler.gQuadVertexShader, nullptr, 0);
	gHandler.gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gHandler.gDeviceContext->PSSetShader(gHandler.gQuadPixelShader, nullptr, 0);

	if (GetAsyncKeyState('B') & 0x8000) {

		blurFilter = !blurFilter;
		Sleep(200);
	}
	
	if (blurFilter == true){

		gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.blurredResultSRV_HV);

	}

	else if (blurFilter == false) {

		gHandler.gDeviceContext->PSSetShaderResources(0, 1, &tHandler.geometryTextureSRV);
	}
	
	gHandler.gDeviceContext->PSSetSamplers(0, 1, &tHandler.texSampler);

	gHandler.gDeviceContext->Draw(3, 0);

	ID3D11ShaderResourceView *const pSRV[1] = { NULL };
	gHandler.gDeviceContext->PSSetShaderResources(0, 1, pSRV);
	gHandler.gDeviceContext->PSSetShaderResources(1, 1, pSRV);
}


