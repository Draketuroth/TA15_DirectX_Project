//----------------------------------------------------------------------------------------------------------------------------------//
// FBX Loader.cpp DirectX11
// Based in FBX SDK 2015 for Visual Studio 2012
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

#include "FBXLoader.h"

// loading funktions
//1. ProcessControlPoints (process vertices)
//2. LoadSkeletonHierarchy (processes joints and clusters)
//3. GatherAnimationData ( processes keyframes and weights)
//4. CreateVertexData ( makes vertex buffer for vertices) 

// updating 
//1. buffercomponents->createSkeletalBuffer.

//----------------------------------------------------------------------------------------------------------------------------------//
// PRIMARY FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------//

FbxImport::FbxImport() {

	gBoneBuffer = nullptr;
	gBoneVertexBuffer = nullptr;
}

FbxImport::~FbxImport() {


}

void FbxImport::ReleaseAll() {

	SAFE_RELEASE(gBoneBuffer);
	SAFE_RELEASE(gBoneVertexBuffer);
}

HRESULT FbxImport::LoadFBX(std::vector<Vertex_Bone>* pOutVertexVector) {

	HRESULT hr;
	// Data parsing file declaration
	ofstream logFile;
	// Storing a pointer for the FBX Manager
	FbxManager* gFbxSdkManager = nullptr;
	FbxNode* pFbxRootNode = nullptr;

	const char* baseFilePath = "FbxModel\\bindpose.fbx";
	const char* currentFilePath;

	// Initialize the FBX loader and instruct it what types of data to load...

	if (gFbxSdkManager == nullptr) {

		// We create our Fbx manager and store it in the Fbx manager pointer variable

		gFbxSdkManager = FbxManager::Create();

		// ...which is the FbxIoSettings, and we can use it to load only meshes and their belonging materials

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(gFbxSdkManager, IOSROOT);

		// We set our Fbx Manager IOsettings with the previously recieved settings specified in the variable above

		gFbxSdkManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(gFbxSdkManager, "");

	FbxScene* pFbxScene = FbxScene::Create(gFbxSdkManager, "");

	//----------------------------------------------------------------------------------------------------------------------------------//
	// LOG FILE: FBX HEADER
	//----------------------------------------------------------------------------------------------------------------------------------//

	logFile.open("log.txt", ios::out);
	logFile << "# ----------------------------------------------------------------------------------------------------------------------------------\n"
				"# Skeletal Animation Log\n"
				"# Based in Autodesk FBX SDK\n"
				"# Fredrik Linde TA15 2017\n"
				"# ----------------------------------------------------------------------------------------------------------------------------------\n\n";

	//----------------------------------------------------------------------------------------------------------------------------------//
	logFile << "# ----------------------------------------------------------------------------------------------------------------------------------\n";
	logFile << "BONE NAMES AND PARENT INDEX:\n";
	logFile << "Base file name: " << baseFilePath << "\n";
	logFile << "# ----------------------------------------------------------------------------------------------------------------------------------\n\n";

	logFile.close();

	//----------------------------------------------------------------------------------------------------------------------------------//
	// LOAD SKELETON (ONCE)
	//----------------------------------------------------------------------------------------------------------------------------------//
	
	LoadBaseFile(baseFilePath, gFbxSdkManager, pImporter, pFbxScene);

	// Get root node from the base file
	pFbxRootNode = pFbxScene->GetRootNode();

	// Get skeleton hierarchy from the base file
	LoadSkeletonHierarchy(pFbxRootNode);

	// Get mesh from base file
	currentMesh = GetMeshFromRoot(pFbxRootNode);

	// Process control points of the mesh
	ProcessControlPoints();

	//----------------------------------------------------------------------------------------------------------------------------------//
	// LOAD ANIMATIONS (ONCE PER FILE)
	//----------------------------------------------------------------------------------------------------------------------------------//

	for (int i = 0; i < ANIMATIONCOUNT; i++){

		if (i == 0) {

			currentFilePath = "FbxModel\\walk.fbx";
			hr = LoadAnimation(currentFilePath, gFbxSdkManager, pImporter, pFbxScene);

			if (FAILED(hr)) {

				cout << currentFilePath << " wasn't found" << endl;
			}
			
			pFbxRootNode = pFbxScene->GetRootNode();

		}

		if (i == 1) {

			currentFilePath = "FbxModel\\wave.fbx";
			hr = LoadAnimation(currentFilePath, gFbxSdkManager, pImporter, pFbxScene);

			if (FAILED(hr)) {

				cout << currentFilePath << " wasn't found" << endl;
			}

			pFbxRootNode = pFbxScene->GetRootNode();

		}

		if (i == 2) {

			currentFilePath = "FbxModel\\stagger.fbx";
			hr = LoadAnimation(currentFilePath, gFbxSdkManager, pImporter, pFbxScene);

			if (FAILED(hr)) {

				cout << currentFilePath << " wasn't found" << endl;
			}

			pFbxRootNode = pFbxScene->GetRootNode();

		}

		GatherAnimationData(pFbxRootNode, pFbxScene, i);

	}

	/*logFile.open("log.txt", ios::out | ios::app);

	logFile << "# ----------------------------------------------------------------------------------------------------------------------------------\n";
	logFile << "VERTEX WEIGHTS:\n";
	logFile << "# ----------------------------------------------------------------------------------------------------------------------------------\n\n";

	logFile.close();

	CreateVertexData(pFbxRootNode, pOutVertexVector);

	logFile.open("log.txt", ios::out | ios::app);

	logFile << "# ----------------------------------------------------------------------------------------------------------------------------------\n";
	logFile << "BIND POSE MATRICES:\n";
	logFile << "# ----------------------------------------------------------------------------------------------------------------------------------\n\n";

	logFile.close();*/

	CreateVertexData(pFbxRootNode, pOutVertexVector);

	InitializeAnimation();

	pImporter->Destroy();
	gFbxSdkManager->Destroy();

	hr = TRUE;
	return hr;
}

void FbxImport::LoadSkeletonHierarchy(FbxNode* rootNode) {

	ofstream logFile;
	logFile.open("log.txt", ios::out | ios::app);

	// Hierarchy depth variable is used for debugging purposes to track the depth of our tree

	meshSkeleton.hierarchyDepth = 0;	// This variable is only here for debugging purposes to track the depth of the recursive search

	for (int subNodeIndex = 0; subNodeIndex < rootNode->GetChildCount(); subNodeIndex++) // loops trough all joints in node
	{

		FbxNode* currentChildNode = rootNode->GetChild(subNodeIndex);	// Get current node in the file
		RecursiveDepthFirstSearch(currentChildNode, meshSkeleton.hierarchyDepth, 0, -1);	// Skeleton root node should be labeled with an index of -1
		
	}

	logFile << "\n";
	logFile.close();
}

void FbxImport::RecursiveDepthFirstSearch(FbxNode* node, int depth, int index, int parentIndex) {

	ofstream logFile;
	logFile.open("log.txt", ios::out | ios::app);

	// Recurvise depth first search function will first control that the actual node is a valid skeleton node

	if (node->GetNodeAttribute() && node->GetNodeAttribute()->GetAttributeType() && node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton){
	
	// A "joint" object is created for every valid skeleton node in which its parent index and name is stored

	Joint currentJoint;
	currentJoint.ParentIndex = parentIndex;
	currentJoint.Name = node->GetName();
	logFile << currentJoint.Name.c_str() << "   " << currentJoint.ParentIndex << "\n";
	meshSkeleton.hierarchy.push_back(currentJoint);

	}

	// Function is called again to traverse the hierarchy, if there is any, underneath this node

	for (int i = 0; i < node->GetChildCount(); i++) {

		RecursiveDepthFirstSearch(node->GetChild(i), depth + 1, meshSkeleton.hierarchy.size(), index);
	}

	logFile.close();
}

void FbxImport::CreateVertexData(FbxNode* pFbxRootNode, vector<Vertex_Bone>* pOutVertexVector) {

	if (pFbxRootNode) {

		// Data parsing file declaration
		ofstream logFile;
		logFile.open("log.txt", ios::out | ios::app);
		int vertexCounter = 0;

		FbxMesh* currentMesh;

		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++) {

			currentMesh = GetMeshFromRoot(pFbxRootNode);
			FbxVector4* pVertices = currentMesh->GetControlPoints();

			for (int j = 0; j < currentMesh->GetPolygonCount(); j++) {

				int iNumVertices = currentMesh->GetPolygonSize(j);	// Retreive the size of every polygon which should be represented as a triangle
				assert(iNumVertices == 3);	// Reassure that every polygon is a triangle and if not, don't allow the user to pass this point

				for (int k = 0; k < iNumVertices; k++) {	// Process every vertex in the triangle

					int iControlPointIndex = currentMesh->GetPolygonVertex(j, k);	// Retrieve the vertex index to know which control point in the vector to use
					ControlPoint* currentControlPoint = controlPoints[iControlPointIndex];
					
					Vertex_Bone vertex;
					vertex.pos = currentControlPoint->Position;	// Initialize the vertex position from the corresponding control point in the vector
					
					FbxVector2 FBXTexcoord;	
					bool unmapped;
					iControlPointIndex = currentMesh->GetPolygonVertexUV(j, k, "map1", FBXTexcoord, unmapped);	// Initialize texture coordinates to store in the output vertex

					vertex.uv.x = (float)FBXTexcoord.mData[0];
					vertex.uv.y = (float)FBXTexcoord.mData[1];
					vertex.uv.y = 1 - vertex.uv.y;

					FbxVector4 FBXNormal;

					iControlPointIndex = currentMesh->GetPolygonVertexNormal(j, k, FBXNormal); // Initialize normals to store in the output vertex

					vertex.normal.x = (float)FBXNormal.mData[0];
					vertex.normal.y = (float)FBXNormal.mData[1];
					vertex.normal.z = (float)FBXNormal.mData[2];

					// Retreive Blending Weight info for each vertex in the mesh
					// Every vertex must have three weights and four influencing bone indices

					logFile << "----------------\n" << "Vertex " << vertexCounter << "\n----------------" << endl;

					vertex.weights[0] = 0.0f;
					vertex.weights[1] = 0.0f;
					vertex.weights[2] = 0.0f;
					vertex.weights[3] = 0.0f;

					for(unsigned int i = 0; i < currentControlPoint->BlendingInfo.size(); i++){

						VertexBlendInfo currentBlendingInfo;
						currentBlendingInfo.BlendingIndex = currentControlPoint->BlendingInfo[i].BlendIndex;
						currentBlendingInfo.BlendingWeight = currentControlPoint->BlendingInfo[i].BlendWeight;
						logFile << currentControlPoint->BlendingInfo[i].BlendWeight << "  " << currentControlPoint->BlendingInfo[i].BlendIndex << "\n";
					
						// Store weight pairs in a separate blending weight vector

						vertex.boneIndices[i] = currentControlPoint->BlendingInfo[i].BlendIndex;
						vertex.weights[i] = currentControlPoint->BlendingInfo[i].BlendWeight;
					
						vertexBlend.VertexBlendingInfoList.push_back(currentBlendingInfo);

					}

					logFile << "\n";

					// Alternatively, the weights should be sorted by weight but this affects compilation time. Use optimized search sort for this type of operation
					//vertexBlend.SortBlendingInfoByWeight();

					vertices.push_back(vertex);	// Store all vertices in a separate vector

					indices.push_back(vertexCounter);	// Store indices so that vertices doesn't have to be loaded twice into the pipeline

					// Copy over the data for the 3 vertices that make up the triangle into the pOutVertexVector
					
					pOutVertexVector->push_back(vertex);

					vertexCounter++;
				}
			}

		}

		int size = controlPoints.size();

		logFile.close();
	}
}

