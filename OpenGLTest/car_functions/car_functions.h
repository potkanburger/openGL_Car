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


#define PI 3.14159265F;

bool collision(const GLfloat objet[], glm::mat4 MVP_obj, const GLfloat obstacle[]);
bool collision2(obs voiture, glm::mat4 MVP_obj, obs obstacle);

void get_centre_rayon(const GLfloat obstacle[], float* rayon, glm::vec4* centre);
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);