#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Camera.h"
#include "../nclgl/MeshMaterial.h"
#include <algorithm>

const  int  POST_PASSES = 10;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	srand((unsigned int)time(0));

    quad = Mesh::GenerateQuad();

	treeObj = Mesh::LoadFromMeshFile("Fir_Tree.msh");
	treeMat = new MeshMaterial("Fir_Tree.mat");
	treeTex = SOIL_load_OGL_texture(TEXTUREDIR"tree_diffuse.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

    heightMap = new  HeightMap(TEXTUREDIR"noise_combined.png");

    earthTex = SOIL_load_OGL_texture(TEXTUREDIR"grass.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

    earthBump = SOIL_load_OGL_texture(TEXTUREDIR"BarrenRedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	grassFlowers = SOIL_load_OGL_texture(TEXTUREDIR"grassFlowers.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	mud = SOIL_load_OGL_texture(TEXTUREDIR"mud.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	aerialRocks = SOIL_load_OGL_texture(TEXTUREDIR"aerial_rocks_02_diff_1k.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	blendMap = SOIL_load_OGL_texture(TEXTUREDIR"blendMap.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

    cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg", TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg", TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	
	cubeMap2 = SOIL_load_OGL_cubemap(TEXTUREDIR"nightTop.png", TEXTUREDIR"nightFront.png", TEXTUREDIR"nightBack.png", TEXTUREDIR"nightBottom.png", TEXTUREDIR"nightLeft.png", TEXTUREDIR"nightRight.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	
	bonfire = Mesh::LoadFromMeshFile("Bonfire_B_Small_Fire.msh");

	woodTex = SOIL_load_OGL_texture(TEXTUREDIR"Texture, Wall rock and wood.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

    if (!earthTex || !earthBump || !cubeMap || !waterTex || !treeTex) {
        return;
    }

	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(grassFlowers, true);
	SetTextureRepeating(mud, true);
	SetTextureRepeating(aerialRocks, true);
	SetTextureRepeating(blendMap, true);

    reflectShader = new  Shader("reflectVertex.glsl", "reflectFragment.glsl");
    skyboxShader = new  Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
    lightShader = new  Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	treesShader = new Shader("treeVertex.glsl", "treeFragment.glsl");
	geoShader = new Shader("pointVert.glsl", "TexturedFragment.glsl", "pointGeom.glsl");
	bumpShader = new Shader("BumpVertex.glsl", "BumpFragment.glsl");

    if (!reflectShader->LoadSuccess() ||
        !skyboxShader->LoadSuccess() ||
        !lightShader->LoadSuccess() ) {
        return ;
    }

    processShader = new  Shader("TexturedVertex.glsl", "processfrag.glsl");

    if (!processShader->LoadSuccess()) {
        return;
    }
 
    heightmapSize = heightMap->GetHeightmapSize();
	randGroundPos = heightMap->GetRandPos();

	root = new SceneNode();

	root->AddChild(new SceneNode(bonfire, Vector4(0, 0, 0, 0)));

    camera = new Camera(-45.0f, 0.0f, heightmapSize * Vector3(0.5f, 5.0f, 0.5f));
    light = new Light(heightmapSize * Vector3(0.5f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), heightmapSize.x);

	light->SetDirection(Vector3(heightmapSize * Vector3(0.1f, 2.5f, 0.3f)));
	light->SetAmbient(Vector3(0.5f, 0.5f, 0.5f));
	light->SetDiffuse(Vector3(1.0f, 1.0f, 1.0f));

	
	light->Set_S_CutOff(12.5f);
	light->Set_S_OuterCutOff(17.5f);
	light->Set_S_Ambient(Vector3(0.5f, 0.5f, 0.5f));
	light->Set_S_Diffuse(Vector3(1.0f, 1.0f, 1.0f));
	light->Set_S_Specular(Vector3(1.0f, 1.0f, 1.0f));
	light->Set_S_Constant(1.0f);
	light->Set_S_Linear(0.045f);
	light->Set_S_Quadratic(0.0075f);
	light->Set_S_Shininess(32.0f);

    projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    init = true;
}

Renderer::~Renderer(void) { 
    delete  camera; 
    delete  heightMap; 
    delete  quad; 
	delete  root;
    delete  reflectShader; 
    delete  skyboxShader;
    delete  lightShader; 
	delete  treesShader;
    delete  light; 
}

    void  Renderer::UpdateScene(float dt) {
        
        camera->UpdateCamera(dt); 
        viewMatrix = camera->BuildViewMatrix(); 
		fireTheta += dt * 2;
		
		if (isDay == true && Window::GetKeyboard()->KeyDown(KEYBOARD_M)) {
			skyTime = 0.0f;
			isDay = false;
		}
		else if(isDay == false && Window::GetKeyboard()->KeyDown(KEYBOARD_N)) {
			skyTime = 1.0f;
			isDay = true;
		}
		
		if (isDay == true) {
			if (skyTime > 0.0f) {
				skyTime -= dt * 0.1f;
			}
			if (skyTime < 0.0f) {
				skyTime = 0.0f;
			}
		}
		else {
			if (skyTime < 1.0f) {
				skyTime += dt * 0.1f;
			}
			if (skyTime > 1.0f) {
				skyTime = 1.0f;
			}
		}
		
		root->Update(dt);
       
    }

    void  Renderer::RenderScene() { 
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 
        DrawSkybox();
		
        DrawHeightmap(); 
        DrawTrees(); 
		DrawFire();
       
    }

    void   Renderer::DrawSkybox() { 
       
        glDepthMask(GL_FALSE); 
        
        BindShader(skyboxShader); 


           
        UpdateShaderMatrices(); 
		
		

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap2);


		glUniform1i(glGetUniformLocation(
			skyboxShader->GetProgram(), "cubeTex"), 0);

		glUniform1i(glGetUniformLocation(
			skyboxShader->GetProgram(), "cubeTex1"), 1);
		
		glUniform1f(glGetUniformLocation(
			skyboxShader->GetProgram(), "fade"), skyTime);

        quad->Draw(); 
        
        glDepthMask(GL_TRUE); 
       
    }

    void   Renderer::DrawHeightmap() {
        
        BindShader(lightShader);
        SetShaderLight(*light);
        glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, earthTex);



		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "rTex"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mud);

		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "gTex"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, grassFlowers);

		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "bTex"), 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, aerialRocks);

		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "blendMap"), 4);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, blendMap);

        modelMatrix.ToIdentity();      //New!
        textureMatrix.ToIdentity();    //New!

        UpdateShaderMatrices();

        heightMap->Draw();
    }

	void Renderer::DrawFire() {
		BindShader(geoShader);

		SetShaderLight(*light);
		glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

		glUniform1i(glGetUniformLocation(geoShader->GetProgram(), "diffuseTex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTex);

		glUniform1i(glGetUniformLocation(geoShader->GetProgram(), "mode"), isFireOn);

		glUniform1f(glGetUniformLocation(geoShader->GetProgram(), "theta"), fireTheta);


		modelMatrix.ToIdentity();

		modelMatrix = modelMatrix *
			Matrix4::Translation(Vector3(heightmapSize.x *0.7f, heightmapSize.y* 0.2f, heightmapSize.z*0.9f)) *
			Matrix4::Scale(Vector3(1, 1, 1) * heightmapSize.y / 3.0f);
		
		textureMatrix.ToIdentity();

		UpdateShaderMatrices();
		bonfire->Draw();
	}

	void Renderer::DrawSceneGraph()
	{
		BuildNodeLists(root);
		SortNodeLists();

		BindShader(sceneShader);
		UpdateShaderMatrices();
		glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);

		DrawNodes();

		ClearNodeLists();
	}


	void Renderer::BuildNodeLists(SceneNode* from)
	{
		if (frameFrustum.InsideFrustum(*from))
		{
			Vector3 dir = from->GetWorldTransform().GetPositionVector() -
				camera->GetPosition();
			from->SetCameraDistance(Vector3::Dot(dir, dir));

			if (from->GetColour().w < 1.0f)
			{
				transparentNodeList.push_back(from);
			}
			else
			{
				nodeList.push_back(from);
			}
		}

		for (vector < SceneNode* >::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i)
		{
			BuildNodeLists((*i));
		}
	}

	void Renderer::SortNodeLists()
	{
		std::sort(transparentNodeList.rbegin(), // note the r!
			transparentNodeList.rend(), // note the r!
			SceneNode::CompareByCameraDistance);

		std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
	}

	void Renderer::DrawNodes() {
		for (const auto& i : nodeList)
		{
			DrawNode(i);
		}

		for (const auto& i : transparentNodeList)
		{
			DrawNode(i);
		}
	}

	void Renderer::DrawNode(SceneNode* n) {
		if (n->GetMesh()) {
			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());


			glUniformMatrix4fv(glGetUniformLocation(sceneShader->GetProgram(), "modelMatrix"), 1, false, model.values);

			glUniform4fv(glGetUniformLocation(sceneShader->GetProgram(), "nodeColour"), 1, (float*)& n->GetColour());

			sceneTex = n->GetTexture();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, sceneTex);

			glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "useTexture"), sceneTex);

			n->Draw(*this);
		}
	}

	void Renderer::ClearNodeLists() {
		transparentNodeList.clear();
		nodeList.clear();
	}
	

	void Renderer::DrawTrees()
	{
		BindShader(treesShader);
		glUniform1i(glGetUniformLocation(treesShader->GetProgram(), "material.diffuseTex"), 0);
		glUniform3fv(glGetUniformLocation(treesShader->GetProgram(), "light.direction"), 1, (float*)& light->GetDirection());
		glUniform3fv(glGetUniformLocation(treesShader->GetProgram(), "light.ambient"), 1, (float*)& light->GetAmbient());
		glUniform3fv(glGetUniformLocation(treesShader->GetProgram(), "light.diffuse"), 1, (float*)& light->GetDiffuse());

		int b = 0;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, treeTex);

		for (int v = 0; v < 500; v++)
		{
			modelMatrix = Matrix4::Translation
			(Vector3((heightmapSize.x * 0.0f) + randGroundPos[b].x,
				((heightmapSize.y * 0.0f) + 0.0f) + randGroundPos[b].y,
				(heightmapSize.z * 0.0f) + randGroundPos[b].z)) 
				* Matrix4::Scale(Vector3(50.0f, 50.0f, 50.0f));
			UpdateShaderMatrices();
			
			treeObj->Draw();
			b += 15;
		}
	} 