void FbxImport::GatherAnimationData(FbxNode* node, FbxScene* scene, int animIndex) {

	FbxMesh* mesh = GetMeshFromRoot(node);
	unsigned int deformerCount = mesh->GetDeformerCount();	// A deformer is associated with manipulating geometry through clusters, which are the joints we're after

	FbxAMatrix geometryTransform = GetGeometryTransformation(node); // Geometric offset must be taken into account, even though it's often an identity matrix

	for (unsigned int deformerIndex = 0; deformerIndex < deformerCount; deformerIndex++) {

		// To reach the link to the joint, we must go through a skin node containing the skinning data holding vertex weights from the binded mesh

		FbxSkin* currentSkin = reinterpret_cast<FbxSkin*>(mesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

		if (!currentSkin) {

			continue;
		}

		unsigned int clusterCount = currentSkin->GetClusterCount();	// Every joint is technically a deformer, so we must process through each one in the hierarchy

		for (unsigned int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) {

			FbxCluster* currentCluster = currentSkin->GetCluster(clusterIndex); // Current joint being processed in the hierarchy
			std::string currentJointName = currentCluster->GetLink()->GetName();	// Here is the direct link to the joint required to retrieve its name and other attributes
			unsigned int currentJointIndex = FindJointIndexByName(currentJointName, meshSkeleton);	// Call to function to retrieve joint index from skeleton hierarchy

			// Declarations of the required matrices needed to create the Global Bind Pose Inverse matrix for every joint

			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindPoseInverseMatrix;

			currentCluster->GetTransformMatrix(transformMatrix);	// This is the transformation of the mesh at bind time
			currentCluster->GetTransformLinkMatrix(transformLinkMatrix);	// The transformation of the cluster (in our case the joint) at binding time from local space to world space
			globalBindPoseInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			ConvertToLeftHanded(transformMatrix);
			ConvertToLeftHanded(transformLinkMatrix);
			ConvertToLeftHanded(globalBindPoseInverseMatrix);

			// Next we must update the matrices in the skeleton hierarchy 
			meshSkeleton.hierarchy[currentJointIndex].TransformMatrix = transformMatrix;
			meshSkeleton.hierarchy[currentJointIndex].TransformLinkMatrix = transformLinkMatrix;
			meshSkeleton.hierarchy[currentJointIndex].GlobalBindposeInverse = globalBindPoseInverseMatrix;

			// KFbxNode was used in previous versions of the FBX SDK to receive the cluster link, but now we only use a normal FbxNode
			meshSkeleton.hierarchy[currentJointIndex].Node = currentCluster->GetLink();
			
			// Associate the joint with the control points it affects
			unsigned int indicesCount = currentCluster->GetControlPointIndicesCount();

			if( animIndex == 0){

			for (unsigned int i = 0; i < indicesCount; i++)
			{

				BlendingIndexWeightPair currentBlendPair;
				currentBlendPair.BlendIndex = currentJointIndex;
				currentBlendPair.BlendWeight = currentCluster->GetControlPointWeights()[i];
				controlPoints[currentCluster->GetControlPointIndices()[i]]->BlendingInfo.push_back(currentBlendPair);

			}

			}

			// Now we can start loading the animation data

			// Alternatively, we can define a ClassId condition and use it in the GetSrcObject function. I found it handy, so I kept this as a comment
			FbxCriteria animLayerCondition = FbxCriteria::ObjectTypeStrict(FbxAnimLayer::ClassId);
			//FbxAnimStack* currentAnimStack = FbxCast<FbxAnimStack>(scene->GetSrcObject(condition, 0));

			FbxAnimStack* currentAnimStack = scene->GetSrcObject<FbxAnimStack>(0);	// Retrieve the animation stack which holds the animation layers
			FbxString animStackName = currentAnimStack->GetName();	// Retrieve the name of the animation stack
			int numAnimLayers = currentAnimStack->GetMemberCount(animLayerCondition);
			FbxAnimLayer* animLayer = currentAnimStack->GetMember<FbxAnimLayer>(0);

			FbxTakeInfo* takeInformation = node->GetScene()->GetTakeInfo(animStackName);	// A take is a group of animation data grouped by name
			FbxTime startTime = takeInformation->mLocalTimeSpan.GetStart();	// Retrieve start time for the animation (either 0 or the user-specified beginning in the time-line)
			FbxTime endTime = takeInformation->mLocalTimeSpan.GetStop();	// Retrieve end time for the animation (often user specified or default )

			FbxLongLong animationLength = meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Length = endTime.GetFrameCount(FbxTime::eFrames24) - startTime.GetFrameCount(FbxTime::eFrames24) + 1;	// To receive the total animation length, just subtract the start time frame with end time frame

			meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence.resize(animationLength);
			
			for (FbxLongLong i = startTime.GetFrameCount(FbxTime::eFrames24); i <= animationLength - 1; i++) {

				
				FbxTime currentTime;
				currentTime.SetFrame(i, FbxTime::eFrames24);
				meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].TimePos = currentTime.GetFrameCount(FbxTime::eFrames24);

				FbxAMatrix currentTransformOffset = node->EvaluateGlobalTransform(currentTime) * geometryTransform;	// Receives global transformation at time t
				meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].GlobalTransform = currentTransformOffset.Inverse() * currentCluster->GetLink()->EvaluateGlobalTransform(currentTime);

				meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].Translation = XMFLOAT3(
					meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].GlobalTransform.GetT().mData[0],
					meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].GlobalTransform.GetT().mData[1],
					meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].GlobalTransform.GetT().mData[2]);

				meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].Scale = XMFLOAT3(
					meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].GlobalTransform.GetS().mData[0],
					meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].GlobalTransform.GetS().mData[1],
					meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].GlobalTransform.GetS().mData[2]);

				meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].RotationQuat = XMFLOAT4(
					meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].GlobalTransform.GetQ().mData[0],
					meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].GlobalTransform.GetQ().mData[1],
					meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].GlobalTransform.GetQ().mData[2],
					meshSkeleton.hierarchy[currentJointIndex].Animations[animIndex].Sequence[i].GlobalTransform.GetQ().mData[3]);

			}

		}
	}

}

