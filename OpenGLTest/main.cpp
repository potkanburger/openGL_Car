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
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100000.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(15, 20, -6), // Camera is at (4,3,-3), in World Space
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


	vector<obs> obstacles;

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat g_vertex_buffer_data_Cube1[] = {
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


	static const GLfloat g_vertex_buffer_data_Roue[] = {
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

	static const GLfloat g_vertex_buffer_data_HitboxVoiture[] = {
		-1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		-1.0f, 0.0f, -1.0f,
		1.0f, 0.0f, -1.0f,
		-1.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 1.0f,
	};


	static const GLfloat g_vertex_buffer_data_ObstacleTrou[] = {
		2.0f, 0.0f, 1.0f,
		4.0f, 0.0f, 1.0f,
		2.0f, 0.0f, -1.0f,
		4.0f, 0.0f, -1.0f,
		2.0f, 0.0f, -1.0f,
		4.0f, 0.0f, 1.0f,
	};

	// One color for each vertex. They were generated randomly.
	static GLfloat g_color_buffer_data_Cube1[12 * 3 * 3];
	for (int v = 0; v<12 * 3; v++){
		g_color_buffer_data_Cube1[3 * v + 0] = v / 36.0f;
		g_color_buffer_data_Cube1[3 * v + 1] = 0.32f;
		g_color_buffer_data_Cube1[3 * v + 2] = 0.55f;
	}

	static GLfloat g_color_buffer_data_Roue[12 * 3 * 3];
	for (int v = 0; v<12 * 3; v++){
		g_color_buffer_data_Roue[3 * v + 0] = 0.80f;
		g_color_buffer_data_Roue[3 * v + 1] = v / 72.0f + 0.10f;
		g_color_buffer_data_Roue[3 * v + 2] = 0.05f;

	}

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


	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_Cube1), g_vertex_buffer_data_Cube1, GL_STATIC_DRAW);


	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_Cube1), g_color_buffer_data_Cube1, GL_STATIC_DRAW);

	GLuint vertexbufferRoue;
	glGenBuffers(1, &vertexbufferRoue);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferRoue);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_Roue), g_vertex_buffer_data_Roue, GL_STATIC_DRAW);


	GLuint colorbufferRoue;
	glGenBuffers(1, &colorbufferRoue);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferRoue);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_Roue), g_color_buffer_data_Roue, GL_STATIC_DRAW);


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


	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

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


										// --------------------------------------------LE SOL AKILEBO-------------------------------------------------------------

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
											0.583f, 0.771f, 0.014f,
											0.609f, 0.115f, 0.436f,
											0.327f, 0.483f, 0.844f,
											0.822f, 0.569f, 0.201f,
											0.435f, 0.602f, 0.223f,
											0.310f, 0.747f, 0.185f,
											0.597f, 0.770f, 0.761f,
											0.559f, 0.436f, 0.730f,
											0.359f, 0.583f, 0.152f,
											0.483f, 0.596f, 0.789f,
											0.559f, 0.861f, 0.639f,
											0.195f, 0.548f, 0.859f,
											0.014f, 0.184f, 0.576f,
											0.771f, 0.328f, 0.970f,
											0.406f, 0.615f, 0.116f,
											0.676f, 0.977f, 0.133f,
											0.971f, 0.572f, 0.833f,
											0.140f, 0.616f, 0.489f,
											0.997f, 0.513f, 0.064f,
											0.945f, 0.719f, 0.592f,
											0.543f, 0.021f, 0.978f,
											0.279f, 0.317f, 0.505f,
											0.167f, 0.620f, 0.077f,
											0.347f, 0.857f, 0.137f,
											0.055f, 0.953f, 0.042f,
											0.714f, 0.505f, 0.345f,
											0.783f, 0.290f, 0.734f,
											0.722f, 0.645f, 0.174f,
											0.302f, 0.455f, 0.848f,
											0.225f, 0.587f, 0.040f,
											0.517f, 0.713f, 0.338f,
											0.053f, 0.959f, 0.120f,
											0.393f, 0.621f, 0.362f,
											0.673f, 0.211f, 0.457f,
											0.820f, 0.883f, 0.371f,
											0.982f, 0.099f, 0.879f,
										};
										GLuint vertexbufferfloor;
										glGenBuffers(1, &vertexbufferfloor);
										glBindBuffer(GL_ARRAY_BUFFER, vertexbufferfloor);
										glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_floor), g_vertex_buffer_data_floor, GL_STATIC_DRAW);
										

										GLuint colorbufferfloor;
										glGenBuffers(1, &colorbufferfloor);
										glBindBuffer(GL_ARRAY_BUFFER, colorbufferfloor);
										glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_floor), g_color_buffer_data_floor, GL_STATIC_DRAW);

















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

		// Strafe UP
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
			z += 0.1f;
		}
		// Strafe DOWN
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
			z = -0.1f;
		}



		// Set positions elements de la voiture (carosserie, roues...)
		
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
		
		glm::mat4 scallingCube2 = glm::scale(Model, glm::vec3(0.2f, 0.2f, 0.2f));
		glm::mat4 FloorScalingMatrix = glm::scale(10000.0f, 0.05f, 10000.0f);
		glm::mat4 FloorTranslationMatrix = glm::translate(0.0f, -2.0f, 0.0f);
		glm::mat4 decaleRoueAvDroite = glm::translate(Model, glm::vec3(1.0f, -1.0f, 1.0f));
		glm::mat4 decaleRoueAvGauche = glm::translate(Model, glm::vec3(1.0f, -1.0f, -1.0f));
		glm::mat4 decaleRoueArDroite = glm::translate(Model, glm::vec3(-1.0f, -1.0f, 1.0f));
		glm::mat4 decaleRoueArGauche = glm::translate(Model, glm::vec3(-1.0f, -1.0f, -1.0f));
		glm::vec3 myRotationAxis(0.0f, 0.0f, 5.0f);
		glm::mat4 myRotation = glm::rotate(Model, r_avant_arriere, myRotationAxis);

		// Rotation latérale des roues avant
		glm::vec3 rlatRouesAvant(0.0f, 1.0f, 0.0f);
		glm::mat4 rotationLRAV = glm::rotate(Model, r_roues_gd, rlatRouesAvant);


		//Matrices MVP
		MVP = Projection * View * myMatrix;
		MVP_RoueAvDroite = MVP*decaleRoueAvDroite*rotationLRAV*myRotation*scallingCube2;
		MVP_RoueAvGauche = MVP*decaleRoueAvGauche*rotationLRAV*myRotation*scallingCube2;
		MVP_RoueArGauche = MVP*decaleRoueArGauche*myRotation*scallingCube2;
		MVP_RoueArDroite = MVP*decaleRoueArDroite*myRotation*scallingCube2;
		
		glm::mat4 MVP_Floor = Projection * View * Model*FloorTranslationMatrix*FloorScalingMatrix;




		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		
		//-----------------------------------------------------------------------CUBE_1---------------------------------------------------------------------------------------


		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
		if (collision2(voiture, myMatrix, obs1)){
			if (collisionFine(g_vertex_buffer_data_HitboxVoiture, myMatrix, g_vertex_buffer_data_ObstacleTrou)){
				for (int v = 0; v<3 * 3; v++){
					g_color_buffer_data_Cube1[12 * v + 0] = 0.1f;
					g_color_buffer_data_Cube1[12 * v + 1] = 0.1f;
					g_color_buffer_data_Cube1[12 * v + 2] = 0.1f;
					g_color_buffer_data_Cube1[12 * v + 3] = 0.1f;
					g_color_buffer_data_Cube1[12 * v + 4] = 0.1f;
					g_color_buffer_data_Cube1[12 * v + 5] = 0.1f;
					g_color_buffer_data_Cube1[12 * v + 6] = 0.1f;
					g_color_buffer_data_Cube1[12 * v + 7] = 0.1f;
					g_color_buffer_data_Cube1[12 * v + 8] = 0.1f;
					g_color_buffer_data_Cube1[12 * v + 9] = 0.1f;
					g_color_buffer_data_Cube1[12 * v + 10] = 0.1f;
					g_color_buffer_data_Cube1[12 * v + 11] = 0.1f;
				}
			}
			else{
				for (int v = 0; v<3 * 3; v++){
					g_color_buffer_data_Cube1[12 * v + 0] = 0.0f;
					g_color_buffer_data_Cube1[12 * v + 1] = 1.0f;
					g_color_buffer_data_Cube1[12 * v + 2] = 0.0f;
					g_color_buffer_data_Cube1[12 * v + 3] = 0.0f;
					g_color_buffer_data_Cube1[12 * v + 4] = 1.0f;
					g_color_buffer_data_Cube1[12 * v + 5] = 0.0f;
					g_color_buffer_data_Cube1[12 * v + 6] = x;
					g_color_buffer_data_Cube1[12 * v + 7] = 0.f;
					g_color_buffer_data_Cube1[12 * v + 8] = 0.0f;
					g_color_buffer_data_Cube1[12 * v + 9] = 0.0f;
					g_color_buffer_data_Cube1[12 * v + 10] = 0.0f;
					g_color_buffer_data_Cube1[12 * v + 11] = 1 - x;
				}
			}
		}
		else{
			for (int v = 0; v<3 * 3; v++){
				g_color_buffer_data_Cube1[12 * v + 0] = 0.0f;
				g_color_buffer_data_Cube1[12 * v + 1] = 1.0f;
				g_color_buffer_data_Cube1[12 * v + 2] = 0.0f;
				g_color_buffer_data_Cube1[12 * v + 3] = 0.0f;
				g_color_buffer_data_Cube1[12 * v + 4] = 1.0f;
				g_color_buffer_data_Cube1[12 * v + 5] = 0.0f;
				g_color_buffer_data_Cube1[12 * v + 6] = x;
				g_color_buffer_data_Cube1[12 * v + 7] = 0.f;
				g_color_buffer_data_Cube1[12 * v + 8] = 0.0f;
				g_color_buffer_data_Cube1[12 * v + 9] = 0.0f;
				g_color_buffer_data_Cube1[12 * v + 10] = 0.0f;
				g_color_buffer_data_Cube1[12 * v + 11] = 1 - x;
			}
		}
		// 2nd attribute buffer : colors

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_Cube1), g_color_buffer_data_Cube1, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		//-----------------------------------------------------------------------ROUE_AV_DROIT---------------------------------------------------------------------------------------


		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_RoueAvDroite[0][0]);

		// 2nd attribute buffer ROUE_AV_DROIT
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferRoue);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbufferRoue);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_Roue), g_color_buffer_data_Roue, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		//-----------------------------------------------------------------------CUBE_ROUE_AV_GAUCHE---------------------------------------------------------------------------------------


		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_RoueAvGauche[0][0]);

		// 2nd attribute buffer CUBE2
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferRoue);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbufferRoue);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_Roue), g_color_buffer_data_Roue, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		//-----------------------------------------------------------------------CUBE_ROUE_AR_GAUCHE---------------------------------------------------------------------------------------


		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_RoueArGauche[0][0]);

		// 2nd attribute buffer ARGAUCHE
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferRoue);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbufferRoue);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_Roue), g_color_buffer_data_Roue, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		//-----------------------------------------------------------------------CUBE_ROUE_AR_DROITE---------------------------------------------------------------------------------------


		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP_RoueArDroite[0][0]);

		// 2nd attribute buffer ARGAUCHE
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferRoue);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbufferRoue);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_Roue), g_color_buffer_data_Roue, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		//-----------------------------------------------------------------------HITBOX---------------------------------------------------------------------------------------


		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferHitboxVoiture);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);


		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbufferHitboxVoiture);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data_HitboxVoiture), g_color_buffer_data_HitboxVoiture, GL_STATIC_DRAW);
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

		//-----------------------------------------------------------------------Le Sol Moche-------------------------------------------------------------------------------
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

		//______________________________________________________________________________________________

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteBuffers(1, &vertexbufferRoue);
	glDeleteBuffers(1, &colorbufferRoue);

	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}