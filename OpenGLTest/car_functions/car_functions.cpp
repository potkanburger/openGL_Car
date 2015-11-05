#include <car_functions.h>


// Fonction LoadShaders du tutoriel opengl-tutorial.org
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


/*
Une premiere fonction de collision, na�ve et tr�s peu pr�cise non utilis�e dans le programme
Cette fonction d�termine des rectangles 3d qui contiennent respectivement l'objet et l'obstacle puis compare dans le meme repere les valeurs en x, y, pour chercher au moins une intersection par axe
*/
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

		bound_obs_max_x = glm::max(bound_obs_max_x, obstacle[3 * i + 0]);
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


/*
	Collisions par boundaries spheres.
	Si la distance (carr�e) entre voiture et obstacle est inf�rieure a la somme des carr�s des rayons des spheres, alors l'intersection des 2 spheres n'est pas nulle.
	Il y a donc un risque de collision effective.
*/
bool collision2(obs voiture, glm::mat4 MVP_obj, vector<obs> obstacles, int* collision){// d�tection de collisions de sph�res 
	int counter = 0;
	for (obs obstacle : obstacles){
		glm::vec4 tmp;
		tmp = MVP_obj*voiture.centre;
		float distance = pow(tmp.x - obstacle.centre.x, 2) + pow(tmp.y - obstacle.centre.y, 2) + pow(tmp.z - obstacle.centre.z, 2);
		if (distance <= pow(voiture.rayon + obstacle.rayon, 2)){
			*collision = counter;
			return true;
		}
		counter++;
	}
	
	return false;
}

/*
	obtention et stockage du centre et du rayon (max) d'un objet.
*/
void get_centre_rayon(const GLfloat obstacle[], float* rayon, glm::vec4 *centre){
	float bound_obs_min_x = obstacle[0];
	float bound_obs_min_y = obstacle[1];
	float bound_obs_min_z = obstacle[2];

	float bound_obs_max_x = obstacle[0];
	float bound_obs_max_y = obstacle[1];
	float bound_obs_max_z = obstacle[2];
	float rayon_obstacle = 0.0f;

	int inter[3] = { 0, 0, 0 };

	for (int i = 1; i < 6; i++){
		float tmp_x = obstacle[3 * i + 0];
		float tmp_y = obstacle[3 * i + 1];
		float tmp_z = obstacle[3 * i + 2];

		bound_obs_min_x = std::min(bound_obs_min_x, tmp_x);
		bound_obs_min_y = std::min(bound_obs_min_y, tmp_y);
		bound_obs_min_z = std::min(bound_obs_min_z, tmp_z);

		bound_obs_max_x = std::max(bound_obs_max_x, tmp_x);
		bound_obs_max_y = std::max(bound_obs_max_y, tmp_y);
		bound_obs_max_z = std::max(bound_obs_max_z, tmp_z);
	}

	rayon_obstacle = std::max(abs(bound_obs_max_x - bound_obs_min_x), abs(bound_obs_max_y - bound_obs_min_y));
	*rayon = std::max(rayon_obstacle, abs(bound_obs_max_z - bound_obs_min_z));

	*centre = glm::vec4(bound_obs_min_x + ((bound_obs_max_x - bound_obs_min_x) / 2), bound_obs_min_y + ((bound_obs_max_y - bound_obs_min_y) / 2), bound_obs_min_z + ((bound_obs_max_z - bound_obs_min_z) / 2), 1.0f);
}

/*
Fonction qui nous permet de recup�rer les 2 cot�s n'etant pas l'hypothenuse du triangle rectangle form� par les 3 points en entr�e.
Ainsi, on obtient les couples de points formant la largeur et ceux formant la longueur du rectangle.

*/
void getTwoSidesOfRectangle(const GLfloat rectangle[], couplePoints* coupleLargeur, couplePoints* coupleLongueur){
	glm::vec3 pt1 = glm::vec3(rectangle[0], rectangle[1], rectangle[2]);
	glm::vec3 pt2 = glm::vec3(rectangle[3], rectangle[4], rectangle[5]);
	glm::vec3 pt3 = glm::vec3(rectangle[6], rectangle[7], rectangle[8]);

	couplePoints cp1 = { cp1.pointA = pt1, cp1.pointB = pt2};
	couplePoints cp2 = { cp2.pointA = pt2, cp2.pointB = pt3 };
	couplePoints cp3 = { cp3.pointA = pt1, cp3.pointB = pt3 };
	couplePoints tabCouples[3] = { cp1, cp2, cp3 };

	int largeur, nL, longueur;
	float d_tmp;
	float min = NULL;
	float max = NULL;
	for (int i = 0; i < 3; i++){
		d_tmp = distanceCarree(tabCouples[i].pointA, tabCouples[i].pointB);
		if (i==0){
			max = d_tmp;
			min = d_tmp;
			largeur = i;
			nL = i;
		}
		else{
			if (d_tmp >= max){
				max = d_tmp;
				nL = i;
			}
			else if (d_tmp <= min){
				min = d_tmp;
				largeur = i;
			}
		}
			
	}

	for (int i = 0; i < 3; i++){
		if (largeur != i && nL != i){
			longueur = i;
		}
	}

	*coupleLargeur = tabCouples[largeur];
	*coupleLongueur = tabCouples[longueur];
}