void FbxImport::SetGlobalTransform() {

	meshSkeleton.hierarchy[0].TransformLinkMatrix = meshSkeleton.hierarchy[0].TransformLinkMatrix;

	for (int i = 1; i < meshSkeleton.hierarchy.size(); i++) {

		meshSkeleton.hierarchy[i].TransformLinkMatrix = meshSkeleton.hierarchy[meshSkeleton.hierarchy[i].ParentIndex].TransformLinkMatrix * meshSkeleton.hierarchy[i].TransformLinkMatrix;	// Global Transform
		meshSkeleton.hierarchy[i].GlobalBindposeInverse = meshSkeleton.hierarchy[i].TransformLinkMatrix.Inverse();	// Inverse Global Bind Pose
	}
}

void FbxImport::UpdateAnimation(ID3D11DeviceContext* gDeviceContext, int animIndex) {

	gDeviceContext->Map(gBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &boneMappedResource);
	VS_SKINNED_DATA* boneBufferPointer = (VS_SKINNED_DATA*)boneMappedResource.pData;

	for (int i = 0; i < meshSkeleton.hierarchy.size(); i++) {

		Interpolate(boneBufferPointer,i, gDeviceContext, animIndex); // check Interpolate function.
	}

	gDeviceContext->Unmap(gBoneBuffer, 0);

}

