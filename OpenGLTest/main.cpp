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
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

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

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(8, 6, -6), // Camera is at (4,3,-3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
	glm::mat4 MVP_RoueAvGauche = Projection * View * Model;
	glm::mat4 MVP_RoueAvDroite = Projection * View * Model;
	glm::mat4 MVP_RoueArDroite = Projection * View * Model;
	glm::mat4 MVP_RoueArGauche = Projection * View * Model;
	glm::mat4 MVP_Voiture = Projection * View * Model;
	glm::mat4 MVP_Floor = Projection * View * Model;

	vector<obs> obstacles;
	

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



	std::vector< glm::vec3 > verticesVoiture;
	verticesVoiture = objLoader("voiture.obj");
	int sizeVoiture = verticesVoiture.size();
	static GLfloat g_voiture_data[10000];
	static GLfloat g_voiture_color[10000];
	float max_x = 0.0f;
	float max_z = 0.0f;

	for (int v = 0; v < sizeVoiture; v++){

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

	static const GLfloat g_vertex_buffer_data_HitboxVoiture[] = {
		-max_x, 0.0f, max_z,
		max_x, 0.0f, max_z,
		-max_x, 0.0f, -max_z,
		max_x, 0.0f, -max_z,
		-max_x, 0.0f, -max_z,
		max_x, 0.0f, max_z
	};


	static const GLfloat g_vertex_buffer_data_ObstacleTrou[] = {
		2.0f, 0.0f, 1.0f,
		4.0f, 0.0f, 1.0f,
		2.0f, 0.0f, -1.0f,
		4.0f, 0.0f, -1.0f,
		2.0f, 0.0f, -1.0f,
		4.0f, 0.0f, 1.0f
	};


	GLuint blender;
	glGenBuffers(1, &blender);
	glBindBuffer(GL_ARRAY_BUFFER, blender);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_voiture_data), g_voiture_data, GL_STATIC_DRAW);

	GLuint colorblender;
	glGenBuffers(1, &colorblender);
	glBindBuffer(GL_ARRAY_BUFFER, colorblender);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_voiture_color), g_voiture_color, GL_STATIC_DRAW);


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

	float pas = 0.1f;
	float pas_init = 0.1f;
	float pas_max = 0.5f;
	float pas_lat = 0.1f;
	float r_avant_arriere = 0.0f;
	float r_roues_gd = 0.0f; //rotation roues gauche-droite
	float r_voiture_gd = 0.0f; //rotation voiture gauche-droite
	bool collision = false;

	glm::mat4 myMatrix = Model;
	glm::vec3 VectVitesse = glm::vec3(x, z, y);
	glm::vec3 DernierVectVitesseNonNul = glm::vec3(x, z, y);

	obs voiture = { voiture.rayon = 0.0f, voiture.centre = glm::vec4(0.0f) };
	obs obs1 = {obs1.rayon = 0.0f, obs1.centre = glm::vec4(0.0f)};
	get_centre_rayon(g_vertex_buffer_data_ObstacleTrou, &obs1.rayon, &obs1.centre);
	get_centre_rayon(g_vertex_buffer_data_HitboxVoiture, &voiture.rayon, &voiture.centre);
	obstacles.push_back(obs1);

	do{

		x = 0.0f;
		y = 0.0f;
		z = 0.0f;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE){
			pas = pas_init;
		}

		// Move forward
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
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

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
			r_voiture_gd = 0.0f;
		}

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

		
		if (collision2(voiture, myMatrix, obs1)){
			if (collisionFine(g_vertex_buffer_data_HitboxVoiture, myMatrix, g_vertex_buffer_data_ObstacleTrou) || collision){
				myMatrix = glm::translate(myMatrix, -DernierVectVitesseNonNul);
				collision = true;
			}
			else{
				myMatrix = glm::translate(myMatrix, VectVitesse);
				myMatrix = glm::rotate(myMatrix, r_voiture_gd*pas / pas_max, glm::vec3(0.0f, 1.0f, 0.0f));
				VectVitesse = glm::vec3(x, z, y);
				if (x != 0.0f || y != 0.0f || z != 0.0f){
					DernierVectVitesseNonNul = glm::vec3(x, z, y);
				}
			}
		}
		else{
			if (collision){
				Sleep(500);
				collision = false;
				VectVitesse = glm::vec3(0.0f, 0.0f, 0.0f);
				r_voiture_gd = 0.0f;
			}
			else{
				myMatrix = glm::translate(myMatrix, VectVitesse);
				myMatrix = glm::rotate(myMatrix, r_voiture_gd*pas / pas_max, glm::vec3(0.0f, 1.0f, 0.0f));
				VectVitesse = glm::vec3(x, z, y);
				if (x != 0.0f || y != 0.0f || z != 0.0f){
					DernierVectVitesseNonNul = glm::vec3(x, z, y);
				}
			}
		}
		
		// SOL

		glm::mat4 FloorScalingMatrix = glm::scale(10000.0f, 0.05f, 10000.0f);
		glm::mat4 FloorTranslationMatrix = glm::translate(0.0f, -1.0f, 0.0f);

		// Set positions elements de la voiture (carosserie, roues...)
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


		//Matrices MVP
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