/*
	Renvoie le carr� de la distance entre 2 points
*/
float distanceCarree(glm::vec3 ptA, glm::vec3 ptB){
	return pow(ptA.x - ptB.x, 2) + pow(ptA.y - ptB.y, 2) + pow(ptA.z - ptB.z, 2);
}


/*
	Fonction de collision fine entre un objet (mobile) et un obstacle fixe
	On obtient la longueur et la largeur des 2 rectangles (hitbox des objets)
	Pour chaque rectangle:
		on prend un des c�t�, on projete l'autre rectangle sur l'axe parallele a ce cot�. On regarde si il y a intersection entre le projet� du cot� sur cet axe et les minima et maxima du projet� de l'autre rectangle sur cet axe.
		on fait la meme op�ration pour le 2eme cot�.
	fin pour;
	Si on a 4 intersections, alors il y a collision.
	Il suffit d'un cot� d'un rectangle sans intersection pour qu'il n'y ait pas collision.
*/
bool collisionFine(const GLfloat voitureOriginale[], glm::mat4 MVP_obj, GLfloat obstacle[]){
	couplePoints largeurA, longueurA;
	couplePoints largeurB, longueurB;
	float pente,alpha_angle;
	GLfloat voiture[6*3];
	glm::vec4 tmp;
	for (int i = 0; i < 6; i++){
		tmp = MVP_obj*glm::vec4(voitureOriginale[3 * i + 0], voitureOriginale[3 * i + 1], voitureOriginale[3 * i + 2], 1.0f);
		voiture[3 * i + 0] = tmp.x;
		voiture[3 * i + 1] = tmp.y;
		voiture[3 * i + 2] = tmp.z;
	}
	getTwoSidesOfRectangle(voiture, &largeurA, &longueurA);
	getTwoSidesOfRectangle(obstacle, &largeurB, &longueurB);

	couplePoints longueursTests[4] = { largeurA, longueurA, largeurB, longueurB };
	float min_xLocalA, max_xLocalA;
	float min_xLocalB, max_xLocalB;
	float tmp_LocalA, tmp_LocalB;
	const GLfloat *objA;
	const GLfloat *objB;


	for (int i = 0; i < 4; i++){
		// Si les 2 rectangles sont dans le meme repere, alors la projection n'est qu'un bon approch� produit des imperfections. On g�re le cas � part, ce qui en plus nous economise des calculs.
		if ((longueursTests[i].pointB.x - longueursTests[i].pointA.x) == 0.0f){
			if (i < 2){
				objA = obstacle;
				objB = voiture;
			}
			else{
				objA = voiture;
				objB = obstacle;
			}

			for (int j = 0; j < 6; j++){
				tmp_LocalA = objA[3 * j + 2];
				tmp_LocalB = objB[3 * j + 2];
				if (j == 0){
					max_xLocalA = tmp_LocalA;
					min_xLocalA = tmp_LocalA;
					max_xLocalB = tmp_LocalB;
					min_xLocalB = tmp_LocalB;
				}
				else{
					if (tmp_LocalA > max_xLocalA){
						max_xLocalA = tmp_LocalA;
					}
					if (tmp_LocalA < min_xLocalA){
						min_xLocalA = tmp_LocalA;
					}

					if (tmp_LocalB > max_xLocalB){
						max_xLocalB = tmp_LocalB;
					}
					if (tmp_LocalB < min_xLocalB){
						min_xLocalB = tmp_LocalB;
					}
				}
			}


		}
		else{
			pente = (longueursTests[i].pointB.z - longueursTests[i].pointA.z) / (longueursTests[i].pointB.x - longueursTests[i].pointA.x);
			objA = obstacle;
			objB = voiture;
			if (pente == 0.0f){
				for (int j = 0; j < 6; j++){
					tmp_LocalA = objA[3 * j + 0];
					tmp_LocalB = objB[3 * j + 0];

					if (j == 0){
						max_xLocalA = tmp_LocalA;
						min_xLocalA = tmp_LocalA;
						max_xLocalB = tmp_LocalB;
						min_xLocalB = tmp_LocalB;
					}
					else{
						if (tmp_LocalA > max_xLocalA){
							max_xLocalA = tmp_LocalA;
						}
						if (tmp_LocalA < min_xLocalA){
							min_xLocalA = tmp_LocalA;
						}

						if (tmp_LocalB > max_xLocalB){
							max_xLocalB = tmp_LocalB;
						}
						if (tmp_LocalB < min_xLocalB){
							min_xLocalB = tmp_LocalB;
						}
					}
				}
			}
			else{
				alpha_angle = atan(pente);
				for (int j = 0; j < 6; j++){
					float tmp_hypothenuseA = objA[3 * j + 2] - (pente*objA[3 * j + 0]);
					float tmp_hypothenuseB = objB[3 * j + 2] - (pente*objB[3 * j + 0]);
					tmp_LocalA = sin(alpha_angle)*tmp_hypothenuseA;
					tmp_LocalB = sin(alpha_angle)*tmp_hypothenuseB;
					if (j == 0){
						max_xLocalA = tmp_LocalA;
						min_xLocalA = tmp_LocalA;
						max_xLocalB = tmp_LocalB;
						min_xLocalB = tmp_LocalB;
					}
					else{
						if (tmp_LocalA > max_xLocalA){
							max_xLocalA = tmp_LocalA;
						}
						if (tmp_LocalA < min_xLocalA){
							min_xLocalA = tmp_LocalA;
						}

						if (tmp_LocalB > max_xLocalB){
							max_xLocalB = tmp_LocalB;
						}
						if (tmp_LocalB < min_xLocalB){
							min_xLocalB = tmp_LocalB;
						}
					}
				}
			}
		}

		if (min_xLocalB <= max_xLocalA && min_xLocalB >= min_xLocalA){

		}
		else if (max_xLocalB <= max_xLocalA && max_xLocalB >= min_xLocalA){

		}
		else if (min_xLocalA <= max_xLocalB && min_xLocalA >= min_xLocalB){

		}
		else if (max_xLocalA <= max_xLocalB && max_xLocalA >= min_xLocalB){

		}
		else{
			return false;
		}

	}
	
	return true;
}