void FbxImport::Interpolate(VS_SKINNED_DATA* boneBufferPointer, int jointIndex, ID3D11DeviceContext* gDeviceContext, int animIndex) {

	// Animation has just started, so return the first keyframe

	FbxLongLong animationLength = meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Length;

	if (animTimePos <= meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[0].TimePos) //first keyframe
	{

		XMFLOAT4X4 M;

		XMVECTOR S = XMLoadFloat3(&meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[0].Scale);
		XMVECTOR T = XMLoadFloat3(&meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[0].Translation);
		XMVECTOR Q = XMLoadFloat4(&meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[0].RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, T));

		boneBufferPointer->gBoneTransform[jointIndex] = XMMatrixTranspose(XMLoadFloat4x4(&M)) * XMMatrixTranspose(invertedBindPose[jointIndex]);
	}

	// Animation has reached its end, so return the last keyframe

	else if (animTimePos >= meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength - 1].TimePos) // last keyframe
	{

		XMFLOAT4X4 M;

		XMVECTOR S = XMLoadFloat3(&meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength - 1].Scale);
		XMVECTOR P = XMLoadFloat3(&meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength - 1].Translation);
		XMVECTOR Q = XMLoadFloat4(&meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[animationLength - 1].RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

		boneBufferPointer->gBoneTransform[jointIndex] = XMMatrixTranspose(XMLoadFloat4x4(&M)) * XMMatrixTranspose(invertedBindPose[jointIndex]);
	}

	// Animation time is between two frames so they should be interpolated

	else 
	{

		XMFLOAT4X4 M;
		// I am using an int here to truncate the animation timepose to know which matrices I am interested about
		// Ex. if time is 1.2, the returning frame is 1.
		int currentFrameIndex = animTimePos;
		float kFirst = meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex].TimePos;
		float kLast = meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex + 1].TimePos;
		
		float interpolationProcent = (animTimePos - kFirst) / (kLast - kFirst);

		XMVECTOR kFirstScale = XMLoadFloat3(&meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex].Scale); // interpolating between the current keyframe and the comming keyframe.
		XMVECTOR kLastScale = XMLoadFloat3(&meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex + 1].Scale);

		XMVECTOR kFirstTranslation = XMLoadFloat3(&meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex].Translation);
		XMVECTOR kLastTranslation = XMLoadFloat3(&meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex + 1].Translation);

		XMVECTOR kFirstQuaternion = XMLoadFloat4(&meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex].RotationQuat);
		XMVECTOR kLastQuaternion = XMLoadFloat4(&meshSkeleton.hierarchy[jointIndex].Animations[animIndex].Sequence[currentFrameIndex + 1].RotationQuat);

		XMVECTOR S = XMVectorLerp(kFirstScale, kLastScale, interpolationProcent);
		XMVECTOR T = XMVectorLerp(kFirstTranslation, kLastTranslation, interpolationProcent);
		XMVECTOR Q = XMQuaternionSlerp(kFirstQuaternion, kLastQuaternion, interpolationProcent);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, T));
		
		boneBufferPointer->gBoneTransform[jointIndex] = XMMatrixTranspose(XMLoadFloat4x4(&M)) * XMMatrixTranspose(invertedBindPose[jointIndex]);

	}
	//HLSL->boneshaders->vertexshader
}

