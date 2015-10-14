#include <car_functions.h>

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()){
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()){
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}



	GLint Result = GL_FALSE;
	int InfoLogLength;



	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

bool collision(const GLfloat objet[], glm::mat4 MVP_obj, const GLfloat obstacle[]){
	float bound_obs_min_x = obstacle[0];
	float bound_obs_min_y = obstacle[1];
	float bound_obs_min_z = obstacle[2];

	float bound_obs_max_x = obstacle[0];
	float bound_obs_max_y = obstacle[1];
	float bound_obs_max_z = obstacle[2];

	int inter[3] = { 0, 0, 0 };

	for (int i = 1; i < 6; i++){
		bound_obs_min_x = glm::min(bound_obs_min_x, obstacle[3 * i + 0]);
		bound_obs_min_y = glm::min(bound_obs_min_y, obstacle[3 * i + 1]);
		bound_obs_min_z = glm::min(bound_obs_min_z, obstacle[3 * i + 2]);

		if (obstacle[3 * i + 0]>bound_obs_max_x){
			bound_obs_max_x = obstacle[3 * i + 0];
		}
		bound_obs_max_y = glm::max(bound_obs_max_y, obstacle[3 * i + 1]);
		bound_obs_max_z = glm::max(bound_obs_max_z, obstacle[3 * i + 2]);
	}

	glm::vec4 tmp;

	for (int i = 0; i < 6; i++){
		tmp = MVP_obj*glm::vec4(objet[3 * i + 0], objet[3 * i + 1], objet[3 * i + 2], 1.0f);

		if (tmp.x >= bound_obs_min_x && tmp.x <= bound_obs_max_x){
			inter[0] = 1;
		}
		if (tmp.y >= bound_obs_min_y && tmp.y <= bound_obs_max_y){
			inter[1] = 1;
		}
		if (tmp.z >= bound_obs_min_z && tmp.z <= bound_obs_max_z){
			inter[2] = 1;
		}

	}

	return (inter[0] + inter[1] + inter[2]) == 3;

}