/* 
Animación por keyframes
La textura del skybox fue conseguida desde la página https ://opengameart.org/content/elyvisions-skyboxes?page=1
y edité en Gimp rotando 90 grados en sentido antihorario la imagen  sp2_up.png para poder ver continuidad.
Fuentes :
	https ://www.khronos.org/opengl/wiki/Keyframe_Animation
	http ://what-when-how.com/wp-content/uploads/2012/07/tmpcd0074_thumb.png
	*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <fstream>   
#include <glew.h>
#include <glfw3.h>
#include <sstream>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables archivo
int guardoArchivo = 0;
int cicloCarga = 0;
int banderaCarga = 0;

//variables para animación
float braviaryavance = 0.0f;
bool avanzabraviary = true;
float movOffsetbraviary = 0.5f;
float anguloAlaBraviary = 0.0f;
float rotacionBraviary = 0.0f;




float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float angulovaria = 0.0f;

//variables para keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, ciclo3, ciclo4, ciclo5, ciclo6, ciclo7, ciclo8 , ciclo9 = 0;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;


Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;


//ModelsSnorlax
Model CuerpoSnorlax_M;
Model PiernaDerecha_M;
Model PiernaIzquierda_M;
Model BrazoIzquierdo_M;
Model BrazoDerecho_M;

//Modelos Braviary
Model Braviary_M;
Model AlaDerechaB_M;
Model AlaIzquierdaB_M;

//Modelos Archen
Model Archen_M;
Model AlaDerechaA_M;
Model AlaIzquierdaA_M;



Model Blackhawk_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función para teclado de keyframes 
void inputKeyframes(bool* keys);

//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;



//NEW// Keyframes
float posXavion = 2.0, posYavion = 5.0, posZavion = -3.0;
float	movAvion_x = 0.0f, movAvion_y = 0.0f;
float giroAvion = 0;

//Snorlax
// Key Frames Snorlax
float movCuerpoSnorlax_x = 0.0f;
float movCuerpoSnorlax_z = 0.0f;
float giroSnorlax = 0.0f;
// Brazos Snorlax
float giroBrazoIzquierdo_x = 0.0f;
float giroBrazoIzquierdo_y = 0.0f;
float giroBrazoIzquierdo_z = 0.0f;
float movBrazoIzquierdo_x = 0.0f;   // Movimiento lineal en X
float movBrazoIzquierdo_z = 0.0f;   // Movimiento lineal en Z

float giroBrazoDerecho_x = 0.0f;
float giroBrazoDerecho_z = 0.0f;
float giroBrazoDe_y = 0.0f;
float movBrazoDerecho_x = 0.0f;     // Movimiento lineal en X
float movBrazoDerecho_z = 0.0f;     // Movimiento lineal en Z

// Piernas Snorlax
float giroPiernaIzquierda_x = 0.0f;
float giroPiernaIzquierda_z = 0.0f;
float movPiernaIzquierda_x = 0.0f;  // Movimiento lineal en X
float movPiernaIzquierda_z = 0.0f;  // Movimiento lineal en Z

float giroPiernaDerecha_x = 0.0f;
float giroPiernaDerecha_z = 0.0f;
float movPiernaDerecha_x = 0.0f;    // Movimiento lineal en X
float movPiernaDerecha_z = 0.0f;    // Movimiento lineal en Z





#define MAX_FRAMES 100 //Número de cuadros máximos
int i_max_steps = 8000; //Número de pasos entre cuadros para interpolación, a mayor número , más lento será el movimiento
int i_curr_steps = 0;

typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float giroAvion;		//Variable para GiroAvion
	float giroAvionInc;		//Variable para IncrementoGiroAvion


	// Incrementos del cuerpo Snorlax
	float movCuerpoSnorlax_xInc;
	float movCuerpoSnorlax_zInc;
	float giroSnorlaxInc;

	
	// Brazos Snorlax
	float giroBrazoIzquierdo_xInc;
	float giroBrazoIzquierdo_zInc;
	float movBrazoIzquierdo_xInc;   // Movimiento lineal en X
	float movBrazoIzquierdo_zInc;   // Movimiento lineal en Z

	float giroBrazoDerecho_xInc;
	float giroBrazoDerecho_zInc;
	float movBrazoDerecho_xInc;     // Movimiento lineal en X
	float movBrazoDerecho_zInc;     // Movimiento lineal en Z

	// Piernas Snorlax
	float giroPiernaIzquierda_xInc;
	float giroPiernaIzquierda_zInc;
	float movPiernaIzquierda_xInc;  // Movimiento lineal en X
	float movPiernaIzquierda_zInc;  // Movimiento lineal en Z

	float giroPiernaDerecha_xInc;
	float giroPiernaDerecha_zInc;
	float movPiernaDerecha_xInc;    // Movimiento lineal en X
	float movPiernaDerecha_zInc;    // Movimiento lineal en Z


	//------------------------------------------------------------------------
	//Key Frames Snorlax
	float movCuerpoSnorlax_x;
	float movCuerpoSnorlax_z;
	float giroSnorlax;


	// Brazos Snorlax
	float giroBrazoIzquierdo_x;
	float giroBrazoIzquierdo_z;
	float movBrazoIzquierdo_x;   // Movimiento lineal en X
	float movBrazoIzquierdo_z;   // Movimiento lineal en Z

	float giroBrazoDerecho_x;
	float giroBrazoDerecho_z;
	float movBrazoDerecho_x;     // Movimiento lineal en X
	float movBrazoDerecho_z;     // Movimiento lineal en Z

	// Piernas Snorlax
	float giroPiernaIzquierda_x;
	float giroPiernaIzquierda_z;
	float movPiernaIzquierda_x;  // Movimiento lineal en X
	float movPiernaIzquierda_z;  // Movimiento lineal en Z

	float giroPiernaDerecha_x;
	float giroPiernaDerecha_z;
	float movPiernaDerecha_x;    // Movimiento lineal en X
	float movPiernaDerecha_z;    // Movimiento lineal en Z

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 21;			//El número de cuadros guardados actualmente desde 0 para no sobreescribir
bool play = false;
int playIndex = 0;

void saveFrame(void) // tecla L
{
	printf("FrameIndex %d\n", FrameIndex);

	// --- Avión (si aún lo usas) ---
	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].giroAvion = giroAvion;

	// --- Snorlax ---

	// Cuerpo
	KeyFrame[FrameIndex].movCuerpoSnorlax_x = movCuerpoSnorlax_x;
	KeyFrame[FrameIndex].movCuerpoSnorlax_z = movCuerpoSnorlax_z;
	KeyFrame[FrameIndex].giroSnorlax = giroSnorlax;

	// Brazo Izquierdo
	KeyFrame[FrameIndex].giroBrazoIzquierdo_x = giroBrazoIzquierdo_x;
	KeyFrame[FrameIndex].giroBrazoIzquierdo_z = giroBrazoIzquierdo_z;
	KeyFrame[FrameIndex].movBrazoIzquierdo_x = movBrazoIzquierdo_x;
	KeyFrame[FrameIndex].movBrazoIzquierdo_z = movBrazoIzquierdo_z;

	// Brazo Derecho
	KeyFrame[FrameIndex].giroBrazoDerecho_x = giroBrazoDerecho_x;
	KeyFrame[FrameIndex].giroBrazoDerecho_z = giroBrazoDerecho_z;
	KeyFrame[FrameIndex].movBrazoDerecho_x = movBrazoDerecho_x;
	KeyFrame[FrameIndex].movBrazoDerecho_z = movBrazoDerecho_z;

	// Pierna Izquierda
	KeyFrame[FrameIndex].giroPiernaIzquierda_x = giroPiernaIzquierda_x;
	KeyFrame[FrameIndex].giroPiernaIzquierda_z = giroPiernaIzquierda_z;
	KeyFrame[FrameIndex].movPiernaIzquierda_x = movPiernaIzquierda_x;
	KeyFrame[FrameIndex].movPiernaIzquierda_z = movPiernaIzquierda_z;

	// Pierna Derecha
	KeyFrame[FrameIndex].giroPiernaDerecha_x = giroPiernaDerecha_x;
	KeyFrame[FrameIndex].giroPiernaDerecha_z = giroPiernaDerecha_z;
	KeyFrame[FrameIndex].movPiernaDerecha_x = movPiernaDerecha_x;
	KeyFrame[FrameIndex].movPiernaDerecha_z = movPiernaDerecha_z;

	// Avanza al siguiente frame
	FrameIndex++;
}



void resetElements(void) // Tecla 0
{
	// --- Avión (si aún lo usas) ---
	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	giroAvion = KeyFrame[0].giroAvion;

	// --- Snorlax ---

	// Cuerpo
	movCuerpoSnorlax_x = KeyFrame[0].movCuerpoSnorlax_x;
	movCuerpoSnorlax_z = KeyFrame[0].movCuerpoSnorlax_z;
	giroSnorlax = KeyFrame[0].giroSnorlax;

	// Brazo Izquierdo
	giroBrazoIzquierdo_x = KeyFrame[0].giroBrazoIzquierdo_x;
	giroBrazoIzquierdo_z = KeyFrame[0].giroBrazoIzquierdo_z;
	movBrazoIzquierdo_x = KeyFrame[0].movBrazoIzquierdo_x;
	movBrazoIzquierdo_z = KeyFrame[0].movBrazoIzquierdo_z;

	// Brazo Derecho
	giroBrazoDerecho_x = KeyFrame[0].giroBrazoDerecho_x;
	giroBrazoDerecho_z = KeyFrame[0].giroBrazoDerecho_z;
	movBrazoDerecho_x = KeyFrame[0].movBrazoDerecho_x;
	movBrazoDerecho_z = KeyFrame[0].movBrazoDerecho_z;

	// Pierna Izquierda
	giroPiernaIzquierda_x = KeyFrame[0].giroPiernaIzquierda_x;
	giroPiernaIzquierda_z = KeyFrame[0].giroPiernaIzquierda_z;
	movPiernaIzquierda_x = KeyFrame[0].movPiernaIzquierda_x;
	movPiernaIzquierda_z = KeyFrame[0].movPiernaIzquierda_z;

	// Pierna Derecha
	giroPiernaDerecha_x = KeyFrame[0].giroPiernaDerecha_x;
	giroPiernaDerecha_z = KeyFrame[0].giroPiernaDerecha_z;
	movPiernaDerecha_x = KeyFrame[0].movPiernaDerecha_x;
	movPiernaDerecha_z = KeyFrame[0].movPiernaDerecha_z;
}

void interpolation(void)
{
	// --- Avión ---
	KeyFrame[playIndex].movAvion_xInc =
		(KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc =
		(KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc =
		(KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;

	// --- Cuerpo Snorlax ---
	KeyFrame[playIndex].movCuerpoSnorlax_xInc =
		(KeyFrame[playIndex + 1].movCuerpoSnorlax_x - KeyFrame[playIndex].movCuerpoSnorlax_x) / i_max_steps;
	KeyFrame[playIndex].movCuerpoSnorlax_zInc =
		(KeyFrame[playIndex + 1].movCuerpoSnorlax_z - KeyFrame[playIndex].movCuerpoSnorlax_z) / i_max_steps;
	KeyFrame[playIndex].giroSnorlaxInc =
		(KeyFrame[playIndex + 1].giroSnorlax - KeyFrame[playIndex].giroSnorlax) / i_max_steps;

	// --- Brazo Izquierdo ---
	KeyFrame[playIndex].giroBrazoIzquierdo_xInc =
		(KeyFrame[playIndex + 1].giroBrazoIzquierdo_x - KeyFrame[playIndex].giroBrazoIzquierdo_x) / i_max_steps;
	KeyFrame[playIndex].giroBrazoIzquierdo_zInc =
		(KeyFrame[playIndex + 1].giroBrazoIzquierdo_z - KeyFrame[playIndex].giroBrazoIzquierdo_z) / i_max_steps;
	KeyFrame[playIndex].movBrazoIzquierdo_xInc =
		(KeyFrame[playIndex + 1].movBrazoIzquierdo_x - KeyFrame[playIndex].movBrazoIzquierdo_x) / i_max_steps;
	KeyFrame[playIndex].movBrazoIzquierdo_zInc =
		(KeyFrame[playIndex + 1].movBrazoIzquierdo_z - KeyFrame[playIndex].movBrazoIzquierdo_z) / i_max_steps;

	// --- Brazo Derecho ---
	KeyFrame[playIndex].giroBrazoDerecho_xInc =
		(KeyFrame[playIndex + 1].giroBrazoDerecho_x - KeyFrame[playIndex].giroBrazoDerecho_x) / i_max_steps;
	KeyFrame[playIndex].giroBrazoDerecho_zInc =
		(KeyFrame[playIndex + 1].giroBrazoDerecho_z - KeyFrame[playIndex].giroBrazoDerecho_z) / i_max_steps;
	KeyFrame[playIndex].movBrazoDerecho_xInc =
		(KeyFrame[playIndex + 1].movBrazoDerecho_x - KeyFrame[playIndex].movBrazoDerecho_x) / i_max_steps;
	KeyFrame[playIndex].movBrazoDerecho_zInc =
		(KeyFrame[playIndex + 1].movBrazoDerecho_z - KeyFrame[playIndex].movBrazoDerecho_z) / i_max_steps;

	// --- Pierna Izquierda ---
	KeyFrame[playIndex].giroPiernaIzquierda_xInc =
		(KeyFrame[playIndex + 1].giroPiernaIzquierda_x - KeyFrame[playIndex].giroPiernaIzquierda_x) / i_max_steps;
	KeyFrame[playIndex].giroPiernaIzquierda_zInc =
		(KeyFrame[playIndex + 1].giroPiernaIzquierda_z - KeyFrame[playIndex].giroPiernaIzquierda_z) / i_max_steps;
	KeyFrame[playIndex].movPiernaIzquierda_xInc =
		(KeyFrame[playIndex + 1].movPiernaIzquierda_x - KeyFrame[playIndex].movPiernaIzquierda_x) / i_max_steps;
	KeyFrame[playIndex].movPiernaIzquierda_zInc =
		(KeyFrame[playIndex + 1].movPiernaIzquierda_z - KeyFrame[playIndex].movPiernaIzquierda_z) / i_max_steps;

	// --- Pierna Derecha ---
	KeyFrame[playIndex].giroPiernaDerecha_xInc =
		(KeyFrame[playIndex + 1].giroPiernaDerecha_x - KeyFrame[playIndex].giroPiernaDerecha_x) / i_max_steps;
	KeyFrame[playIndex].giroPiernaDerecha_zInc =
		(KeyFrame[playIndex + 1].giroPiernaDerecha_z - KeyFrame[playIndex].giroPiernaDerecha_z) / i_max_steps;
	KeyFrame[playIndex].movPiernaDerecha_xInc =
		(KeyFrame[playIndex + 1].movPiernaDerecha_x - KeyFrame[playIndex].movPiernaDerecha_x) / i_max_steps;
	KeyFrame[playIndex].movPiernaDerecha_zInc =
		(KeyFrame[playIndex + 1].movPiernaDerecha_z - KeyFrame[playIndex].movPiernaDerecha_z) / i_max_steps;
}



// ----------------------------------------------------
// Función para guardar todos los keyframes de Snorlax a un archivo
// ----------------------------------------------------
void GuardarKeyframesEnArchivo(FRAME KeyFrame[], int FrameIndex, const std::string& nombreArchivo) {
	std::ofstream archivo(nombreArchivo, std::ios::app); // modo append

	if (!archivo.is_open()) {
		std::cerr << "Error al abrir el archivo para guardar los keyframes.\n";
		return;
	}

	archivo << "---- NUEVA SESIÓN DE GUARDADO ----\n";
	for (int i = 0; i < FrameIndex; ++i) {
		archivo << "Keyframe " << i << ":\n";

		// ------------------ Cuerpo Snorlax ------------------
		archivo << "movCuerpoSnorlax_x: " << KeyFrame[i].movCuerpoSnorlax_x << "\n";
		archivo << "movCuerpoSnorlax_z: " << KeyFrame[i].movCuerpoSnorlax_z << "\n";
		archivo << "giroSnorlax: " << KeyFrame[i].giroSnorlax << "\n";

		// ------------------ Brazo Izquierdo ------------------
		archivo << "giroBrazoIzquierdo_x: " << KeyFrame[i].giroBrazoIzquierdo_x << "\n";
		archivo << "giroBrazoIzquierdo_z: " << KeyFrame[i].giroBrazoIzquierdo_z << "\n";
		archivo << "movBrazoIzquierdo_x: " << KeyFrame[i].movBrazoIzquierdo_x << "\n";
		archivo << "movBrazoIzquierdo_z: " << KeyFrame[i].movBrazoIzquierdo_z << "\n";

		// ------------------ Brazo Derecho ------------------
		archivo << "giroBrazoDerecho_x: " << KeyFrame[i].giroBrazoDerecho_x << "\n";
		archivo << "giroBrazoDerecho_z: " << KeyFrame[i].giroBrazoDerecho_z << "\n";
		archivo << "movBrazoDerecho_x: " << KeyFrame[i].movBrazoDerecho_x << "\n";
		archivo << "movBrazoDerecho_z: " << KeyFrame[i].movBrazoDerecho_z << "\n";

		// ------------------ Pierna Izquierda ------------------
		archivo << "giroPiernaIzquierda_x: " << KeyFrame[i].giroPiernaIzquierda_x << "\n";
		archivo << "giroPiernaIzquierda_z: " << KeyFrame[i].giroPiernaIzquierda_z << "\n";
		archivo << "movPiernaIzquierda_x: " << KeyFrame[i].movPiernaIzquierda_x << "\n";
		archivo << "movPiernaIzquierda_z: " << KeyFrame[i].movPiernaIzquierda_z << "\n";

		// ------------------ Pierna Derecha ------------------
		archivo << "giroPiernaDerecha_x: " << KeyFrame[i].giroPiernaDerecha_x << "\n";
		archivo << "giroPiernaDerecha_z: " << KeyFrame[i].giroPiernaDerecha_z << "\n";
		archivo << "movPiernaDerecha_x: " << KeyFrame[i].movPiernaDerecha_x << "\n";
		archivo << "movPiernaDerecha_z: " << KeyFrame[i].movPiernaDerecha_z << "\n";

		archivo << "----------------------------\n";
	}

	archivo.close();
	std::cout << "Se guardaron " << FrameIndex << " keyframes en " << nombreArchivo << "\n";
}




// ----------------------------------------------------
// Función para cargar keyframes de Snorlax desde un archivo
// ----------------------------------------------------
void CargarKeyframesDesdeArchivo(FRAME KeyFrame[], int& FrameIndex, const std::string& nombreArchivo) {
	std::ifstream archivo(nombreArchivo);

	if (!archivo.is_open()) {
		std::cerr << "Error al abrir el archivo para cargar los keyframes.\n";
		return;
	}

	std::string linea;
	int index = 0;

	while (std::getline(archivo, linea)) {
		std::stringstream ss(linea.substr(linea.find(":") + 1));

		// ------------------ Cuerpo Snorlax ------------------
		if (linea.find("movCuerpoSnorlax_x:") != std::string::npos) ss >> KeyFrame[index].movCuerpoSnorlax_x;
		else if (linea.find("movCuerpoSnorlax_z:") != std::string::npos) ss >> KeyFrame[index].movCuerpoSnorlax_z;
		else if (linea.find("giroSnorlax:") != std::string::npos) ss >> KeyFrame[index].giroSnorlax;

		// ------------------ Brazo Izquierdo ------------------
		else if (linea.find("giroBrazoIzquierdo_x:") != std::string::npos) ss >> KeyFrame[index].giroBrazoIzquierdo_x;
		else if (linea.find("giroBrazoIzquierdo_z:") != std::string::npos) ss >> KeyFrame[index].giroBrazoIzquierdo_z;
		else if (linea.find("movBrazoIzquierdo_x:") != std::string::npos) ss >> KeyFrame[index].movBrazoIzquierdo_x;
		else if (linea.find("movBrazoIzquierdo_z:") != std::string::npos) ss >> KeyFrame[index].movBrazoIzquierdo_z;

		// ------------------ Brazo Derecho ------------------
		else if (linea.find("giroBrazoDerecho_x:") != std::string::npos) ss >> KeyFrame[index].giroBrazoDerecho_x;
		else if (linea.find("giroBrazoDerecho_z:") != std::string::npos) ss >> KeyFrame[index].giroBrazoDerecho_z;
		else if (linea.find("movBrazoDerecho_x:") != std::string::npos) ss >> KeyFrame[index].movBrazoDerecho_x;
		else if (linea.find("movBrazoDerecho_z:") != std::string::npos) ss >> KeyFrame[index].movBrazoDerecho_z;

		// ------------------ Pierna Izquierda ------------------
		else if (linea.find("giroPiernaIzquierda_x:") != std::string::npos) ss >> KeyFrame[index].giroPiernaIzquierda_x;
		else if (linea.find("giroPiernaIzquierda_z:") != std::string::npos) ss >> KeyFrame[index].giroPiernaIzquierda_z;
		else if (linea.find("movPiernaIzquierda_x:") != std::string::npos) ss >> KeyFrame[index].movPiernaIzquierda_x;
		else if (linea.find("movPiernaIzquierda_z:") != std::string::npos) ss >> KeyFrame[index].movPiernaIzquierda_z;

		// ------------------ Pierna Derecha ------------------
		else if (linea.find("giroPiernaDerecha_x:") != std::string::npos) ss >> KeyFrame[index].giroPiernaDerecha_x;
		else if (linea.find("giroPiernaDerecha_z:") != std::string::npos) ss >> KeyFrame[index].giroPiernaDerecha_z;
		else if (linea.find("movPiernaDerecha_x:") != std::string::npos) ss >> KeyFrame[index].movPiernaDerecha_x;
		else if (linea.find("movPiernaDerecha_z:") != std::string::npos) ss >> KeyFrame[index].movPiernaDerecha_z;

		// ------------------ Fin de keyframe ------------------
		else if (linea.find("----------------------------") != std::string::npos) {
			index++;
			if (index >= MAX_FRAMES) break; // Evita desbordamiento
		}
	}

	FrameIndex = index;
	archivo.close();

	std::cout << "Se cargaron " << FrameIndex << " keyframes desde " << nombreArchivo << "\n";
}


void animate(void)
{
	// Movimiento del Snorlax con barra espaciadora
	if (play)
	{
		if (i_curr_steps >= i_max_steps) // ¿Fin de la animación entre frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);

			if (playIndex > FrameIndex - 2) // ¿Fin de toda la animación con el último frame?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("Terminó la animación\n");
				playIndex = 0;
				play = false;
			}
			else // Interpolación del próximo cuadro
			{
				i_curr_steps = 0; // Resetea contador
				interpolation();  // Interpolar valores del siguiente frame
			}
		}
		else
		{
			// -----------------------------
			// Dibujar Animación de Snorlax
			// -----------------------------

			// --- Cuerpo ---
			movCuerpoSnorlax_x += KeyFrame[playIndex].movCuerpoSnorlax_xInc;
			movCuerpoSnorlax_z += KeyFrame[playIndex].movCuerpoSnorlax_zInc;
			giroSnorlax += KeyFrame[playIndex].giroSnorlaxInc;

			// --- Brazo Izquierdo ---
			giroBrazoIzquierdo_x += KeyFrame[playIndex].giroBrazoIzquierdo_xInc;
			giroBrazoIzquierdo_z += KeyFrame[playIndex].giroBrazoIzquierdo_zInc;
			movBrazoIzquierdo_x += KeyFrame[playIndex].movBrazoIzquierdo_xInc;
			movBrazoIzquierdo_z += KeyFrame[playIndex].movBrazoIzquierdo_zInc;

			// --- Brazo Derecho ---
			giroBrazoDerecho_x += KeyFrame[playIndex].giroBrazoDerecho_xInc;
			giroBrazoDerecho_z += KeyFrame[playIndex].giroBrazoDerecho_zInc;
			movBrazoDerecho_x += KeyFrame[playIndex].movBrazoDerecho_xInc;
			movBrazoDerecho_z += KeyFrame[playIndex].movBrazoDerecho_zInc;

			// --- Pierna Izquierda ---
			giroPiernaIzquierda_x += KeyFrame[playIndex].giroPiernaIzquierda_xInc;
			giroPiernaIzquierda_z += KeyFrame[playIndex].giroPiernaIzquierda_zInc;
			movPiernaIzquierda_x += KeyFrame[playIndex].movPiernaIzquierda_xInc;
			movPiernaIzquierda_z += KeyFrame[playIndex].movPiernaIzquierda_zInc;

			// --- Pierna Derecha ---
			giroPiernaDerecha_x += KeyFrame[playIndex].giroPiernaDerecha_xInc;
			giroPiernaDerecha_z += KeyFrame[playIndex].giroPiernaDerecha_zInc;
			movPiernaDerecha_x += KeyFrame[playIndex].movPiernaDerecha_xInc;
			movPiernaDerecha_z += KeyFrame[playIndex].movPiernaDerecha_zInc;

			// Avanza paso a paso dentro de la interpolación actual
			i_curr_steps++;
		}
	}
}


///////////////* FIN KEYFRAMES*////////////////////////////



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, -0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -6.0f, 0.0f, 0.5f, 0.5f);

	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	//Modelos snorlax
	CuerpoSnorlax_M = Model();
	CuerpoSnorlax_M.LoadModel("Models/cabezasnorlax.fbx");
	PiernaDerecha_M = Model();
	PiernaDerecha_M.LoadModel("Models/piederechosnorlax.fbx");
	PiernaIzquierda_M = Model();
	PiernaIzquierda_M.LoadModel("Models/pieizquierdosnorlax.fbx");
	BrazoIzquierdo_M = Model();
	BrazoIzquierdo_M.LoadModel("Models/brazoizquierdosnorlax.fbx");
	BrazoDerecho_M = Model();
	BrazoDerecho_M.LoadModel("Models/brazoderechosnorlax.fbx");


	//Modelos Braviary
	Braviary_M = Model();
	Braviary_M.LoadModel("Models/braviary.fbx");

	AlaDerechaB_M = Model();
	AlaDerechaB_M.LoadModel("Models/aladerechaB.fbx");

	AlaIzquierdaB_M = Model();
	AlaIzquierdaB_M.LoadModel("Models/alaizquierdaB.fbx");	


	//Modelos Archen
	Archen_M = Model();
	Archen_M.LoadModel("Models/archen.fbx");	

	AlaDerechaA_M = Model();
	AlaDerechaA_M.LoadModel("Models/aladerechaarchen.fbx");


	AlaIzquierdaA_M = Model();
	AlaIzquierdaA_M.LoadModel("Models/alaizquierdaarchen.fbx");	



	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");



	std::vector<std::string> skyboxFaces;

	skyboxFaces.push_back("Textures/Skybox/central2.png");
	skyboxFaces.push_back("Textures/Skybox/izquierda.png");
	skyboxFaces.push_back("Textures/Skybox/abajo.png");
	skyboxFaces.push_back("Textures/Skybox/arriba.png");
	skyboxFaces.push_back("Textures/Skybox/central1.png");
	skyboxFaces.push_back("Textures/Skybox/derecha.png");


	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;
	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);

	//---------PARA TENER KEYFRAMES GUARDADOS NO VOLATILES QUE SIEMPRE SE UTILIZARAN SE DECLARAN AQUÍ

	// ---------------- KEYFRAMES SNORLAX CAMINANDO Y GIRANDO ----------------

