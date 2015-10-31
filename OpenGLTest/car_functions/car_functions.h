// Include standard headers
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <sstream>
using namespace std;

#include <stdlib.h>
#include <string.h>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

struct obs{
	float rayon;
	glm::vec4 centre;
};

struct couplePoints{
	glm::vec3 pointA;
	glm::vec3 pointB;
};

struct triangle{
	int pt1;
	int pt2;
	int pt3;
};


#define PI 3.14159265F;


bool collision(const GLfloat objet[], glm::mat4 MVP_obj, const GLfloat obstacle[]);
bool collision2(obs voiture, glm::mat4 MVP_obj, obs obstacle);
bool collisionFine(const GLfloat voitureOriginale[], glm::mat4 MVP_obj, const GLfloat obstacle[]);
void getTwoSidesOfRectangle(const GLfloat rectangle[], couplePoints* coupleLargeur, couplePoints* coupleLongueur);
float distanceCarree(glm::vec3 ptA, glm::vec3 ptB);
void get_centre_rayon(const GLfloat obstacle[], float* rayon, glm::vec4* centre);
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

std::vector<glm::vec3> objLoader(string fichier);