//----------------------------------------------------------------------------------------------------------------------------------//
// SECONDARY FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------//

void FbxImport::InitializeAnimation() {

	size_t hierarchySize = meshSkeleton.hierarchy.size();


}

FbxAMatrix FbxImport::GetGeometryTransformation(FbxNode* node) {

	// Geometric offset is to allow this offset to not inherit and propagate to children or its parents

	if (!node) {	// If the node is valid, continue processing the transformation

		throw std::exception("NULL for mesh geometry");

	}

	const FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(T, R, S);
}

XMMATRIX FbxImport::Load4X4JointTransformations(Joint joint, int transformIndex){	// Function to specifically convert joint transformations to XMFLOAT4X4

	ofstream logFile;
	logFile.open("log.txt", ios::out | ios::app);

	logFile << "---------------------------\n" << "gBoneTransform " << transformIndex << "\n---------------------------" << endl;

	XMFLOAT4X4 matrix;
	
	matrix.m[0][0] = joint.GlobalBindposeInverse.Get(0, 0);
	matrix.m[0][1] = joint.GlobalBindposeInverse.Get(0, 1);
	matrix.m[0][2] = joint.GlobalBindposeInverse.Get(0, 2);
	matrix.m[0][3] = joint.GlobalBindposeInverse.Get(0, 3);
	logFile << (int)matrix.m[0][0] << " " << (int)matrix.m[0][1] << " " << (int)matrix.m[0][2] << " " << (int)matrix.m[0][3] << "\n";

	matrix.m[1][0] = joint.GlobalBindposeInverse.Get(1, 0);
	matrix.m[1][1] = joint.GlobalBindposeInverse.Get(1, 1);
	matrix.m[1][2] = joint.GlobalBindposeInverse.Get(1, 2);
	matrix.m[1][3] = joint.GlobalBindposeInverse.Get(1, 3);
	logFile << (int)matrix.m[1][0] << " " << (int)matrix.m[1][1] << " " << (int)matrix.m[1][2] << " " << (int)matrix.m[1][3] << "\n";

	matrix.m[2][0] = joint.GlobalBindposeInverse.Get(2, 0);
	matrix.m[2][1] = joint.GlobalBindposeInverse.Get(2, 1);
	matrix.m[2][2] = joint.GlobalBindposeInverse.Get(2, 2);
	matrix.m[2][3] = joint.GlobalBindposeInverse.Get(2, 3);
	logFile << (int)matrix.m[2][0] << " " << (int)matrix.m[2][1] << " " << (int)matrix.m[2][2] << " " << (int)matrix.m[2][3] << "\n";

	matrix.m[3][0] = joint.GlobalBindposeInverse.Get(3, 0);
	matrix.m[3][1] = joint.GlobalBindposeInverse.Get(3, 1);
	matrix.m[3][2] = joint.GlobalBindposeInverse.Get(3, 2);
	matrix.m[3][3] = joint.GlobalBindposeInverse.Get(3, 3);
	logFile << (int)matrix.m[3][0] << " " << (int)matrix.m[3][1] << " " << (int)matrix.m[3][2] << " " << (int)matrix.m[3][3] << "\n";
	
	XMMATRIX converted = XMLoadFloat4x4(&matrix);

	logFile << "\n";
	logFile.clear();

	return converted;
}