// ===============================
// === FRAME 0: posición inicial ===
// ===============================

// --- CUERPO ---
	KeyFrame[0].movCuerpoSnorlax_x = 0.0f;
	KeyFrame[0].movCuerpoSnorlax_z = 0.0f;
	KeyFrame[0].giroSnorlax = 0.0f;

	// --- BRAZOS ---
	KeyFrame[0].giroBrazoIzquierdo_x = 0.0f;
	KeyFrame[0].giroBrazoIzquierdo_z = 45.0f;
	KeyFrame[0].giroBrazoDerecho_x = 0.0f;
	KeyFrame[0].giroBrazoDerecho_z = -45.0f;

	// --- PIERNAS ---
	KeyFrame[0].giroPiernaIzquierda_x = 0.0f;
	KeyFrame[0].giroPiernaIzquierda_z = -20.0f;

	KeyFrame[0].giroPiernaDerecha_x = 0.0f;
	KeyFrame[0].giroPiernaDerecha_z = 20.0f;



	// ===============================
	// === FRAME 1: paso hacia +X ===
	// ===============================

	// --- CUERPO ---
	KeyFrame[1].movCuerpoSnorlax_x = 2.0f;
	KeyFrame[1].movCuerpoSnorlax_z = 0.0f;
	KeyFrame[1].giroSnorlax = 0.0f;

	// --- BRAZOS ---
	KeyFrame[1].giroBrazoIzquierdo_x = 0.0f;
	KeyFrame[1].giroBrazoIzquierdo_z = -45.0f;
	KeyFrame[1].giroBrazoDerecho_x = 0.0f;
	KeyFrame[1].giroBrazoDerecho_z = 45.0f;

	// --- PIERNAS ---
	KeyFrame[1].giroPiernaIzquierda_x = 0.0f;
	KeyFrame[1].giroPiernaIzquierda_z = 20.0f;
	KeyFrame[1].giroPiernaDerecha_x = 0.0f;
	KeyFrame[1].giroPiernaDerecha_z = -20.0f;



	// ============================================================
