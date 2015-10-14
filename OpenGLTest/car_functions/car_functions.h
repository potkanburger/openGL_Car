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


#define PI 3.14159265F;

bool collision(const GLfloat objet[], glm::mat4 MVP_obj, const GLfloat obstacle[]);
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);