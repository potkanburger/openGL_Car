// Include standard headers
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <string.h>
#include <windows.h>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;


#include <car_functions.h>



int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1920, 1080, "Car Jacquet", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);// Ici, nous sert comme "couleur du ciel"

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 1000 units
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	// Camera matrix
	glm::vec3 CameraPos = glm::vec3(-8, 10, 0);
	glm::vec3 CarOrigin = glm::vec3(0, 0, 0);
	glm::mat4 View = glm::lookAt(
		CameraPos, // originale Camera is at (-8,10,0), in World Space
		CarOrigin, // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices

	// On crée l'ensemble des MVP particuliers dont on aura besoin pour l'ensemble des objets/vertices
	//Initialisation a la Projection Vue Modele initiale pour etre sur de ne pas faire appel a un MVP nul
	glm::mat4 MVP = Projection * View * Model;
	glm::mat4 MVP_RoueAvGauche = Projection * View * Model;
	glm::mat4 MVP_RoueAvDroite = Projection * View * Model;
	glm::mat4 MVP_RoueArDroite = Projection * View * Model;
	glm::mat4 MVP_RoueArGauche = Projection * View * Model;
	glm::mat4 MVP_Voiture = Projection * View * Model;
	glm::mat4 MVP_Floor = Projection * View * Model;
	

	// Couleurs des hitboxs de certains objets pour tests et démonstration visuelle de la collision
	static GLfloat g_color_buffer_data_HitboxVoiture[6 * 3];
	for (int v = 0; v< 6; v++){
		g_color_buffer_data_HitboxVoiture[3 * v + 0] = 0.60f;
		g_color_buffer_data_HitboxVoiture[3 * v + 1] = 0.10f;
		g_color_buffer_data_HitboxVoiture[3 * v + 2] = 0.05f;
	}

	static GLfloat g_color_buffer_data_ObstacleTrou[6 * 3];
	for (int v = 0; v< 6; v++){
		g_color_buffer_data_HitboxVoiture[3 * v + 0] = 0.60f;
		g_color_buffer_data_HitboxVoiture[3 * v + 1] = 0.00f;
		g_color_buffer_data_HitboxVoiture[3 * v + 2] = 0.85f;
	}

	// --------------------------------------------LE SOL -------------------------------------------------------------

	static const GLfloat g_vertex_buffer_data_floor[] = {
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};


	static const GLfloat g_color_buffer_data_floor[] = {
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
		0.676f, 0.977f, 0.133f,
	};


	//Load de la voiture (carosserie) et attribution de la couleur (ici, rouge)

	std::vector< glm::vec3 > verticesVoiture;
	verticesVoiture = objLoader("car_base2_carrosserie3.obj");
	//verticesVoiture = objLoader("voiture.obj");
	int sizeVoiture = verticesVoiture.size();
	static GLfloat g_voiture_data[10000];
	static GLfloat g_voiture_color[10000];
	float max_x = 0.0f;//valeur max en x de la voiture
	float max_z = 0.0f;//valeur max en z de la voiture


	for (int v = 0; v < sizeVoiture; v++){
		//Calcul des maximums en x et z de la voiture pour créer une hitbox 2d correspondant
		if (abs(verticesVoiture.at(v).x)>max_x)max_x = abs(verticesVoiture.at(v).x);
		if (abs(verticesVoiture.at(v).z)>max_z)max_z = abs(verticesVoiture.at(v).z);
		g_voiture_data[3 * v + 0] = verticesVoiture.at(v).x;
		g_voiture_data[3 * v + 1] = verticesVoiture.at(v).y;
		g_voiture_data[3 * v + 2] = verticesVoiture.at(v).z;

		g_voiture_color[3 * v + 0] = 0.90f;
		g_voiture_color[3 * v + 1] = 0.00f;
		g_voiture_color[3 * v + 2] = 0.00f;

	}
	max_x /= 4; // On rescale la voiture, il faut rescale la hitbox aussi.
	max_z /= 4;



	//Load de la voiture (roues) et attribution de la couleur (ici, bleue en général avec quelques variations pour bien visualiser les roues)

	std::vector< glm::vec3 > verticesRoue;
	verticesRoue = objLoader("roue.obj");
	int sizeRoue = verticesRoue.size();
	static GLfloat g_roue_data[10000];
	static GLfloat g_roue_color[10000];
	float tmp_c = 0.0f;
	float tmp_d = 0.50f;

	for (int v = 0; v < sizeRoue; v++){
		g_roue_data[3 * v + 0] = verticesRoue.at(v).x;
		g_roue_data[3 * v + 1] = verticesRoue.at(v).y;
		g_roue_data[3 * v + 2] = verticesRoue.at(v).z;
		if (v%3!=0){
			tmp_c = 1.0f;
			tmp_d = 0.0f;
		}
		else{
			tmp_c = 0.0f;
			tmp_d = 0.50f;
		}
		g_roue_color[3 * v + 0] = tmp_d;
		g_roue_color[3 * v + 1] = 0.0f;
		g_roue_color[3 * v + 2] = tmp_c;

	}

	//Load de la voiture (vitres) et attribution de la couleur (ici, gris-bleu océan)

	std::vector< glm::vec3 > verticesVitres;
	verticesVitres = objLoader("car_base2_vitre.obj");
	int sizeVitres = verticesVitres.size();
	static GLfloat g_vitres_data[10000];
	static GLfloat g_vitres_color[10000];

	for (int v = 0; v < sizeVitres; v++){
		g_vitres_data[3 * v + 0] = verticesVitres.at(v).x;
		g_vitres_data[3 * v + 1] = verticesVitres.at(v).y;
		g_vitres_data[3 * v + 2] = verticesVitres.at(v).z;
		
		g_vitres_color[3 * v + 0] = 0.40f;
		g_vitres_color[3 * v + 1] = 0.80f;
		g_vitres_color[3 * v + 2] = 0.67f;

	}


	//Load des barrieres et attribution de la couleur (ici, rose)

	std::vector< glm::vec3 > verticesBarriere;
	verticesBarriere = objLoader("barriere.obj");
	int sizeBarriere = verticesBarriere.size();
	static GLfloat g_barriere_data[10000];
	static GLfloat g_barriere_color[10000];
	float max_x_barriere = 0.0f;
	float max_z_barriere = 0.0f;

	for (int v = 0; v < sizeBarriere; v++){
		if (abs(verticesBarriere.at(v).x)>max_x_barriere)max_x_barriere = abs(verticesBarriere.at(v).x);
		if (abs(verticesBarriere.at(v).z)>max_z_barriere)max_z_barriere = abs(verticesBarriere.at(v).z);
		g_barriere_data[3 * v + 0] = verticesBarriere.at(v).x;
		g_barriere_data[3 * v + 1] = verticesBarriere.at(v).y;
		g_barriere_data[3 * v + 2] = verticesBarriere.at(v).z;

		g_barriere_color[3 * v + 0] = 1.00f;
		g_barriere_color[3 * v + 1] = 0.24f;
		g_barriere_color[3 * v + 2] = 0.59f;

	}

	max_x_barriere /= 4; // On rescale les barrieres, il faut rescale la hitbox aussi.
	max_z_barriere /= 4;

	// HITBOX Générale d'une barriere
	static const GLfloat g_vertex_buffer_data_HitboxBarriere[] = {
		-max_x_barriere, 0.0f, max_z_barriere,
		max_x_barriere, 0.0f, max_z_barriere,
		-max_x_barriere, 0.0f, -max_z_barriere,
		max_x_barriere, 0.0f, -max_z_barriere,
		-max_x_barriere, 0.0f, -max_z_barriere,
		max_x_barriere, 0.0f, max_z_barriere
	};


	//Creation de 5 barrieres:

	static GLfloat g_barriere1[10000];
	static GLfloat g_barriere2[10000];
	static GLfloat g_barriere3[10000];
	static GLfloat g_barriere4[10000];
	static GLfloat g_barriere5[10000];

	static GLfloat hitboxBar1[18];
	static GLfloat hitboxBar2[18];
	static GLfloat hitboxBar3[18];
	static GLfloat hitboxBar4[18];
	static GLfloat hitboxBar5[18];

	for (int v = 0; v < sizeBarriere; v++){
		g_barriere1[3 * v + 0] = g_barriere_data[3 * v + 0]+50;
		g_barriere1[3 * v + 1] = g_barriere_data[3 * v + 1];
		g_barriere1[3 * v + 2] = g_barriere_data[3 * v + 2]+60;

		g_barriere2[3 * v + 0] = g_barriere_data[3 * v + 0] + 130;
		g_barriere2[3 * v + 1] = g_barriere_data[3 * v + 1];
		g_barriere2[3 * v + 2] = g_barriere_data[3 * v + 2] + 70;

		g_barriere3[3 * v + 0] = g_barriere_data[3 * v + 0] + 150;
		g_barriere3[3 * v + 1] = g_barriere_data[3 * v + 1];
		g_barriere3[3 * v + 2] = g_barriere_data[3 * v + 2] - 30;

		g_barriere4[3 * v + 0] = g_barriere_data[3 * v + 0] + 10;
		g_barriere4[3 * v + 1] = g_barriere_data[3 * v + 1];
		g_barriere4[3 * v + 2] = g_barriere_data[3 * v + 2] - 90;

		g_barriere5[3 * v + 0] = g_barriere_data[3 * v + 0] + 100;
		g_barriere5[3 * v + 1] = g_barriere_data[3 * v + 1];
		g_barriere5[3 * v + 2] = g_barriere_data[3 * v + 2] + 300;
	}

	for (int v = 0; v < 6; v++){
		hitboxBar1[3 * v + 0] = g_vertex_buffer_data_HitboxBarriere[3 * v + 0]+50.0f/4;
		hitboxBar1[3 * v + 1] = g_vertex_buffer_data_HitboxBarriere[3 * v + 1];
		hitboxBar1[3 * v + 2] = g_vertex_buffer_data_HitboxBarriere[3 * v + 2]+60.0f/4;

		hitboxBar2[3 * v + 0] = g_vertex_buffer_data_HitboxBarriere[3 * v + 0]+130.0f/4;
		hitboxBar2[3 * v + 1] = g_vertex_buffer_data_HitboxBarriere[3 * v + 1];
		hitboxBar2[3 * v + 2] = g_vertex_buffer_data_HitboxBarriere[3 * v + 2]+70.0f/4;

		hitboxBar3[3 * v + 0] = g_vertex_buffer_data_HitboxBarriere[3 * v + 0]+150.0f/4;
		hitboxBar3[3 * v + 1] = g_vertex_buffer_data_HitboxBarriere[3 * v + 1];
		hitboxBar3[3 * v + 2] = g_vertex_buffer_data_HitboxBarriere[3 * v + 2]-30.0f/4;

		hitboxBar4[3 * v + 0] = g_vertex_buffer_data_HitboxBarriere[3 * v + 0]+10.0f/4;
		hitboxBar4[3 * v + 1] = g_vertex_buffer_data_HitboxBarriere[3 * v + 1];
		hitboxBar4[3 * v + 2] = g_vertex_buffer_data_HitboxBarriere[3 * v + 2]-90.0f/4;

		hitboxBar5[3 * v + 0] = g_vertex_buffer_data_HitboxBarriere[3 * v + 0]+100.0f/4;
		hitboxBar5[3 * v + 1] = g_vertex_buffer_data_HitboxBarriere[3 * v + 1];
		hitboxBar5[3 * v + 2] = g_vertex_buffer_data_HitboxBarriere[3 * v + 2]+300.0f/4;
	}


	// HITBOX de la voiture
	static GLfloat g_vertex_buffer_data_HitboxVoiture[] = {
		-max_x, 0.0f, max_z,
		max_x, 0.0f, max_z,
		-max_x, 0.0f, -max_z,
		max_x, 0.0f, -max_z,
		-max_x, 0.0f, -max_z,
		max_x, 0.0f, max_z
	};


	//HITBOX d'un trou
	static GLfloat g_vertex_buffer_data_ObstacleTrou[] = {
		3.0f, 0.0f, 1.0f,
		5.0f, 0.0f, 1.0f,
		3.0f, 0.0f, -1.0f,
		5.0f, 0.0f, -1.0f,
		3.0f, 0.0f, -1.0f,
		5.0f, 0.0f, 1.0f
	};


	//Association de nos vertices à leurs buffers respectifs

	GLuint blender;
	glGenBuffers(1, &blender);
	glBindBuffer(GL_ARRAY_BUFFER, blender);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_voiture_data), g_voiture_data, GL_STATIC_DRAW);

	GLuint colorblender;
	glGenBuffers(1, &colorblender);
	glBindBuffer(GL_ARRAY_BUFFER, colorblender);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_voiture_color), g_voiture_color, GL_STATIC_DRAW);

	GLuint vitresBlender;
	glGenBuffers(1, &vitresBlender);
	glBindBuffer(GL_ARRAY_BUFFER, vitresBlender);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vitres_data), g_vitres_data, GL_STATIC_DRAW);

	GLuint colorVitresblender;
	glGenBuffers(1, &colorVitresblender);
	glBindBuffer(GL_ARRAY_BUFFER, colorVitresblender);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vitres_color), g_vitres_color, GL_STATIC_DRAW);


	GLuint roueBlender;
	glGenBuffers(1, &roueBlender);
	glBindBuffer(GL_ARRAY_BUFFER, roueBlender);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_roue_data), g_roue_data, GL_STATIC_DRAW);

	GLuint colorRoueBlender;
	glGenBuffers(1, &colorRoueBlender);
	glBindBuffer(GL_ARRAY_BUFFER, colorRoueBlender);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_roue_color), g_roue_color, GL_STATIC_DRAW);

	

	GLuint vertexbufferHitboxVoiture;
	glGenBuffers(1, &vertexbufferHitboxVoiture);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferHitboxVoiture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_HitboxVoiture), g_vertex_buffer_data_HitboxVoiture, GL_STATIC_DRAW);


	GLuint colorbufferHitboxVoiture;
	glGenBuffers(1, &colorbufferHitboxVoiture);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferHitboxVoiture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_HitboxVoiture), g_color_buffer_data_HitboxVoiture, GL_STATIC_DRAW);

	GLuint vertexbufferObstacleTrou;
	glGenBuffers(1, &vertexbufferObstacleTrou);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferObstacleTrou);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_ObstacleTrou), g_vertex_buffer_data_ObstacleTrou, GL_STATIC_DRAW);


	GLuint colorbufferObstacleTrou;
	glGenBuffers(1, &colorbufferObstacleTrou);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferObstacleTrou);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_ObstacleTrou), g_color_buffer_data_ObstacleTrou, GL_STATIC_DRAW);

	GLuint vertexBar1;
	glGenBuffers(1, &vertexBar1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBar1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_barriere1), g_barriere1, GL_STATIC_DRAW);

	GLuint vertexBar2;
	glGenBuffers(1, &vertexBar2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBar2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_barriere2), g_barriere2, GL_STATIC_DRAW);

	GLuint vertexBar3;
	glGenBuffers(1, &vertexBar3);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBar3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_barriere3), g_barriere3, GL_STATIC_DRAW);

	GLuint vertexBar4;
	glGenBuffers(1, &vertexBar4);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBar4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_barriere4), g_barriere4, GL_STATIC_DRAW);

	GLuint vertexBar5;
	glGenBuffers(1, &vertexBar5);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBar5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_barriere5), g_barriere5, GL_STATIC_DRAW);

	GLuint colorBarriere;
	glGenBuffers(1, &colorBarriere);
	glBindBuffer(GL_ARRAY_BUFFER, colorBarriere);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_barriere_color), g_barriere_color, GL_STATIC_DRAW);


	GLuint vertexbufferfloor;
	glGenBuffers(1, &vertexbufferfloor);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferfloor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_floor), g_vertex_buffer_data_floor, GL_STATIC_DRAW);


	GLuint colorbufferfloor;
	glGenBuffers(1, &colorbufferfloor);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferfloor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_floor), g_color_buffer_data_floor, GL_STATIC_DRAW);


	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	float jump_power = 0.0f;
	float jump_fall = 0.0f;
	float sol = 0.0f;
	float NOS = 100.0f;
	float wait_NOS = 0.0f;

	// Valeurs par frame
	float pas = 0.1f;//vitesse en cours
	float pas_init = 0.1f;//vitesse de demarrage
	float pas_max = 0.5f;//vitesse max
	float pas_lat = 0.1f;//decalage lateral max
	float r_avant_arriere = 0.0f;//rotation des roues avant-arriere
	float r_roues_gd = 0.0f; //rotation roues gauche-droite
	float r_voiture_gd = 0.0f; //rotation voiture gauche-droite
	bool collision = false;// Y'a t'il une collision en cours ?

	glm::mat4 myMatrix = Model;//represente les translations et roations successives sur le Modele 
	glm::vec3 VectVitesse = glm::vec3(x, z, y);//Vecteur de la frame en cours, represente le trajet qui devra etre effectué avant la prochaine frame
	glm::vec3 VectCamera = glm::vec3(x, z, y);//Vecteur de la frame en cours, represente le trajet qui devra etre effectué avant la prochaine frame pour la caméra
	float rot_Camera = 0.0f;// rotation de la caméra
	glm::vec3 DernierVectVitesseNonNul = glm::vec3(x, z, y);//Represente le dernier vecteur vitesse non nul. Permet de revenir en arriere lors d'une collision


	vector<obs> obstacles;// un vecteur contenant nos obstacles
	int obstacleEnCollisionSphere;//indice de l'objet en collision

	//Pour chacun des obstacles on determine son centre et son rayon pour créer des boundaries spheres

	obs voiture = { voiture.rayon = 0.0f, voiture.centre = glm::vec4(0.0f) };
	obs obs1 = { obs1.rayon = 0.0f, obs1.centre = glm::vec4(0.0f)};
	get_centre_rayon(g_vertex_buffer_data_ObstacleTrou, &obs1.rayon, &obs1.centre);
	get_centre_rayon(g_vertex_buffer_data_HitboxVoiture, &voiture.rayon, &voiture.centre);
	obstacles.push_back(obs1);


	obs bar1 = { bar1.rayon = 0.0f, bar1.centre = glm::vec4(0.0f)};
	obs bar2 = { bar2.rayon = 0.0f, bar2.centre = glm::vec4(0.0f)};
	obs bar3 = { bar3.rayon = 0.0f, bar3.centre = glm::vec4(0.0f)};
	obs bar4 = { bar4.rayon = 0.0f, bar4.centre = glm::vec4(0.0f)};
	obs bar5 = { bar5.rayon = 0.0f, bar5.centre = glm::vec4(0.0f)};
	
	get_centre_rayon(hitboxBar1, &bar1.rayon, &bar1.centre);
	get_centre_rayon(hitboxBar2, &bar2.rayon, &bar2.centre);
	get_centre_rayon(hitboxBar3, &bar3.rayon, &bar3.centre);
	get_centre_rayon(hitboxBar4, &bar4.rayon, &bar4.centre);
	get_centre_rayon(hitboxBar5, &bar5.rayon, &bar5.centre);

	obstacles.push_back(bar1);
	obstacles.push_back(bar2);
	obstacles.push_back(bar3);
	obstacles.push_back(bar4);
	obstacles.push_back(bar5);


	//On stocke tous nos obstacles dans un seul tableau, chaque obstacle est accessible par son indice
	GLfloat vertexsObstacles[6][18];
	
	for (int j = 0; j < 18; j++){
		vertexsObstacles[0][j] = g_vertex_buffer_data_ObstacleTrou[j];
		vertexsObstacles[1][j] = hitboxBar1[j];
		vertexsObstacles[2][j] = hitboxBar2[j];
		vertexsObstacles[3][j] = hitboxBar3[j];
		vertexsObstacles[4][j] = hitboxBar4[j];
		vertexsObstacles[5][j] = hitboxBar5[j];
	}
		

	do{
		//Pour chaque frame on initialise x, y et z à 0. Ils formeront le nouveau vecteur vitesse
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;

		//Si on est au moins mort, la vitesse revient à la vitesse de demarrage
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE){
			pas = pas_init;
		}

		// Move forward
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
			//On avance, on fait tourner les roues, on accelere si on peut(cad si on est pas a vitesse max [reste a max], ou à une vitesse impossible auquel cas on va a la vitesse de demarrage)
			r_avant_arriere -= 5.0f;
			if (pas < 0.0f){
				pas = pas_init;
			}
			else{
				pas = pas*1.05f;
			}

			if (pas > pas_max){
				pas = pas_max;
			}
			x = pas;

			// Gestion de la rotation pour un état de mouvement avant
			//r_voiture_gd represente la rotation de la voiture gauche-droite
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
				if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
					if (r_voiture_gd > 0.0f){
						r_voiture_gd = 0.0f;
					}
					r_voiture_gd -= pas_lat;
					if (r_voiture_gd<-30 * pas_lat){
						r_voiture_gd = -30 * pas_lat;
					}

				}
				else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
					if (r_voiture_gd < 0.0f){
						r_voiture_gd = 0.0f;
					}

					r_voiture_gd += pas_lat;

					if (r_voiture_gd> 30 * pas_lat){
						r_voiture_gd = 30 * pas_lat;
					}
				}
			}

		}
		// Move backward
		// Meme raisonnement pour le mouvement vers l'arriere avec adaption des cas (symetrie)
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
			r_avant_arriere += 5.0f;
			if (pas > 0.0f){
				pas = -pas_init;
			}
			else{
				pas = pas*1.05f;
			}

			if (pas < -pas_max){
				pas = -pas_max;
			}
			x = pas;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
				if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
					r_voiture_gd = 0.0f;
				}
				else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
					if (r_voiture_gd > 0.0f){
						r_voiture_gd = 0.0f;
					}

					r_voiture_gd -= pas_lat;
					if (r_voiture_gd<-30 * pas_lat){
						r_voiture_gd = -30 * pas_lat;
					}
				}
				else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){

					if (r_voiture_gd < 0.0f){
						r_voiture_gd = 0.0f;
					}

					r_voiture_gd += pas_lat;
					if (r_voiture_gd> 30 * pas_lat){
						r_voiture_gd = 30 * pas_lat;
					}
				}
			}
		}


		//Strafe Left-Right (Wheels)
		//On fait tourner les roues, independamment du fait d'avancer ou non)
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){

			// Strafe right
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
				r_roues_gd -= 2.0f;

				if (r_roues_gd<-30.0f){
					r_roues_gd = -30.0f;
				}


			}

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
				r_voiture_gd = 0.0f;
			}


			// Strafe left
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
				r_roues_gd += 2.0f;
				if (r_roues_gd>30.0f){
					r_roues_gd = 30.0f;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
				r_voiture_gd = 0.0f;
			}

		}// retour volant
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE){

			if (r_roues_gd>0.0f){
				r_roues_gd -= 2.0f;
			}
			else if (r_roues_gd<0.0f){
				r_roues_gd += 2.0f;
			}

			if (abs(r_voiture_gd)>0.0f){
				r_voiture_gd = 0.0f;
			}


		}
		//On essaye d'eviter d'avoir un cas ou toutes les directions sont demandée en meme temps. Dans ce cas: aucune rotation
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
			r_voiture_gd = 0.0f;
		}

		/* FONCTION de saut avec chargement avant lacher, qui n'est pas compatible avec l'ensemble, mais pourrait etre implémentée
		// JUMP 
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
			jump_power += 0.1f;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE){
			if (jump_power > 0.0f){
				jump_power -= 0.1f;
				z += 0.1f;
				jump_fall += 0.1f;
			}
			else if (jump_fall > sol){
				jump_fall -= 0.1f;
				z -= 0.1f;
			}
		}
		// Strafe DOWN
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
			z = -0.1f;
		}
		*/


		//BOOST
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
			if (NOS >= 0.0f){
				x = pas_max*2.5f;
				NOS -= 1;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE){
			if (NOS >= 100.0f){
				NOS = 100;
			}
			else { NOS += 1; }
		}

		obstacleEnCollisionSphere = NULL;
		if (collision2(voiture, myMatrix, obstacles, &obstacleEnCollisionSphere)){//On regarde si on a une collision dans une boundary sphere
			//Si oui, alors on fait une collision plus fine qui met en oeuvre des intersections de rectangles
			if (collisionFine(g_vertex_buffer_data_HitboxVoiture, myMatrix, vertexsObstacles[obstacleEnCollisionSphere]) || collision){
				//la collision est avérée, on repart dans la direction opposée jusqu'a ce qu'on ressorte de la boundary sphere
				myMatrix = glm::translate(myMatrix, -DernierVectVitesseNonNul);
				collision = true;
				VectCamera = -DernierVectVitesseNonNul;

			}
			else{//On est proche de l'obstacle sans etre en collision, on fait comme si de rien n'etait
				myMatrix = glm::rotate(myMatrix, r_voiture_gd*pas / pas_max, glm::vec3(0.0f, 1.0f, 0.0f));
				myMatrix = glm::translate(myMatrix, VectVitesse);
				rot_Camera += r_voiture_gd*pas / pas_max;
				VectVitesse = glm::vec3(x, z, y);
				VectCamera = VectVitesse;
				if (x != 0.0f || y != 0.0f || z != 0.0f){
					DernierVectVitesseNonNul = glm::vec3(x, z, y);
				}
			}
		}
		else{
			//On est dans aucne boundary sphere, donc on verifie juste si on ne sort pas d'une collision
			// Si il y'ya eu une collision et qu'on vient d'en sortir, alors l'utilisateur perd le controle pour 0.5s, rotation et vitesse sont remis a 0
			if (collision){
				Sleep(500);
				collision = false;
				VectVitesse = glm::vec3(0.0f, 0.0f, 0.0f);
				r_voiture_gd = 0.0f;
				pas = pas_init;
			}
			else{
				myMatrix = glm::rotate(myMatrix, r_voiture_gd*pas / pas_max, glm::vec3(0.0f, 1.0f, 0.0f));
				myMatrix = glm::translate(myMatrix, VectVitesse);
				rot_Camera += r_voiture_gd*pas / pas_max;
				VectVitesse = glm::vec3(x, z, y);
				if (x != 0.0f || y != 0.0f || z != 0.0f){
					DernierVectVitesseNonNul = glm::vec3(x, z, y);
				}
			}
				
				VectCamera = VectVitesse;
		}
		
		// SOL

		glm::mat4 FloorScalingMatrix = glm::scale(10000.0f, 0.05f, 10000.0f);
		glm::mat4 FloorTranslationMatrix = glm::translate(0.0f, -1.0f, 0.0f);

		// Set positions elements de la voiture (carosserie, roues...)
		//Resize (division par 4) pour les objets importés de blender
		glm::mat4 scallingCube2 = glm::scale(Model, glm::vec3(0.2f, 0.2f, 0.33f));
		glm::mat4 scallingVoiture = glm::scale(Model, glm::vec3(0.25f, 0.25f, 0.25f));
		glm::mat4 translateVoiture = glm::translate(Model, glm::vec3(0.0f, -0.9f, 0.0f));
		glm::mat4 decaleRoueAvDroite = glm::translate(Model, glm::vec3(1.35f, 0.0f, 0.6f));
		glm::mat4 decaleRoueAvGauche = glm::translate(Model, glm::vec3(1.35f, 0.0f, -0.6f));
		glm::mat4 decaleRoueArDroite = glm::translate(Model, glm::vec3(-1.3f, 0.0f, 0.6f));
		glm::mat4 decaleRoueArGauche = glm::translate(Model, glm::vec3(-1.3f, 0.0f, -0.6f));
		glm::vec3 myRotationAxis(0.0f, 0.0f, 5.0f);
		glm::mat4 myRotation = glm::rotate(Model, r_avant_arriere, myRotationAxis);

		// Rotation latérale des roues avant
		glm::vec3 rlatRouesAvant(0.0f, 1.0f, 0.0f);
		glm::mat4 rotationLRAV = glm::rotate(Model, r_roues_gd, rlatRouesAvant);


		// Modification de la View pour que la caméra suive la voiture

		VectCamera = glm::rotate(VectCamera, rot_Camera, glm::vec3(0.0f, 1.0f, 0.0f));
		
		//CameraPos = glm::vec3(CameraPos.x + VectCamera.x, CameraPos.y + VectCamera.y, CameraPos.z + VectCamera.z);

		CarOrigin = glm::vec3(CarOrigin.x + VectCamera.x, CarOrigin.y + VectCamera.y, CarOrigin.z + VectCamera.z);


		View = glm::lookAt(
			CameraPos, 
			CarOrigin, 
			glm::vec3(0, 1, 0)
			);
		

		//Matrices MVP

		//La MVP représente une rotation et translation du Modele de la frame en cours. L'ensemble des de la voiture MVP est basé sur ce premier MVP, ainsi tous les éléments sont liés
		MVP = Projection * View * myMatrix;
		MVP_Voiture = MVP * translateVoiture * scallingVoiture;
		MVP_RoueAvDroite = MVP*decaleRoueAvDroite*rotationLRAV*myRotation*scallingCube2;
		MVP_RoueAvGauche = MVP*decaleRoueAvGauche*rotationLRAV*myRotation*scallingCube2;
		MVP_RoueArGauche = MVP*decaleRoueArGauche*myRotation*scallingCube2;
		MVP_RoueArDroite = MVP*decaleRoueArDroite*myRotation*scallingCube2;
		MVP_Floor = Projection*View*Model*FloorTranslationMatrix*FloorScalingMatrix;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);


		//AFFICHAGE DES DIFFERENTS VERTICES EN UTILISANT LES MVP

		//-----------------------------------------------------------------------VOITURE---------------------------------------------------------------------------------------

	
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_Voiture[0][0]);

		// 2nd attribute buffer ROUE_AV_DROIT
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, blender);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorblender);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_voiture_color), g_voiture_color, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, sizeVoiture); 

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);



		//-----------------------------------------------------------------------VITRES_VOITURE---------------------------------------------------------------------------------------


		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_Voiture[0][0]);

		// 2nd attribute buffer ROUE_AV_DROIT
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vitresBlender);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorVitresblender);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vitres_color), g_vitres_color, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, sizeVitres);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		//-----------------------------------------------------------------------ROUE_AV_DROIT---------------------------------------------------------------------------------------

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_RoueAvDroite[0][0]);

		// 2nd attribute buffer ROUE_AV_DROIT
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, roueBlender);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorRoueBlender);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_roue_color),g_roue_color, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, sizeRoue); 

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		//-----------------------------------------------------------------------ROUE_AV_GAUCHE---------------------------------------------------------------------------------------


		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_RoueAvGauche[0][0]);

		// 2nd attribute buffer CUBE2
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, roueBlender);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorRoueBlender);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_roue_color), g_roue_color, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, sizeRoue); 

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		//-----------------------------------------------------------------------ROUE_AR_GAUCHE---------------------------------------------------------------------------------------


		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_RoueArGauche[0][0]);

		// 2nd attribute buffer ARGAUCHE
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, roueBlender);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorRoueBlender);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_roue_color), g_roue_color, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, sizeRoue); 

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		//-----------------------------------------------------------------------ROUE_AR_DROITE---------------------------------------------------------------------------------------


		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_RoueArDroite[0][0]);

		// 2nd attribute buffer ARGAUCHE
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, roueBlender);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorRoueBlender);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_roue_color), g_roue_color, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, sizeRoue); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		//-----------------------------------------------------------------------OBSCTACLE (TROU)-----------------------------------------------------------------------------

		
		// Send our transformation to the currently bound shader, 
		MVP = Projection * View * Model;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferObstacleTrou);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbufferObstacleTrou);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_ObstacleTrou), g_color_buffer_data_ObstacleTrou, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 2 * 3); // 2*3 indices starting at 0 -> 2 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);



		//-----------------------------------------------------------------------OBSCTACLE (BARRIERE 1)-----------------------------------------------------------------------------


		// Send our transformation to the currently bound shader, 
		MVP = Projection * View * Model*scallingVoiture;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBar1);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBarriere);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_barriere_color), g_barriere_color, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, sizeBarriere); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		//-----------------------------------------------------------------------OBSCTACLE (BARRIERE 2)-----------------------------------------------------------------------------
		

		// Send our transformation to the currently bound shader, 
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBar2);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBarriere);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_barriere_color), g_barriere_color, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, sizeBarriere); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		//-----------------------------------------------------------------------OBSCTACLE (BARRIERE 3)-----------------------------------------------------------------------------


		// Send our transformation to the currently bound shader, 
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBar3);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBarriere);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_barriere_color), g_barriere_color, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, sizeBarriere); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);



		//-----------------------------------------------------------------------OBSCTACLE (BARRIERE 4)-----------------------------------------------------------------------------


		// Send our transformation to the currently bound shader, 
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBar4);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBarriere);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_barriere_color), g_barriere_color, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, sizeBarriere); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		//-----------------------------------------------------------------------OBSCTACLE (BARRIERE 5)-----------------------------------------------------------------------------


		// Send our transformation to the currently bound shader, 
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBar5);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBarriere);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_barriere_color), g_barriere_color, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, sizeBarriere); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);



		//-----------------------------------------------------------------------Le Sol -------------------------------------------------------------------------------
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_Floor[0][0]);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferfloor);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbufferfloor);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);



		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		

		

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &blender);
	glDeleteBuffers(1, &colorblender);
	glDeleteBuffers(1, &roueBlender);
	glDeleteBuffers(1, &colorRoueBlender);
	glDeleteBuffers(1, &vertexbufferfloor);
	glDeleteBuffers(1, &colorbufferfloor);

	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}