XMMATRIX FbxImport::Load4X4Transformations(FbxAMatrix fbxMatrix) {

	XMFLOAT4X4 matrix;

	matrix.m[0][0] = fbxMatrix.Get(0, 0);
	matrix.m[0][1] = fbxMatrix.Get(0, 1);
	matrix.m[0][2] = fbxMatrix.Get(0, 2);
	matrix.m[0][3] = fbxMatrix.Get(0, 3);

	matrix.m[1][0] = fbxMatrix.Get(1, 0);
	matrix.m[1][1] = fbxMatrix.Get(1, 1);
	matrix.m[1][2] = fbxMatrix.Get(1, 2);
	matrix.m[1][3] = fbxMatrix.Get(1, 3);

	matrix.m[2][0] = fbxMatrix.Get(2, 0);
	matrix.m[2][1] = fbxMatrix.Get(2, 1);
	matrix.m[2][2] = fbxMatrix.Get(2, 2);
	matrix.m[2][3] = fbxMatrix.Get(2, 3);

	matrix.m[3][0] = fbxMatrix.Get(3, 0);
	matrix.m[3][1] = fbxMatrix.Get(3, 1);
	matrix.m[3][2] = fbxMatrix.Get(3, 2);
	matrix.m[3][3] = fbxMatrix.Get(3, 3);
	
	XMMATRIX converted = XMLoadFloat4x4(&matrix);

	return converted;
}

