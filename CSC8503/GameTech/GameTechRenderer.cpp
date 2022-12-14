#include "GameTechRenderer.h"
#ifdef _x64
#include "../CSC8503Common/GameObject.h"
#include "../../Common/Camera.h"
#include "../../Common/NCLVector2.h"
#include "../../Common/NCLVector3.h"
#include "../../Common/TextureLoader.h"
#include "../../UI/GUI.h"
#include "..//..//Common/Assets.h"

#include "../../Common/stb/stb_image.h"

using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096

NCLMatrix4 biasMatrix = NCLMatrix4::Translation(NCLVector3(0.5, 0.5, 0.5)) * NCLMatrix4::Scale(NCLVector3(0.5, 0.5, 0.5));

GameTechRenderer::GameTechRenderer(GameWorld& w, D_GUI* UI, ResourceManager* rm)
	: OGLRenderer(*Window::GetWindow()), gameWorld(w), gameUI(UI) {
	//	glEnable(GL_DEPTH_TEST);
	resourceManager = (OGLResourceManager*)rm;

	sphere = (OGLMesh*) resourceManager->LoadMesh("sphere.msh");

	quad = OGLMesh::GenerateQuad();

	paintTex = (OGLTexture*)resourceManager->LoadTexture("paintSplat2.png");

	shadowShader = new OGLShader("GameTechShadowVert.glsl", "GameTechShadowFrag.glsl");

	GenerateShadowBuffer(shadowFBO);

	glClearColor(1, 1, 1, 1);

	//Set up the light properties
	lightColour = NCLVector4(0.8f, 0.8f, 0.5f, 1.0f);
	lightRadius = 1000.0f;
	//lightRadius = 350.0f;
	lightPosition = NCLVector3(-50.0f, 50.0f, 200.0f);


	sceneShader = new OGLShader("GameTechVert.glsl", "bufferFragment.glsl");
	decalShader = new OGLShader("decalVert.glsl", "decalFrag.glsl");
	pointLightShader = new OGLShader("pointlightvertex.glsl", "pointlightfrag.glsl");
	combineShader = new OGLShader("combinevert.glsl", "combinefrag.glsl");

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	glGenFramebuffers(1, &decalFBO);
	sceneBuffers.push_back(bufferFBO);
	sceneBuffers.push_back(pointLightFBO);
	sceneBuffers.push_back(decalFBO);


	GLenum buffers[2] = {
		GL_COLOR_ATTACHMENT0 ,
		GL_COLOR_ATTACHMENT1
	};

	GLenum buffers2[3] = {
		GL_COLOR_ATTACHMENT0 ,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};

	GLenum decalBuffer[1] = { GL_COLOR_ATTACHMENT0 };

	//Skybox!
	skyboxShader = new OGLShader("skyboxVertex.glsl", "skyboxFragment.glsl");
	skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({ NCLVector3(-1, 1,-1), NCLVector3(-1,-1,-1) , NCLVector3(1,-1,-1) , NCLVector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();
	GenerateScreenTexture(bufferFinalTex);

	LoadSkybox();
	LoadPrinter();
	LoadNavMesh();
	LoadNavMesh02();
	LoadStartImage();

	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(bufferShadowTex);
	GenerateScreenTexture(decalScoreTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);

	// And now attach them to our FBOs
	// Binding our attachment textures to their respective FBO's, firstly the first FBO 
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, bufferShadowTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(3, buffers2);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)  return;

	glBindFramebuffer(GL_FRAMEBUFFER, decalFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, decalScoreTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)  return;

	// Setting up second FBO attachments
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	loading = true;
}

GameTechRenderer::~GameTechRenderer() {
	for (GLuint buffer : sceneBuffers) {
		glDeleteFramebuffers(1, &buffer);
	}
	for (GLuint tex : sceneTextures) {
		glDeleteTextures(1, &tex);
	}
}

void GameTechRenderer::LoadPrinter() {
	print_shader = new OGLShader("PrinterVertex.glsl", "PrinterFragment.glsl");
	split_shader = new OGLShader("SplitVertex.glsl", "PrinterFragment.glsl");
	printer = OGLMesh::GenerateQuad();

	glGenFramebuffers(1, &printFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, printFBO);
	glGenTextures(1, &print_depth_Tex);
	glBindTexture(GL_TEXTURE_2D, print_depth_Tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, currentWidth, currentHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, print_depth_Tex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, print_depth_Tex, 0);
	sceneTextures.push_back(print_depth_Tex);
	sceneBuffers.push_back(printFBO);

	//*************************************************//
	glGenTextures(1, &print_Tex);
	glBindTexture(GL_TEXTURE_2D, print_Tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, currentWidth, currentHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferFinalTex, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	sceneTextures.push_back(print_Tex);
}


void GameTechRenderer::LoadSkybox() {
	string filenames[6] = {
		"/Cubemap/skyrender0004.png",
		"/Cubemap/skyrender0001.png",
		"/Cubemap/skyrender0003.png",
		"/Cubemap/skyrender0006.png",
		"/Cubemap/skyrender0002.png",
		"/Cubemap/skyrender0005.png"
	};

	int width[6] = { 0 };
	int height[6] = { 0 };
	int channels[6] = { 0 };
	int flags[6] = { 0 };

	vector<char*> texData(6, nullptr);

	for (int i = 0; i < 6; ++i) {
		TextureLoader::LoadTexture(filenames[i], texData[i], width[i], height[i], channels[i], flags[i]);
		if (i > 0 && (width[i] != width[0] || height[0] != height[0])) {
			std::cout << __FUNCTION__ << " cubemap input textures don't match in size?\n";
			return;
		}
	}
	glGenTextures(1, &skyboxTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	GLenum type = channels[0] == 4 ? GL_RGBA : GL_RGB;

	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width[i], height[i], 0, type, GL_UNSIGNED_BYTE, texData[i]);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	sceneTextures.push_back(skyboxTex);
	//skybox fbo
	glGenFramebuffers(1, &depthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	GenerateScreenTexture(buffer_colour_depth_tex);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer_colour_depth_tex, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenFramebuffers(1, &skyboxFBO);
	GenerateScreenTexture(skybox_tex);
	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, skybox_tex, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	sceneBuffers.push_back(depthFBO);
	sceneBuffers.push_back(skyboxFBO);
}

void GameTechRenderer::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, currentWidth, currentHeight, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
	sceneTextures.push_back(into);
	screenTextures.push_back(into);
}

void GameTechRenderer::GenerateShadowBuffer(GLuint& into) {
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);
	sceneTextures.push_back(shadowTex);

	glGenFramebuffers(1, &into);
	glBindFramebuffer(GL_FRAMEBUFFER, into);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	sceneBuffers.push_back(into);
}

void GameTechRenderer::ResizeSceneTextures(float width, float height) {
	glBindTexture(GL_TEXTURE_2D, print_depth_Tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, currentWidth, currentHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, print_Tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, currentWidth, currentHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	for (auto tex : screenTextures) {
		if (tex != bufferDepthTex) {
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, currentWidth, currentHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, currentWidth, currentHeight, GL_DEPTH_COMPONENT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);



}

void GameTechRenderer::RenderFrame() {
	glEnable(GL_CULL_FACE);
	glClearColor(1, 1, 1, 1);
	BuildObjectList(gameWorld.GetMainCamera());
	SortObjectList();

	RenderShadowMap();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_BLEND);
	FillBuffers(gameWorld.GetMainCamera(), 0.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_BLEND);
	FillBuffers(gameWorld.GetMainCamera(), 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	RenderSkybox(gameWorld.GetMainCamera());

	DrawPaintDecals(gameWorld.GetMainCamera());
	DrawPointLights(gameWorld.GetMainCamera());
	CombineBuffers(gameWorld.GetMainCamera());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	inSplitScreen ? SplitRender() : PresentScene(false, 0);

	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...

	gameUI->UI_Render();
}

void GameTechRenderer::FillBuffers(Camera* current_camera, float depth) {
	BindShader(sceneShader);

	float screenAspect = (float)currentWidth / (float)currentHeight;
	NCLMatrix4 viewMatrix = current_camera->BuildViewMatrix();
	NCLMatrix4 projMatrix = current_camera->BuildProjectionMatrix(screenAspect);

	OGLShader* activeShader = nullptr;
	int projLocation = 0;
	int viewLocation = 0;
	int modelLocation = 0;
	int colourLocation = 0;
	int hasVColLocation = 0;
	int hasTexLocation = 0;
	int hasBumpLocation = 0;
	int shadowLocation = 0;

	int cameraLocation = 0;
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	for (const auto& i : activeObjects) {
		OGLShader* shader = (OGLShader*)(*i).GetShader();
		BindShader(shader);

		vector<TextureBase*> textures = (*i).GetTextures();

		if (activeShader != shader) {
			projLocation = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
			viewLocation = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
			modelLocation = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
			shadowLocation = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
			colourLocation = glGetUniformLocation(shader->GetProgramID(), "objectColour");
			hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
			hasTexLocation = glGetUniformLocation(shader->GetProgramID(), "hasTexture");
			hasBumpLocation = glGetUniformLocation(shader->GetProgramID(), "hasBump");

			cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
			glUniform3fv(cameraLocation, 1, (float*)&current_camera->GetPosition());

			glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

			int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
			glUniform1i(shadowTexLocation, 2);

			activeShader = shader;
		}

		NCLMatrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);

		NCLMatrix4 fullShadowMat = shadowMatrix * modelMatrix;
		glUniformMatrix4fv(shadowLocation, 1, false, (float*)&fullShadowMat);

		glUniform4fv(colourLocation, 1, (float*)&i->GetColour());

		glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() ? 1 : 0);
		bool hasDiff = (OGLTexture*)(*i).GetDefaultTexture() ? true : false;
		bool hasBump = textures.size() == layerCount * 2;
		glUniform1i(hasBumpLocation, hasBump);

		glUniform1f(glGetUniformLocation(shader->GetProgramID(), "isDepth"), depth);

		if (i->GetAnimation()) {
			MeshGeometry* mesh = i->GetMesh();
			vector <NCLMatrix4> frameMatrices;
			const vector<NCLMatrix4> invBindPose = mesh->GetInverseBindPose();
			const NCLMatrix4* frameData = i->GetAnimation()->GetJointData(i->GetCurrentFrame());

			for (unsigned int i = 0; i < mesh->GetJointCount(); ++i) {
				auto matrix = invBindPose[i];
				frameMatrices.emplace_back(frameData[i] * matrix);
			}

			int j = glGetUniformLocation(((OGLShader*)(*i).GetShader())->GetProgramID(), "joints");
			glUniformMatrix4fv(j, frameMatrices.size(), false,
				(float*)frameMatrices.data());

		}

		BindMesh((*i).GetMesh());
		int activeDiffuse = -1;
		int activeBump = -1;
		for (int i = 0; i < layerCount; ++i) {
			if (hasDiff && ((OGLTexture*)textures[i])->GetObjectID() != activeDiffuse) {
				BindTextureToShader((OGLTexture*)textures[i], "mainTex", 0);
				activeDiffuse = ((OGLTexture*)textures[i])->GetObjectID();
			}
			if (hasBump && ((OGLTexture*)textures[i + layerCount])->GetObjectID() != activeDiffuse) {
				BindTextureToShader((OGLTexture*)textures[i + layerCount], "bumpTex", 1);
				activeBump = ((OGLTexture*)textures[i + layerCount])->GetObjectID();
			}
			DrawBoundMesh(i);
		}

	}

	//Debug Information
	if (ShowNavMesh) {
		RenderNavMesh(gameWorld.GetMainCamera());
		RenderNavMesh02(gameWorld.GetMainCamera());
	}

	glDisable(GL_CULL_FACE);
	DrawDebugData(true);
	glEnable(GL_CULL_FACE);
}

// Reopen first buffer?
void GameTechRenderer::DrawPaintDecals(Camera* current_camera) {
	glBindFramebuffer(GL_FRAMEBUFFER, decalFBO);
	glDepthMask(GL_FALSE);

	BindShader(decalShader);

	glUniform1i(glGetUniformLocation(decalShader->GetProgramID(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(decalShader->GetProgramID(), "normalTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	float screenAspect = (float)currentWidth / (float)currentHeight;
	NCLMatrix4 viewMatrix = current_camera->BuildViewMatrix();
	NCLMatrix4 projMatrix = current_camera->BuildProjectionMatrix(screenAspect);

	int cameraLocation = glGetUniformLocation(decalShader->GetProgramID(), "cameraPos");
	glUniform3fv(cameraLocation, 1, (float*)&current_camera->GetPosition());

	int projLocation = glGetUniformLocation(decalShader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(decalShader->GetProgramID(), "viewMatrix");
	int modelLocation = glGetUniformLocation(decalShader->GetProgramID(), "modelMatrix");

	NCLMatrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(decalShader->GetProgramID(), "inverseProjView"), 1, false, invViewProj.array);
	glUniform1f(glGetUniformLocation(decalShader->GetProgramID(), "aspect"), screenAspect);
	glUniform2f(glGetUniformLocation(decalShader->GetProgramID(), "pixelSize"), 1.0f / currentWidth, 1.0f / currentHeight);
	glUniform1f(glGetUniformLocation(decalShader->GetProgramID(), "far"), current_camera->GetFarPlane());

	int colourLocation = glGetUniformLocation(decalShader->GetProgramID(), "objectColour");

	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	int hasTexLocation = glGetUniformLocation(decalShader->GetProgramID(), "hasTexture");
	int hasBumpLocation = glGetUniformLocation(decalShader->GetProgramID(), "hasBump");
	glUniform1i(hasTexLocation, 1);
	glUniform1i(hasBumpLocation, 0);

	for (const auto& d : decalsToBeGenerated) {
		RenderObject* r = d->GetRenderObject();
		BindMesh((*r).GetMesh());

		glUniformMatrix4fv(modelLocation, 1, false, (float*)&d->GetTransform().GetMatrix());
		colourLocation = glGetUniformLocation(decalShader->GetProgramID(), "objectColour");
		glUniform4fv(colourLocation, 1, (float*)&r->GetColour());
		int invModelLocation = glGetUniformLocation(decalShader->GetProgramID(), "inverseModel");
		NCLMatrix4 invModelMat = (*r).GetTransform()->GetMatrix().Inverse();
		glUniformMatrix4fv(invModelLocation, 1, false, invModelMat.array);

		NCLMatrix4 modelMatrix = (*r).GetTransform()->GetMatrix();
		glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);

		int layerCount = (*r).GetMesh()->GetSubMeshCount();
		BindTextureToShader(paintTex, "mainTex", 1);
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}

	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::DrawPointLights(Camera* current_camera) {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	NCLMatrix4 viewMatrix = current_camera->BuildViewMatrix();
	NCLMatrix4 projMatrix = current_camera->BuildProjectionMatrix(screenAspect);


	/** Draw Point Lights **/
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(pointLightShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	NCLVector3 pos = current_camera->GetPosition();
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(pointLightShader->GetProgramID(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(pointLightShader->GetProgramID(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform1i(glGetUniformLocation(pointLightShader->GetProgramID(), "shadowTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferShadowTex);

	glUniform3fv(glGetUniformLocation(pointLightShader->GetProgramID(), "cameraPos"), 1, (float*)&current_camera->GetPosition());
	//std::cout << current_camera->GetPosition() << std::endl;

	glUniform2f(glGetUniformLocation(pointLightShader->GetProgramID(), "pixelSize"), 1.0f / currentWidth, 1.0f / currentHeight);

	NCLMatrix4 invViewProj = (projMatrix * viewMatrix).Inverse();

	int projLocation = glGetUniformLocation(pointLightShader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(pointLightShader->GetProgramID(), "viewMatrix");
	int modelLocation = glGetUniformLocation(pointLightShader->GetProgramID(), "modelMatrix");
	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	glUniformMatrix4fv(glGetUniformLocation(pointLightShader->GetProgramID(), "inverseProjView"), 1, false, invViewProj.array);

	glUniform3fv(glGetUniformLocation(pointLightShader->GetProgramID(), "lightPos"), 1, (float*)&lightPosition);

	glUniform4fv(glGetUniformLocation(pointLightShader->GetProgramID(), "lightColour"), 1, (float*)&lightColour);

	glUniform1f(glGetUniformLocation(pointLightShader->GetProgramID(), "lightRadius"), lightRadius);

	BindMesh(sphere);
	DrawBoundMesh();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::CombineBuffers(Camera* current) {

	BindShader(combineShader);
	float screenAspect = (float)currentWidth / (float)currentHeight;
	NCLMatrix4 viewMatrix = current->BuildViewMatrix();
	NCLMatrix4 projMatrix = current->BuildProjectionMatrix(screenAspect);
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	NCLMatrix4 identity = NCLMatrix4();
	int projLocation = glGetUniformLocation(combineShader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(combineShader->GetProgramID(), "viewMatrix");
	int modelLocation = glGetUniformLocation(combineShader->GetProgramID(), "modelMatrix");
	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(modelLocation, 1, false, (float*)&identity);
	glUniform1i(glGetUniformLocation(combineShader->GetProgramID(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgramID(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgramID(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgramID(), "skyboxTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, skybox_tex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgramID(), "scene_depth"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, buffer_colour_depth_tex);

	quad->SetPrimitiveType(GeometryPrimitive::TriangleStrip);
	glBindFramebuffer(GL_FRAMEBUFFER, printFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	BindMesh(quad);
	DrawBoundMesh();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void GameTechRenderer::SplitRender() {
	BuildObjectList(gameWorld.GetViceCamera());
	SortObjectList();

	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, currentWidth / 2, currentHeight);
	PresentScene(true, -0.5);

	glDisable(GL_SCISSOR_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_BLEND);
	FillBuffers(gameWorld.GetViceCamera(), 0.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_BLEND);
	FillBuffers(gameWorld.GetViceCamera(), 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	RenderSkybox(gameWorld.GetViceCamera());

	DrawPaintDecals(gameWorld.GetViceCamera());
	DrawPointLights(gameWorld.GetViceCamera());
	CombineBuffers(gameWorld.GetViceCamera());


	glEnable(GL_SCISSOR_TEST);
	glScissor(currentWidth / 2, 0, currentWidth / 2, currentHeight);
	PresentScene(true, 0.5);
	glDisable(GL_SCISSOR_TEST);
}


void GameTechRenderer::PresentScene(bool split, GLfloat offset) {
	OGLShader* activeShader = nullptr;
	if (split) {
		BindShader(split_shader);
		int offsetLocation = glGetUniformLocation(split_shader->GetProgramID(), "offset");
		glUniform1fv(offsetLocation, 1, (float*)&offset);
		activeShader = split_shader;
	}
	else {
		BindShader(print_shader);
		activeShader = print_shader;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferFinalTex);
	glUniform1i(glGetUniformLocation(activeShader->GetProgramID(), "diffuseTex"), 0);

	BindMesh(quad);
	DrawBoundMesh();
}


void GameTechRenderer::BeginFrame() {
	gameUI->UI_Frame();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	BindShader(nullptr);
	BindMesh(nullptr);
}

void GameTechRenderer::BuildObjectList(Camera* currentCamera) {
	activeObjects.clear();
	decalsToBeGenerated.clear();
	frameFrustum.FromMatrix(currentCamera->BuildProjectionMatrix((float)currentWidth / (float)currentHeight) * currentCamera->BuildViewMatrix());

	gameWorld.OperateOnContents(
		[&](GameObject* o) {
			if (o->IsActive()) {
				RenderObject* g = o->GetRenderObject();
				if (g && frameFrustum.InsideFrustum(*g)) {
					NCLVector3 dir = g->GetTransform()->GetPosition() - currentCamera->GetPosition();
					g->SetCameraDistance(dir.LengthSquared());
					if (o->GOisPaintPlacingCube()) {
						decalsToBeGenerated.push_back(o);
					}
					else {
						activeObjects.emplace_back(g);
					}
				}
			}
		}
	);
}

void GameTechRenderer::SortObjectList() {
	std::sort(activeObjects.begin(),
		activeObjects.end(),
		RenderObject::CompareByCameraDistance);
}

void GameTechRenderer::RenderShadowMap() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glCullFace(GL_FRONT);

	BindShader(shadowShader);
	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");
	int hasJointsLocation = glGetUniformLocation(shadowShader->GetProgramID(), "hasJoints");

	NCLMatrix4 shadowViewMatrix = NCLMatrix4::BuildViewMatrix(lightPosition, NCLVector3(0, 0, 0), NCLVector3(0, 1, 0));
	NCLMatrix4 shadowProjMatrix = NCLMatrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

	NCLMatrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

	shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

	for (const auto& i : activeObjects) {
		if (i->GetAnimation()) {
			MeshGeometry* mesh = i->GetMesh();
			vector <NCLMatrix4> frameMatrices;
			const vector<NCLMatrix4> invBindPose = mesh->GetInverseBindPose();
			const NCLMatrix4* frameData = i->GetAnimation()->GetJointData(i->GetCurrentFrame());

			for (unsigned int i = 0; i < mesh->GetJointCount(); ++i) {
				frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
			}

			glUniformMatrix4fv(glGetUniformLocation(shadowShader->GetProgramID(), "joints"), frameMatrices.size(), false,
				(float*)frameMatrices.data());

			glUniform1i(hasJointsLocation, true);
		}
		else {
			glUniform1i(hasJointsLocation, false);
		}

		NCLMatrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		NCLMatrix4 mvpMatrix = mvMatrix * modelMatrix;
		glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
		BindMesh((*i).GetMesh());
		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		for (int i = 0; i < layerCount; ++i) {
			DrawBoundMesh(i);
		}
	}

	glViewport(0, 0, currentWidth, currentHeight);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);
}

void GameTechRenderer::RenderSkybox(Camera* current_camera) {

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	float screenAspect = (float)currentWidth / (float)currentHeight;
	NCLMatrix4 viewMatrix = current_camera->BuildViewMatrix();
	NCLMatrix4 projMatrix = current_camera->BuildProjectionMatrix(screenAspect);

	BindShader(skyboxShader);

	int projLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "viewMatrix");
	int texLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "cubeTex");

	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	BindMesh(skyboxMesh);
	DrawBoundMesh();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::RenderCamera(Camera* current_camera) {

	float screenAspect = (float)currentWidth / (float)currentHeight;
	NCLMatrix4 viewMatrix = current_camera->BuildViewMatrix();
	NCLMatrix4 projMatrix = current_camera->BuildProjectionMatrix(screenAspect);

	OGLShader* activeShader = nullptr;
	int projLocation = 0;
	int viewLocation = 0;
	int modelLocation = 0;
	int colourLocation = 0;
	int hasVColLocation = 0;
	int hasTexLocation = 0;
	int hasBumpLocation = 0;
	int shadowLocation = 0;

	int lightPosLocation = 0;
	int lightColourLocation = 0;
	int lightRadiusLocation = 0;

	int cameraLocation = 0;
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	for (const auto& i : activeObjects) {
		OGLShader* shader = (OGLShader*)(*i).GetShader();
		BindShader(shader);

		vector<TextureBase*> textures = (*i).GetTextures();

		/*if (textures.size() > 1) {
			BindTextureToShader((OGLTexture*)textures[1], "bumpTex", 2);
		}*/

		if (activeShader != shader) {
			projLocation = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
			viewLocation = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
			modelLocation = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
			shadowLocation = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
			colourLocation = glGetUniformLocation(shader->GetProgramID(), "objectColour");
			hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
			hasTexLocation = glGetUniformLocation(shader->GetProgramID(), "hasTexture");
			hasBumpLocation = glGetUniformLocation(shader->GetProgramID(), "hasBump");

			lightPosLocation = glGetUniformLocation(shader->GetProgramID(), "lightPos");
			lightColourLocation = glGetUniformLocation(shader->GetProgramID(), "lightColour");
			lightRadiusLocation = glGetUniformLocation(shader->GetProgramID(), "lightRadius");

			cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
			glUniform3fv(cameraLocation, 1, (float*)&current_camera->GetPosition());

			glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

			glUniform3fv(lightPosLocation, 1, (float*)&lightPosition);
			glUniform4fv(lightColourLocation, 1, (float*)&lightColour);
			glUniform1f(lightRadiusLocation, lightRadius);

			int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
			glUniform1i(shadowTexLocation, 1);

			activeShader = shader;
		}

		NCLMatrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);

		NCLMatrix4 fullShadowMat = shadowMatrix * modelMatrix;
		glUniformMatrix4fv(shadowLocation, 1, false, (float*)&fullShadowMat);

		glUniform4fv(colourLocation, 1, (float*)&i->GetColour());

		glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

		int layerCount = (*i).GetMesh()->GetSubMeshCount();
		glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() ? 1 : 0);
		bool hasDiff = (OGLTexture*)(*i).GetDefaultTexture() ? true : false;
		bool hasBump = textures.size() == layerCount * 2;
		glUniform1i(hasBumpLocation, hasBump);

		if (i->GetAnimation()) {
			MeshGeometry* mesh = i->GetMesh();
			vector <NCLMatrix4> frameMatrices;
			const vector<NCLMatrix4> invBindPose = mesh->GetInverseBindPose();
			const NCLMatrix4* frameData = i->GetAnimation()->GetJointData(i->GetCurrentFrame());

			for (unsigned int i = 0; i < mesh->GetJointCount(); ++i) {
				auto matrix = invBindPose[i];
				frameMatrices.emplace_back(frameData[i] * matrix);
			}

			int j = glGetUniformLocation(((OGLShader*)(*i).GetShader())->GetProgramID(), "joints");
			glUniformMatrix4fv(j, frameMatrices.size(), false,
				(float*)frameMatrices.data());

		}

		BindMesh((*i).GetMesh());
		int activeDiffuse = -1;
		int activeBump = -1;
		for (int i = 0; i < layerCount; ++i) {
			if (hasDiff && ((OGLTexture*)textures[i])->GetObjectID() != activeDiffuse) {
				BindTextureToShader((OGLTexture*)textures[i], "mainTex", 0);
				activeDiffuse = ((OGLTexture*)textures[i])->GetObjectID();
			}
			if (hasBump && ((OGLTexture*)textures[i + layerCount])->GetObjectID() != activeDiffuse) {
				BindTextureToShader((OGLTexture*)textures[i + layerCount], "bumpTex", 2);
				activeBump = ((OGLTexture*)textures[i + layerCount])->GetObjectID();
			}
			DrawBoundMesh(i);
		}
	}
}



void GameTechRenderer::RenderNavMesh(Camera* current_camera) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	BindShader(NavMesh_shader);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	float screenAspect = (float)currentWidth / (float)currentHeight;

	NCLMatrix4 viewMatrix = current_camera->BuildViewMatrix();
	NCLMatrix4 projMatrix = current_camera->BuildProjectionMatrix(screenAspect);

	NCLMatrix4 vp = projMatrix * viewMatrix;

	int vpLocation = glGetUniformLocation(NavMesh_shader->GetProgramID(), "viewProjMatrix");
	glUniformMatrix4fv(vpLocation, 1, false, (float*)&vp);

	BindMesh(navMesh);
	DrawBoundMesh();

	BindMesh(centroidMesh);
	DrawBoundMesh();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GameTechRenderer::LoadNavMesh() {
	NavMesh_shader = new OGLShader("DebugVert.glsl", "DebugFrag.glsl");

	navMesh = new OGLMesh();

	std::ifstream mapFile(Assets::DATADIR + "Final01.navmesh");

	int vCount = 0;
	int iCount = 0;

	mapFile >> vCount;
	mapFile >> iCount;

	vector<NCLVector3>			meshVerts;
	vector<NCLVector4>			meshColours;
	vector<unsigned int>	meshIndices;

	for (int i = 0; i < vCount; ++i) {
		NCLVector3 temp;
		mapFile >> temp.x;
		mapFile >> temp.y;
		mapFile >> temp.z;
		meshVerts.emplace_back(temp);
		meshColours.emplace_back(NCLVector4(0, 1, 0, 1));
	}

	for (int i = 0; i < iCount; ++i) {
		unsigned int temp = -1;
		mapFile >> temp;
		meshIndices.emplace_back(temp);
	}

	struct TriNeighbours {
		int indices[3];
	};

	int numTris = iCount / 3;	//the indices describe n / 3 triangles
	vector< TriNeighbours> allNeighbours;
	//Each of these triangles will be sharing edges with some other triangles
	//so it has a maximum of 3 'neighbours', desribed by an index into n / 3 tris
	//if its a -1, then the edge is along the edge of the map...
	for (int i = 0; i < numTris; ++i) {
		TriNeighbours neighbours;
		mapFile >> neighbours.indices[0];
		mapFile >> neighbours.indices[1];
		mapFile >> neighbours.indices[2];
		allNeighbours.emplace_back(neighbours);
	}

	navMesh->SetVertexPositions(meshVerts);
	navMesh->SetVertexColours(meshColours);
	navMesh->SetVertexIndices(meshIndices);

	navMesh->UploadToGPU();

	vector<NCLVector3> centroids;
	vector<NCLVector4> centreColours;
	vector<unsigned int> lines;

	for (int i = 0; i < iCount; i += 3) {
		NCLVector3 a = meshVerts[meshIndices[i + 0]];
		NCLVector3 b = meshVerts[meshIndices[i + 1]];
		NCLVector3 c = meshVerts[meshIndices[i + 2]];

		NCLVector3 middle = (a + b + c) / 3.0f;
		centroids.emplace_back(middle);

		centreColours.emplace_back(NCLVector4(1, 0, 0, 1));
	}

	for (int i = 0; i < numTris; ++i) {
		TriNeighbours& n = allNeighbours[i];
		for (int j = 0; j < 3; ++j) {
			if (n.indices[j] != -1) {
				TriNeighbours& nj = allNeighbours[n.indices[j]];

				lines.emplace_back(i);
				lines.emplace_back(n.indices[j]);
			}
		}
	}
	centroidMesh = new OGLMesh();
	centroidMesh->SetVertexPositions(centroids);
	centroidMesh->SetVertexColours(centreColours);
	centroidMesh->SetVertexIndices(lines);
	centroidMesh->SetPrimitiveType(NCL::GeometryPrimitive::Lines);
	centroidMesh->UploadToGPU();
}

void GameTechRenderer::LoadNavMesh02() {
	navMesh02 = new OGLMesh();

	std::ifstream mapFile(Assets::DATADIR + "Final02.navmesh");

	int vCount = 0;
	int iCount = 0;

	mapFile >> vCount;
	mapFile >> iCount;

	vector<NCLVector3>			meshVerts;
	vector<NCLVector4>			meshColours;
	vector<unsigned int>	meshIndices;

	for (int i = 0; i < vCount; ++i) {
		NCLVector3 temp;
		mapFile >> temp.x;
		mapFile >> temp.y;
		mapFile >> temp.z;
		meshVerts.emplace_back(temp);
		meshColours.emplace_back(NCLVector4(0, 1, 0, 1));
	}

	for (int i = 0; i < iCount; ++i) {
		unsigned int temp = -1;
		mapFile >> temp;
		meshIndices.emplace_back(temp);
	}

	struct TriNeighbours {
		int indices[3];
	};

	int numTris = iCount / 3;	//the indices describe n / 3 triangles
	vector< TriNeighbours> allNeighbours;
	//Each of these triangles will be sharing edges with some other triangles
	//so it has a maximum of 3 'neighbours', desribed by an index into n / 3 tris
	//if its a -1, then the edge is along the edge of the map...
	for (int i = 0; i < numTris; ++i) {
		TriNeighbours neighbours;
		mapFile >> neighbours.indices[0];
		mapFile >> neighbours.indices[1];
		mapFile >> neighbours.indices[2];
		allNeighbours.emplace_back(neighbours);
	}

	navMesh02->SetVertexPositions(meshVerts);
	navMesh02->SetVertexColours(meshColours);
	navMesh02->SetVertexIndices(meshIndices);

	navMesh02->UploadToGPU();

	vector<NCLVector3> centroids;
	vector<NCLVector4> centreColours;
	vector<unsigned int> lines;

	for (int i = 0; i < iCount; i += 3) {
		NCLVector3 a = meshVerts[meshIndices[i + 0]];
		NCLVector3 b = meshVerts[meshIndices[i + 1]];
		NCLVector3 c = meshVerts[meshIndices[i + 2]];

		NCLVector3 middle = (a + b + c) / 3.0f;
		centroids.emplace_back(middle);

		centreColours.emplace_back(NCLVector4(1, 0, 0, 1));
	}

	for (int i = 0; i < numTris; ++i) {
		TriNeighbours& n = allNeighbours[i];
		for (int j = 0; j < 3; ++j) {
			if (n.indices[j] != -1) {
				TriNeighbours& nj = allNeighbours[n.indices[j]];

				lines.emplace_back(i);
				lines.emplace_back(n.indices[j]);
			}
		}
	}
	centroidMesh02 = new OGLMesh();
	centroidMesh02->SetVertexPositions(centroids);
	centroidMesh02->SetVertexColours(centreColours);
	centroidMesh02->SetVertexIndices(lines);
	centroidMesh02->SetPrimitiveType(NCL::GeometryPrimitive::Lines);
	centroidMesh02->UploadToGPU();
}

void GameTechRenderer::RenderNavMesh02(Camera* current_camera) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	BindShader(NavMesh_shader);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	float screenAspect = (float)currentWidth / (float)currentHeight;

	NCLMatrix4 viewMatrix = current_camera->BuildViewMatrix();
	NCLMatrix4 projMatrix = current_camera->BuildProjectionMatrix(screenAspect);

	NCLMatrix4 vp = projMatrix * viewMatrix;

	int vpLocation = glGetUniformLocation(NavMesh_shader->GetProgramID(), "viewProjMatrix");
	glUniformMatrix4fv(vpLocation, 1, false, (float*)&vp);

	BindMesh(navMesh02);
	DrawBoundMesh();

	BindMesh(centroidMesh02);
	DrawBoundMesh();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

NCLMatrix4 GameTechRenderer::SetupDebugLineMatrix()	const {
	float screenAspect = (float)currentWidth / (float)currentHeight;
	NCLMatrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
	NCLMatrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

	return projMatrix * viewMatrix;
}

NCLMatrix4 GameTechRenderer::SetupDebugStringMatrix()	const {
	return NCLMatrix4::Orthographic(-1, 1.0f, 100, 0, 0, 100);
}

void GameTechRenderer::LoadStartImage(){
	loading_shader = new OGLShader("LoadingVertex.glsl", "LoadingFragment.glsl");
	glGenTextures(1, &background_tex);
	glBindTexture(GL_TEXTURE_2D, background_tex);

	char* texData;
	int width = 0;
	int height = 0;
	int channel = 0;
	int flag = 0;

	stbi_set_flip_vertically_on_load(true);
	TextureLoader::LoadTexture("SpitoonBackground.png", texData, width, height, channel, flag);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);

	glBindTexture(GL_TEXTURE_2D, 0);
	sceneTextures.push_back(background_tex);

	/////////////////////
	glGenTextures(1, &loading_tex);
	glBindTexture(GL_TEXTURE_2D, loading_tex);

	stbi_set_flip_vertically_on_load(true);
	TextureLoader::LoadTexture("nyan.png", texData, width, height, channel, flag);
	stbi_set_flip_vertically_on_load(false);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);

	glBindTexture(GL_TEXTURE_2D, 0);
	sceneTextures.push_back(loading_tex);
}

void GameTechRenderer::RenderStartView() {
	static float movement = 0.0f;
	if (movement < 2.0f) {
		movement += Window::GetTimer()->GetTimeDeltaSeconds() / 5;
	}
	else if (movement > 2.0f) {

		loading = false;
	}

	BeginFrame();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	///////////
	BindShader(print_shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, background_tex);
	glUniform1i(glGetUniformLocation(print_shader->GetProgramID(), "diffuseTex"), 0);
	BindMesh(printer);
	DrawBoundMesh();
	glBindTexture(GL_TEXTURE_2D, 0);
	///////////
	if (loading) {
		BindShader(loading_shader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, loading_tex);
		glUniform1i(glGetUniformLocation(loading_shader->GetProgramID(), "nyan"), 0);
		glUniform1f(glGetUniformLocation(loading_shader->GetProgramID(), "movement"), movement);
		BindMesh(printer);
		DrawBoundMesh();
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	BindShader(nullptr);
	///////////
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);

	gameUI->UI_Render();
	OGLRenderer::SwapBuffers();
}
#endif _x64