/*
	Notre fonction de loading d'objets blender. Les faces sont triangul�es lors de l'export, on recupere seulement les differents points et les faces, puis on combine les points pour former un vector de points compatibles avec le reste du projet.

*/
std::vector<glm::vec3> objLoader(string fichier){
	ifstream file(fichier.c_str(), ios::in);
	string c =  "v ";
	string face = "f ";
	std::vector<glm::vec3> tmp;
	std::vector<glm::vec3> vertices;
	std::vector<triangle> faces;
	if (!file){
		return tmp;
	}
	std::string line;
	float tx, ty, tz;
	while (std::getline(file, line))
	{
		string t = line.substr(0, 2);
		if (t == c){
			string tmpl = line.substr(2);
			int tmp_pos = tmpl.find_first_of(" ");
			string tmpl1 = tmpl.substr(tmp_pos+1);
			int tmp_pos2 = tmpl1.find_first_of(" ");
			string tmp_x = tmpl.substr(0, tmp_pos);
			string tmp_y = tmpl.substr(tmp_pos + 1, tmp_pos2);
			string tmp_z = tmpl1.substr(tmp_pos2+1);
			
			tx = (GLfloat) stof(tmp_x);
			ty = (GLfloat) stof(tmp_y);
			tz = (GLfloat) stof(tmp_z);

			tmp.push_back(glm::vec3(tx, ty, tz));
		}
		else if (t == face){
			string tmp = line.substr(2);
			int tmp_pos = tmp.find_first_of(" ");
			string tmpl1 = tmp.substr(tmp_pos + 1);
			int tmp_pos2 = tmpl1.find_first_of(" ");
			string a = tmp.substr(0, tmp_pos);
			string b = tmp.substr(tmp_pos + 1, tmp_pos2);
			string c = tmpl1.substr(tmp_pos2 + 1);
			triangle t = { t.pt1 = stoi(a)-1, t.pt2 = stoi(b)-1, t.pt3 = stoi(c)-1 };
			faces.push_back(t);
		}
			
		
	}

	file.close();
	for (triangle i : faces){
		vertices.push_back(tmp.at(i.pt1));
		vertices.push_back(tmp.at(i.pt2));
		vertices.push_back(tmp.at(i.pt3));
	}
	return vertices;
}