// === FRAME 2: final del paso +X y empieza giro hacia -Z ===
// ============================================================

// --- CUERPO ---
	KeyFrame[2].movCuerpoSnorlax_x = 4.0f;
	KeyFrame[2].movCuerpoSnorlax_z = 0.0f;
	KeyFrame[2].giroSnorlax = 45.0f; // empieza el giro

	// --- BRAZOS ---
	KeyFrame[2].giroBrazoIzquierdo_z = 45.0f;
	KeyFrame[2].giroBrazoIzquierdo_x = 45.0f;

	KeyFrame[2].movBrazoIzquierdo_x = -0.8f;
	KeyFrame[2].movBrazoIzquierdo_z = 0.8f;

	KeyFrame[2].giroBrazoDerecho_x = 45.0f;
	KeyFrame[2].giroBrazoDerecho_z = -45.0f;

	KeyFrame[2].movBrazoDerecho_x = 0.8f;
	KeyFrame[2].movBrazoDerecho_z = -0.8f;

	// --- PIERNAS --- 
	KeyFrame[2].giroPiernaIzquierda_x = 45.0f;
	KeyFrame[2].giroPiernaIzquierda_z = -20.0f; // como brazo derecho (-Z)

	KeyFrame[2].movPiernaIzquierda_x = 0.2f;
	KeyFrame[2].movPiernaIzquierda_z = -0.8f;

	KeyFrame[2].giroPiernaDerecha_x = 45.0f;
	KeyFrame[2].giroPiernaDerecha_z = 20.0f; // como brazo izquierdo (+Z)
	KeyFrame[2].movPiernaDerecha_x = -0.8f;
	KeyFrame[2].movPiernaDerecha_z = 0.8f;



	// ====================================================
	// === FRAME 3: termina el giro, mira hacia -Z ===
	// ====================================================

	// --- CUERPO ---
	KeyFrame[3].movCuerpoSnorlax_x = 4.0f;
	KeyFrame[3].movCuerpoSnorlax_z = -0.5f;
	KeyFrame[3].giroSnorlax = 90.0f;

	// --- BRAZOS ---
	KeyFrame[3].giroBrazoIzquierdo_z = -45.0f;
	KeyFrame[3].giroBrazoIzquierdo_x = 90.0f;

	KeyFrame[3].movBrazoIzquierdo_x = -0.8f;
	KeyFrame[3].movBrazoIzquierdo_z = 0.8f;

	KeyFrame[3].giroBrazoDerecho_x = 90.0f;
	KeyFrame[3].giroBrazoDerecho_z = 45.0f;

	KeyFrame[3].movBrazoDerecho_x = 0.8f;
	KeyFrame[3].movBrazoDerecho_z = -0.8f;

	// --- PIERNAS ---
	KeyFrame[3].giroPiernaIzquierda_x = 90.0f;
	KeyFrame[3].giroPiernaIzquierda_z = 45.0f;

	KeyFrame[3].movPiernaIzquierda_x = 0.2f;
	KeyFrame[3].movPiernaIzquierda_z = -0.8f;

	KeyFrame[3].giroPiernaDerecha_x = 90.0f;
	KeyFrame[3].giroPiernaDerecha_z = -45.0f;
	KeyFrame[3].movPiernaDerecha_x = -0.8f;
	KeyFrame[3].movPiernaDerecha_z = 0.8f;



	// =======================================
	// === FRAME 4: camina hacia -Z ===
	// =======================================

	// --- CUERPO ---
	KeyFrame[4].movCuerpoSnorlax_x = 4.0f;
	KeyFrame[4].movCuerpoSnorlax_z = -3.5f;
	KeyFrame[4].giroSnorlax = 90.0f;

	// --- BRAZOS ---
	KeyFrame[4].giroBrazoIzquierdo_x = 90.0f;
	KeyFrame[4].giroBrazoIzquierdo_z = 45.0f;

	KeyFrame[4].movBrazoIzquierdo_x = -0.8f;
	KeyFrame[4].movBrazoIzquierdo_z = 0.8f;

	KeyFrame[4].giroBrazoDerecho_x = 90.0f;
	KeyFrame[4].giroBrazoDerecho_z = -45.0f;

	KeyFrame[4].movBrazoDerecho_x = 0.8f;
	KeyFrame[4].movBrazoDerecho_z = -0.8f;

	// --- PIERNAS --- 
	KeyFrame[4].giroPiernaIzquierda_x = 90.0f;
	KeyFrame[4].giroPiernaIzquierda_z = -20.0f;

	KeyFrame[4].movPiernaIzquierda_x = 0.2f;
	KeyFrame[4].movPiernaIzquierda_z = -0.8f;

	KeyFrame[4].giroPiernaDerecha_x = 90.0f;
	KeyFrame[4].giroPiernaDerecha_z = 20.0f;

	KeyFrame[4].movPiernaDerecha_x = -0.8f;
	KeyFrame[4].movPiernaDerecha_z = 0.8f;



	// =======================================
	// === FRAME 5: sigue avanzando hacia -Z ===
	// =======================================

	// --- CUERPO ---
	KeyFrame[5].movCuerpoSnorlax_x = 4.0f;
	KeyFrame[5].movCuerpoSnorlax_z = -7.0f;
	KeyFrame[5].giroSnorlax = 90.0f;

	// --- BRAZOS ---
	KeyFrame[5].giroBrazoIzquierdo_x = 90.0f;
	KeyFrame[5].giroBrazoIzquierdo_z = -45.0f;

	KeyFrame[5].movBrazoIzquierdo_x = -0.8f;
	KeyFrame[5].movBrazoIzquierdo_z = 0.8f;

	KeyFrame[5].giroBrazoDerecho_x = 90.0f;
	KeyFrame[5].giroBrazoDerecho_z = 45.0f;

	KeyFrame[5].movBrazoDerecho_x = 0.8f;
	KeyFrame[5].movBrazoDerecho_z = -0.8f;

	// --- PIERNAS --- (brazos invertidos)
	KeyFrame[5].giroPiernaIzquierda_x = 90.0f;
	KeyFrame[5].giroPiernaIzquierda_z = 45.0f;

	KeyFrame[5].movPiernaIzquierda_x = 0.0f;
	KeyFrame[5].movPiernaIzquierda_z = -0.8f;

	KeyFrame[5].giroPiernaDerecha_x = 90.0f;
	KeyFrame[5].giroPiernaDerecha_z = -45.0f;

	KeyFrame[5].movPiernaDerecha_x = -0.8f;
	KeyFrame[5].movPiernaDerecha_z = 0.8f;



	// =======================================
	// === FRAME 6: último paso hacia -Z ===
	// =======================================

	// --- CUERPO ---
	KeyFrame[6].movCuerpoSnorlax_x = 4.0f;
	KeyFrame[6].movCuerpoSnorlax_z = -10.0f;
	KeyFrame[6].giroSnorlax = 90.0f;

	// --- BRAZOS ---
	KeyFrame[6].giroBrazoIzquierdo_x = 90.0f;
	KeyFrame[6].giroBrazoIzquierdo_z = 45.0f;

	KeyFrame[6].movBrazoIzquierdo_x = -0.8f;
	KeyFrame[6].movBrazoIzquierdo_z = 0.8f;

	KeyFrame[6].giroBrazoDerecho_x = 90.0f;
	KeyFrame[6].giroBrazoDerecho_z = -45.0f;

	KeyFrame[6].movBrazoDerecho_x = 0.8f;
	KeyFrame[6].movBrazoDerecho_z = -0.8f;

	// --- PIERNAS 
	KeyFrame[6].giroPiernaIzquierda_x = 90.0f;
	KeyFrame[6].giroPiernaIzquierda_z = -20.0f;

	KeyFrame[6].movPiernaIzquierda_x = 0.2f;
	KeyFrame[6].movPiernaIzquierda_z = -0.8f;

	KeyFrame[6].giroPiernaDerecha_x = 90.0f;
	KeyFrame[6].giroPiernaDerecha_z = 20.0f;

	KeyFrame[6].movPiernaDerecha_x = -0.8f;
	KeyFrame[6].movPiernaDerecha_z = 0.8f;


	//Generando frames de forma infinita
	// =======================================
