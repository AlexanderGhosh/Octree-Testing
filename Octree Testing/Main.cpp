#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>

#include "Timer.h"
#include "Octree.h"

#define MAX_OBJECTS 8
#define MIN_OBJECTS 1
#define MAX_CHILDREN 8
#define WORLD_SPACE_X 5
#define WORLD_SPACE_Y 5
#define WORLD_SPACE_Z 5

#define MAX_RECERSIVE_DEPTH 10
#define MAX_ITTERATIONS 1

constexpr glm::ivec2 DIMENTIONS(800, 400);
constexpr glm::fvec3 BG_COLOUR(0.5, 0.75, 0.5);

using namespace std;

list<Octree> trees;
GLFWwindow* window;
unsigned VBO, VAO;

float randRange(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

std::array<BoundingBox, MAX_CHILDREN> Subdivide(BoundingBox& box) {
	std::array<BoundingBox, MAX_CHILDREN> res{};
	const Vec3 length = box.Length();
	const float half_x = length.x * 0.5f;
	const float half_y = length.y * 0.5f;
	const float half_z = length.z * 0.5f;

	const Vec3 half(half_x, half_y, half_z);

	const BoundingBox translated = box - box.min;

	const BoundingBox tr1 = translated * 0.5f + half;
	const BoundingBox tl1 = tr1 + Vec3(-half_x, 0, 0);
	const BoundingBox tr2 = tr1 + Vec3(0, 0, -half_z);
	const BoundingBox tl2 = tr1 + Vec3(-half_x, 0, -half_z);

	const BoundingBox br1 = translated * 0.5f + Vec3(half_x, 0, half_z);
	const BoundingBox bl1 = br1 + Vec3(-half_x, 0, 0);
	const BoundingBox br2 = br1 + Vec3(0, 0, -half_z);
	const BoundingBox bl2 = br1 + Vec3(-half_x, 0, -half_z);

	res[0] = tr1 + box.min;
	res[1] = tl1 + box.min;
	res[2] = tr2 + box.min;
	res[3] = tl2 + box.min;
				
	res[4] = br1 + box.min;
	res[5] = bl1 + box.min;
	res[6] = br2 + box.min;
	res[7] = bl2 + box.min;

	return res;
}

Octree* CreateNode() {
	return &trees.emplace_back();
}

int recursion = 0;
void BuildTree(Octree* node) {
	// max reccusion depth reached
	if (recursion >= MAX_RECERSIVE_DEPTH) {
		return;
	}
	// node doesnt exist
	if (!node) {
		return;
	}
	// already subdivided enough
	if (node->objects.size() <= MIN_OBJECTS) {
		return;
	}

	recursion++;

	// subdivide the bounding box
	auto subdivisions = Subdivide(node->box);
	// create children as needed and check bounding
	int i = -1;
	for (auto& box : subdivisions) {
		i++;
		Octree* child = nullptr;
		for (auto itt = node->objects.begin(); itt != node->objects.end();) {
			Vec3& obj = **itt;
			if (box.Contains(obj)) {
				// create child and add to parent
				if (!child) {
					child = CreateNode();
					child->box = box;
					node->AddChild(child);
				}
				child->AddObject(obj);
				itt = node->objects.erase(itt);
			}
			else {
				itt++;
			}
		}
	}
	/*for (int i = 0; i < MAX_CHILDREN; i++) {
		Octree* child = CreateNode();
		child->box = subdivisions[i];
		node->AddChild(child);
	}*/

	// test objects against children
	/*for (auto c_itt = node->children.begin(); c_itt != node->children.end(); c_itt++) {
		Octree* child = *c_itt;
		for (auto itt = node->objects.begin(); itt != node->objects.end();) {
			Vec3& obj = **itt;
			if (child->box.Contains(obj)) {
				child->AddObject(obj);
				itt = node->objects.erase(itt);
			}
			else {
				itt++;
			}
		}
	}*/
	// recersive on each child
	for (auto itt = node->children.begin(); itt != node->children.end(); itt++) {
		Octree* child = *itt;
		BuildTree(child);
	}
	
}


void initOpenGL(GLFWwindow*& window);
void closeOpenGL();
int initShader();
void initBuffers();
void destroyBuffers();

void main() {
	srand(0);
	vector<Vec3> objects;
	objects.reserve(MAX_OBJECTS);
	objects.resize(MAX_OBJECTS);
	trees.resize(1);
	// generate random objects within the world space
	for (int i = 0; i < MAX_OBJECTS; i++) {
		objects[i] = Vec3(
			randRange(-WORLD_SPACE_X, WORLD_SPACE_X),
			randRange(-WORLD_SPACE_Y, WORLD_SPACE_Y),
			randRange(-WORLD_SPACE_Z, WORLD_SPACE_Z));

		//objects[i] = Vec3(2, 2, 2);
	}

	Vec3 worldMax(WORLD_SPACE_X, WORLD_SPACE_Y, WORLD_SPACE_Z);
	Vec3 worldMin(-WORLD_SPACE_X, -WORLD_SPACE_Y, -WORLD_SPACE_Z);
	
	BoundingBox worldBox(worldMax, worldMin);
	auto subDivisions = Subdivide(worldBox);

	Octree* root = &trees.front();
	root->box = worldBox;
	for (auto& obj : objects) {
		root->objects.push_back(&obj);
	}

	float total = 0;

	Timer timer;
	for (int i = 0; i < MAX_ITTERATIONS; i++)
	{
		timer.start();


		BuildTree(root);


		Octree::idCounter = 1;
		trees.clear();
		trees.emplace_back();
		root = &trees.front();
		root->box = worldBox;
		for (auto& obj : objects) {
			root->objects.push_back(&obj);
		}
		timer.stop();
		total += timer.getDuration(0);
	}
	total /= (float)MAX_ITTERATIONS;
	cout << to_string(total);

	glm::vec3 viewPos(0, 0, -10), fwd(0, 0 ,-1);
	glm::mat4 view(1), proj(1);

	view = glm::lookAt(viewPos, viewPos + fwd, { 0, 1, 0 });
	proj = glm::perspective<float>(glm::radians(45.0f), (float)DIMENTIONS.x / (float)DIMENTIONS.y, 0.1, 1000);

	initOpenGL(window);
	initBuffers();
	glPointSize(5);
	const int shader = initShader();

	while (!glfwWindowShouldClose(window)) {
		glClearColor(BG_COLOUR.x, BG_COLOUR.y, BG_COLOUR.z, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(shader);

		const float radius = 5.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		camX = 0;
		camZ = 5;
		glm::mat4 view;
		view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		// view = glm::lookAt(viewPos, viewPos + fwd, { 0, 1, 0 });

		int loc = glGetUniformLocation(shader, "view");
		glUniformMatrix4fv(loc, 1, GL_TRUE, &view[0][0]);

		loc = glGetUniformLocation(shader, "proj");
		glUniformMatrix4fv(loc, 1, GL_TRUE, &proj[0][0]);

		glm::mat4 model(1);
		model = glm::translate(model, { 0, 0, 0 });
		loc = glGetUniformLocation(shader, "model");
		glUniformMatrix4fv(loc, 1, GL_TRUE, &model[0][0]);

		glBindVertexArray(VAO);

		glDrawArrays(GL_POINTS, 0, 3);

		glfwSwapInterval(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	destroyBuffers();
	closeOpenGL();
}

void initOpenGL(GLFWwindow*& win) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_SAMPLES, 4);
	// glfwSetErrorCallback(GLFWErrorCallBack);
	win = glfwCreateWindow(DIMENTIONS.x, DIMENTIONS.y, "Cellular Automation", NULL, NULL);
	if (!win)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(win);

	// glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glViewport(0, 0, DIMENTIONS.x, DIMENTIONS.y);
	glewInit();

	// glfwSetKeyCallback(window, keyCallBack);
}

void closeOpenGL()
{
	glfwMakeContextCurrent(nullptr);
	glfwDestroyWindow(window);
	window = nullptr;
	glfwTerminate();
}


int initShader()
{
	const std::string filePath = "Shader";
	const std::string extensions[] = { "/vert.glsl", "/frag.glsl" };
	std::string codes[] = { "", "", "" };
	std::ifstream streams[] = { { }, { }, { } };
	const GLenum types[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
	unsigned shaders[] = { 0, 0, 0 };

	int success;
	char infoLog[512];
	for (short i = 0; i < 2; i++) {
		const std::string& extension = extensions[i];
		std::string& code = codes[i];
		std::ifstream& stream = streams[i];
		const GLenum& type = types[i];
		unsigned& shader = shaders[i];

		stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			stream.open(filePath + extension);
			std::stringstream s;
			// read file's buffer contents into streams
			s << stream.rdbuf();
			// close file handlers
			stream.close();
			// convert stream into string
			code = s.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << filePath + extension << " not read" << std::endl;
		}
		const char* c = code.c_str();

		shader = glCreateShader(type);
		glShaderSource(shader, 1, &c, NULL);
		glCompileShader(shader);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << filePath + extension << " faild to compile " << infoLog << std::endl;
		}
	}

	// link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, shaders[0]); // vertex
	glAttachShader(shaderProgram, shaders[1]); // fragment
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << filePath << " faild to link " << infoLog << std::endl;
	}

	glDetachShader(shaderProgram, shaders[0]);
	glDetachShader(shaderProgram, shaders[1]);

	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	glUseProgram(0);
	return shaderProgram;
}

void destroyBuffers()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}


void initBuffers()
{
	std::vector<float> data{
		 -1, -1,  -1,
		 -0.5, -0.5,  -0.5,
		 0, 0, 0
	};

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), &data[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