unsigned int FbxImport::FindJointIndexByName(string& jointName, Skeleton skeleton) {

	for (unsigned int i = 0; i < skeleton.hierarchy.size(); i++) {

		if (skeleton.hierarchy[i].Name == jointName) {	// It's possible to compare a constant character with a string, which is the case here

			return i;
		}
	}

	// If Skeleton information can't be read, inform the user of the problem

	throw std::exception("Skeleton information in FBX file cannot be received and might be corrupt");
}

FbxMesh* FbxImport::GetMeshFromRoot(FbxNode* node) {	// Function to receive a mesh from the root node

	FbxMesh* currentMesh;

	for (int i = 0; i < node->GetChildCount(); i++) {	// Get number of children nodes from the root node

		FbxNode* pFbxChildNode = node->GetChild(i);	// Current child being processed in the file

		if (pFbxChildNode->GetNodeAttribute() == NULL) {

			continue;
		}

		FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();	// Get the attribute type of the child node

		if (AttributeType != FbxNodeAttribute::eMesh) {	

			continue;
		}

		currentMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

	}

	return currentMesh;
}

void FbxImport::ProcessControlPoints() {	// Function to process every vertex in the mesh

	unsigned int controlPointCount = currentMesh->GetControlPointsCount();	// Store the total amount of control points

	// Loop through all vertices and create individual controlpoints that are store in the control points vector

	for (unsigned int i = 0; i < controlPointCount; i++) {

		ControlPoint* currentControlPoint = new ControlPoint();

		XMFLOAT3 position;
		position.x = static_cast<float>(currentMesh->GetControlPointAt(i).mData[0]);
		
		position.y = static_cast<float>(currentMesh->GetControlPointAt(i).mData[1]);
		
		position.z = static_cast<float>(currentMesh->GetControlPointAt(i).mData[2]);

		currentControlPoint->Position = position;
		controlPoints[i] = currentControlPoint;
	}

	
}

void FbxImport::ConvertToLeftHanded(FbxAMatrix &matrix) {

	FbxVector4 translation = matrix.GetT();
	FbxVector4 rotation = matrix.GetR();

	translation.Set(translation.mData[0], translation.mData[1], -translation.mData[2]);
	rotation.Set(-rotation.mData[0], -rotation.mData[1], rotation.mData[2]);

	matrix.SetT(translation);
	matrix.SetR(rotation);
}

HRESULT FbxImport::LoadBaseFile(const char* fileName, FbxManager* gFbxSdkManager, FbxImporter* pImporter, FbxScene* pScene) {
	
	bool bSuccess = pImporter->Initialize(fileName, -1, gFbxSdkManager->GetIOSettings());

	if (!bSuccess) {

		return E_FAIL;
	}

	bSuccess = pImporter->Import(pScene);

	if (!bSuccess) {

		return E_FAIL;
	}

	return true;
}

HRESULT FbxImport::LoadAnimation(const char* fileName, FbxManager* gFbxSdkManager, FbxImporter* pImporter, FbxScene* pScene) {

	bool bSuccess = pImporter->Initialize(fileName, -1, gFbxSdkManager->GetIOSettings());

	if (!bSuccess) {

		return E_FAIL;
	}

	bSuccess = pImporter->Import(pScene);

	if (!bSuccess) {

		return E_FAIL;
	}

	return true;
}