// === FRAME 7 al 26: avance hacia -Z ===
// =======================================

	for (int i = 7; i <= 26; i++) {
		KeyFrame[i] = KeyFrame[6]; // Copia base del frame 6
	}

	// === Movimiento del cuerpo
	KeyFrame[7].movCuerpoSnorlax_z = -13.0f;
	KeyFrame[8].movCuerpoSnorlax_z = -16.0f;
	KeyFrame[9].movCuerpoSnorlax_z = -19.0f;
	KeyFrame[10].movCuerpoSnorlax_z = -22.0f;
	KeyFrame[11].movCuerpoSnorlax_z = -25.0f;
	KeyFrame[12].movCuerpoSnorlax_z = -28.0f;
	KeyFrame[13].movCuerpoSnorlax_z = -31.0f;
	KeyFrame[14].movCuerpoSnorlax_z = -34.0f;
	KeyFrame[15].movCuerpoSnorlax_z = -37.0f;
	KeyFrame[16].movCuerpoSnorlax_z = -40.0f;
	KeyFrame[17].movCuerpoSnorlax_z = -43.0f;
	KeyFrame[18].movCuerpoSnorlax_z = -46.0f;
	KeyFrame[19].movCuerpoSnorlax_z = -49.0f;
	KeyFrame[20].movCuerpoSnorlax_z = -52.0f;
	KeyFrame[21].movCuerpoSnorlax_z = -55.0f;
	KeyFrame[22].movCuerpoSnorlax_z = -58.0f;
	KeyFrame[23].movCuerpoSnorlax_z = -61.0f;
	KeyFrame[24].movCuerpoSnorlax_z = -64.0f;
	KeyFrame[25].movCuerpoSnorlax_z = -67.0f;
	KeyFrame[26].movCuerpoSnorlax_z = -70.0f;

	// === Movimiento alternado de brazos y piernas ===
	for (int i = 7; i <= 26; i++) {
		if (i % 2 == 0) {
			// Movimiento 1: brazo izquierdo adelante, pierna derecha adelante
			KeyFrame[i].giroBrazoIzquierdo_z = 45.0f;
			KeyFrame[i].giroBrazoDerecho_z = -45.0f;

			KeyFrame[i].giroPiernaIzquierda_z = -20.0f;
			KeyFrame[i].giroPiernaDerecha_z = 20.0f;
		}
		else {
			// Movimiento 2: brazo izquierdo atrás, pierna derecha atrás
			KeyFrame[i].giroBrazoIzquierdo_z = -45.0f;
			KeyFrame[i].giroBrazoDerecho_z = 45.0f;

			KeyFrame[i].giroPiernaIzquierda_z = 20.0f;
			KeyFrame[i].giroPiernaDerecha_z = -20.0f;
		}
	}



	


	//Se agregan nuevos frames 


		printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora par¿a reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
		printf("3.-Presiona L para guardar frame\n4.-Presiona P para habilitar guardar nuevo frame\n5.-Presiona 1 para mover en X\n6.-Presiona 2 para habilitar mover en X\n");
		printf("7.-Presiona G para guardar en el archivo\n8.-Presiona 3 para mover en-x\n9.-Presiona 4 para volver a habilitar mover en -x\n10.-Presiona 5 para mover en y\n");
		printf("11.-Presiona 6 para habilitar volver a mover en y \n12.-Presiona 7  para mover en -y\n13.-Presiona 8 para volver a habilitar mover en -y\n");
		printf("14.-Presiona M para hacer que rote en 180 grados \n15.-Presiona N para habilitar la rotación \n16-. Presionar C para Cargar txt pasado \n17-. Presionar V para habilitar Cargar txt pasado  ");



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		glm::vec3 lowerLight = glm::vec3(0.0f,0.0f,0.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 8.0f * deltaTime;
		
	
		//dragonavance

		anguloAlaBraviary += 8.0f * deltaTime;
		float aleteoBraviary = sin(glm::radians(anguloAlaBraviary)) * 30; // 30° es la amplitud del movimiento

		float aleteoArchen = sin(glm::radians(anguloAlaBraviary)) * 60;
		// BANDERA DE AVANCE Y REGRESO

		if (avanzabraviary) {
			if (braviaryavance > -250.0f) {
				braviaryavance -= movOffsetbraviary * deltaTime;
			}
			else {
				avanzabraviary = false;
				rotacionBraviary = 180.0f; // Girar hacia atrás
			}
		}
		else {
			if (braviaryavance < 325.0f) {
				braviaryavance += movOffsetbraviary * deltaTime;
			}
			else {
				avanzabraviary = true;
				rotacionBraviary = 0.0f; // Volver al frente
			}
		}






		if (movCoche < 30.0f)
		{
			movCoche -= movOffset * deltaTime;
			//printf("avanza%f \n ",movCoche);
		}
		rotllanta += rotllantaOffset * deltaTime;

	

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//-------Para Keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		model=glm::mat4(1.0);
		modelaux= glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();
		  
		// ----- SNORLAX ---------

		// Cuerpo Snorlax
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f + movCuerpoSnorlax_x, 1.32f , -2.0f + movCuerpoSnorlax_z));
		modelaux = model;
		model = glm::rotate(model, giroSnorlax * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Giro del cuerpo
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoSnorlax_M.RenderModel();


		// --- Pierna derecha ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.2f + movPiernaDerecha_x,-1.1f,-0.5f + movPiernaDerecha_z
		));
		model = glm::rotate(model, giroPiernaDerecha_x * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Giro eje X
		model = glm::rotate(model, giroPiernaDerecha_z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // Giro eje Z
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaDerecha_M.RenderModel();


		// --- Pierna izquierda ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.2f + movPiernaIzquierda_x,-1.1f,0.5f + movPiernaIzquierda_z));
		model = glm::rotate(model, giroPiernaIzquierda_x * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Giro eje X
		model = glm::rotate(model, giroPiernaIzquierda_z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // Giro eje Z
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PiernaIzquierda_M.RenderModel();


		// --- Brazo izquierdo ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f + movBrazoIzquierdo_x,0.6f,-0.7f + movBrazoIzquierdo_z));
		model = glm::rotate(model, giroBrazoIzquierdo_x * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Giro eje X
		model = glm::rotate(model, giroBrazoIzquierdo_z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // Giro eje Z
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoIzquierdo_M.RenderModel();


		// --- Brazo derecho ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f + movBrazoDerecho_x,0.6f,0.75f + movBrazoDerecho_z));
		model = glm::rotate(model, giroBrazoDerecho_x * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Giro eje X
		model = glm::rotate(model, giroBrazoDerecho_z * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // Giro eje Z
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoDerecho_M.RenderModel();


		//-----------------------------------------




		//-----BRAVIARY -----

		// Matriz base del Braviary
		glm::mat4 modelBraviary = glm::mat4(1.0f);
		modelBraviary = glm::translate(modelBraviary, glm::vec3(1.05f - braviaryavance, 50.0f + sin(glm::radians(angulovaria)), 6.0f));
		modelBraviary = glm::scale(modelBraviary, glm::vec3(0.3f, 0.3f, 0.3f));
		modelBraviary = glm::rotate(modelBraviary, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelBraviary = glm::rotate(modelBraviary, glm::radians(rotacionBraviary), glm::vec3(0.0f, 0.0f, 1.0f)); // <<--- AQUÍ
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelBraviary));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Braviary_M.RenderModel();

		//AlaDerecha
		glm::mat4 modelAlaDerechaB = modelBraviary;
		modelAlaDerechaB = glm::translate(modelAlaDerechaB, glm::vec3(6.5f, -3.0f, 9.5f));
		modelAlaDerechaB = glm::rotate(modelAlaDerechaB, glm::radians(-aleteoBraviary), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelAlaDerechaB));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		AlaDerechaB_M.RenderModel();

		//Alaizquierdda
		glm::mat4 modelAlaIzquierdaB = modelBraviary; // también hereda del cuerpo
		modelAlaIzquierdaB = glm::translate(modelAlaIzquierdaB, glm::vec3(6.5f, 3.0f, 9.5f));
		modelAlaIzquierdaB = glm::rotate(modelAlaIzquierdaB, glm::radians(aleteoBraviary), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelAlaIzquierdaB));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		AlaIzquierdaB_M.RenderModel();




		//-----Archen ----

		glm::mat4 modelArchen = glm::mat4(1.0f);
		modelArchen = glm::translate(modelArchen, glm::vec3(1.05f - braviaryavance, 50.0f + sin(glm::radians(angulovaria)) , 20.0f));
		modelArchen = glm::scale(modelArchen, glm::vec3(0.03f, 0.03f, 0.03f));
		modelArchen = glm::rotate(modelArchen, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelArchen = glm::rotate(modelArchen, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelArchen = glm::rotate(modelArchen, glm::radians(rotacionBraviary), glm::vec3(0.0f, 0.0f, 1.0f)); // <<--- AQUÍ
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelArchen));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Archen_M.RenderModel();

		//AlaDerecha
		glm::mat4 modelAlaDerechaA = modelArchen;
		modelAlaDerechaA = glm::translate(modelAlaDerechaA, glm::vec3(-5.5f, 5.0f, 25.0f));
		modelAlaDerechaA = glm::rotate(modelAlaDerechaA, glm::radians(-aleteoArchen), glm::vec3(1.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelAlaDerechaA));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		AlaDerechaA_M.RenderModel();

		//Alaizquierdda
		glm::mat4 modelAlaIzquierdaA = modelArchen; // también hereda del cuerpo
		modelAlaIzquierdaA = glm::translate(modelAlaIzquierdaA, glm::vec3(5.5f, 5.0f,25.0f));
		modelAlaIzquierdaA = glm::rotate(modelAlaIzquierdaA, glm::radians(aleteoArchen), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelAlaIzquierdaA));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		AlaIzquierdaA_M.RenderModel();




		model = glm::mat4(1.0);
		posblackhawk=glm::vec3(posXavion + movAvion_x, posYavion + movAvion_y, posZavion);
		model = glm::translate(model, posblackhawk);
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, giroAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//color = glm::vec3(0.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();
		
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}



void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;

			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1 && reproduciranimacion>0)
		{
			printf("Ya puedes reproducir de nuevo la animación con la tecla de barra espaciadora'\n");
			reproduciranimacion = 0;
			
		}
	}


	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movAvion_x es: %f\n", movAvion_x);
			printf("movAvion_y es: %f\n", movAvion_y);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}


	if (keys[GLFW_KEY_G])
	{
		if (guardoArchivo < 1) {
			guardoArchivo++;
			GuardarKeyframesEnArchivo(KeyFrame, FrameIndex, "keyframes_guardados.txt");
			printf("Keyframes agregados al archivo. Presiona P para poder guardar nuevamente.\n");
			
		}
			
	}

	if (keys[GLFW_KEY_P] )
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
			guardoArchivo = 0;
			reinicioFrame++;
			printf("Ya puedes guardar otro frame presionando la tecla L'\n");
		}
	}

	// --- CARGAR ARCHIVOS PASADOS ---
	if (keys[GLFW_KEY_C])
	{
		if (cicloCarga < 1)
		{
			CargarKeyframesDesdeArchivo(KeyFrame, FrameIndex, "keyframes_guardados.txt");
			resetElements(); // opcional
			cicloCarga++;
			banderaCarga = 0; // permite volver a usar la tecla V una vez
			printf("Keyframes cargados. Presiona V para habilitar nueva carga.\n");
		}
	}

	// --- HABILITAR NUEVA CARGA ---
	if (keys[GLFW_KEY_V])
	{
		if (banderaCarga < 1)
		{
			cicloCarga = 0;     // resetea para poder volver a cargar
			banderaCarga++;     // evita que se imprima infinitamente
			printf("Ya puedes volver a cargar animaciones con C.\n");
		}
	}

	// --- ROTACION AVION 
	// --- ROTACIÓN 180° CON TECLA M ---
	if (keys[GLFW_KEY_M])
	{
		if (ciclo8 < 1) // nuevo contador
		{
			giroAvion += 180.0f;

			// Aseguramos que el ángulo se mantenga en el rango [0, 360)
			if (giroAvion >= 360.0f)
				giroAvion -= 360.0f;

			printf("\nEl avión giró 180 grados. giroAvion = %f\n", giroAvion);
			ciclo8++;
			ciclo9 = 0;
			printf("Presiona la tecla N para habilitar nuevamente el giro.\n");
		}
	}

	if (keys[GLFW_KEY_N])
	{
		if (ciclo9 < 1)
		{
			ciclo8 = 0;
			ciclo9++;
			printf("Ya puedes volver a rotar el avión con la tecla M.\n");
		}
	}
	// --- MOVIMIENTO EN +X ---	
	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x += 1.0f;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 2 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_2])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 1\n");
		}
	}

	// --- MOVIMIENTO EN -X ---
	if (keys[GLFW_KEY_3])
	{
		if (ciclo3 < 1)
		{
			movAvion_x -= 1.0f;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			ciclo3++;
			ciclo4 = 0;
			printf("Presiona la tecla 4 para habilitar de nuevo el cambio en X (-)\n");
		}
	}

	if (keys[GLFW_KEY_4])
	{
		if (ciclo4 < 1)
		{
			ciclo3 = 0;
			ciclo4++;
			printf("Ya puedes volver a modificar -X (tecla 3)\n");
		}
	}

	// --- MOVIMIENTO EN +Y ---
	if (keys[GLFW_KEY_5])
	{
		if (ciclo5 < 1)
		{
			movAvion_y += 1.0f;
			printf("\n movAvion_y es: %f\n", movAvion_y);
			ciclo5++;
			ciclo6 = 0;
			printf("Presiona la tecla 6 para habilitar de nuevo el cambio en Y (+)\n");
		}
	}

	if (keys[GLFW_KEY_6])
	{
		if (ciclo6 < 1)
		{
			ciclo5 = 0;
			ciclo6++;
			printf("Ya puedes volver a modificar +Y (tecla 5)\n");
		}
	}

	// --- MOVIMIENTO EN -Y ---
	if (keys[GLFW_KEY_7])
	{
		if (ciclo7 < 1)
		{
			movAvion_y -= 1.0f;
			printf("\n movAvion_y es: %f\n", movAvion_y);
			ciclo7++;
			printf("Presiona la tecla 8 para habilitar de nuevo el cambio en Y (-)\n");
		}
	}

	if (keys[GLFW_KEY_8])
	{
		if (ciclo7 > 0)
		{
			ciclo7 = 0;
			printf("Ya puedes volver a modificar -Y (tecla 7)\n");
		}
	}

}
