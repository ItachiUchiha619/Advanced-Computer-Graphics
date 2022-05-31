#pragma  once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/CubeRobot.h"

class  Camera;
class  Shader;
class  HeightMap;
class  MeshMaterial;
class  Light;

class  Renderer : public  OGLRenderer     {
public:
    Renderer(Window &parent );
	~Renderer(void);
void  RenderScene ()  override;
void  UpdateScene(float dt) override;

protected:
    void DrawHeightmap();
    void DrawSkybox();
	void DrawTrees();
	void DrawFire();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);
	void DrawSceneGraph();
    
    Shader* lightShader;
    Shader* reflectShader;
    Shader* skyboxShader;
	Shader* geoShader;
    
    HeightMap* heightMap;
    Mesh* quad;
	
	//tree
	Mesh* treeObj;
	MeshMaterial* treeMat;
	GLuint treeTex;
	Shader* treesShader;
    
    Light*       light;
    Camera*      camera;
	SceneNode*   root;
	Frustum      frameFrustum;

	GLuint        cubeMap;
	GLuint		  cubeMap2;

	GLuint        waterTex;
	GLuint        earthTex;
	GLuint        earthBump;
	GLuint        grassFlowers;
	GLuint		  mud;
	GLuint		  aerialRocks;
	GLuint        blendMap;
	GLuint        sceneTex;

	float     waterRotate;
	float     waterCycle;

	void          PresentScene();
	void          DrawPostProcess();

	Shader* sceneShader;
	Shader* processShader;

	GLuint        heightTexture;
	GLuint        bufferFBO;
	GLuint        processFBO;
	GLuint        bufferColourTex[2];
	GLuint        bufferDepthTex;

	bool isDay = true;
	float skyTime = 0.0f;
	bool camSwitch = false;
	bool orienSwitch = false;
	
	Shader* bumpShader;
	Mesh* bonfire;
	GLuint woodTex;
	float fireTheta = 0.0f;

	int isFireOn = 1;

	vector<Vector3> randGroundPos;
	Vector3  heightmapSize;
	vector <SceneNode*> transparentNodeList;
	vector <SceneNode*> nodeList;
};

