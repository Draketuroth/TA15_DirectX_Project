
#include "GraphicComponents.h"
#include "BufferComponents.h"
#include "TextureComponents.h"

void Render(GraphicComponents &gHandler, BufferComponents &bHandler, TextureComponents &tHandler, FbxImport &fbxImporter, Terrain &terrain);

void ClearRenderTargets(GraphicComponents &gHandler, BufferComponents &bHandler, TextureComponents &tHandler);

void RenderShadowMap(GraphicComponents &gHandler, BufferComponents &bHandler, TextureComponents &tHandler);

void SetGeometryTexture(GraphicComponents &gHandler, BufferComponents &bHandler, TextureComponents &tHandler);

void RenderSkeletalAnimation(GraphicComponents &gHandler, BufferComponents &bHandler, TextureComponents &tHandler, FbxImport &fbxImporter);

void RenderObjTerrain(GraphicComponents &gHandler, BufferComponents &bHandler, TextureComponents &tHandler, Terrain &terrain);

void RenderCylinder(GraphicComponents &gHandler, BufferComponents &bHandler, TextureComponents &tHandler);

void RenderParticles(GraphicComponents &gHandler, BufferComponents &bHandler, TextureComponents &tHandler);

void ComputeBlur(GraphicComponents &gHandler, BufferComponents &bHandler, TextureComponents &tHandler);

void DrawFullScreenQuad(GraphicComponents &gHandler, BufferComponents &bHandler, TextureComponents &tHandler);
