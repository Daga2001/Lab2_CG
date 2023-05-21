#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "VectorMesh.h"
#include "Shader.h"
#include "Camera.h"
#include "MathOGL.h"
#include "CartesianMesh.h"
#include "PointMesh.h"

//------------------------------------------------------------------------------------------------------------
// Variables and objects declaration
//------------------------------------------------------------------------------------------------------------
std::vector<glm::vec3> points;
std::string algorithm_name;
double ox = 0;
double oy = 0;
double oz = 0;
double oxf = 0;
double oyf = 0;
double ozf = 0;
double radius = 0;
unsigned int nVectors = 0;
int widthWin = 800;
int heightWin = 600;

const float toRadians = 3.14159265f / 180.0f;
const float nearPlane = 0.1f;
const float farPlane = 100.0f;
const float deltaPlane = glm::abs(farPlane - nearPlane);
const glm::vec3 windowColor = glm::vec3(153, 75, 214);

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<VectorMesh*> vectorMeshList;
std::vector<PointMesh*> pointsList;
std::vector<Shader> shaderList;
Camera camera;
CartesianMesh* plane;
MathOGL mathGL = MathOGL();

GLfloat cubeW = 1.0f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
// Auxiliar Functions
//------------------------------------------------------------------------------------------------------------

/**
 * This function takes a vector of 3D points and returns a reordered vector where each point is
 * adjacent to its closest neighbor.
 * 
 * @param puntos a vector of glm::vec3 points that needs to be reordered in such a way that the points
 * are adjacent to each other based on their distance.
 * 
 * @return A reordered vector of glm::vec3 points where each point is adjacent to the previous one.
 */
std::vector<glm::vec3> reorder_points_adjacent(std::vector<glm::vec3> puntos) {
	std::vector<glm::vec3> resultado;
	resultado.push_back(puntos[0]);

	while (resultado.size() < puntos.size()) {
		glm::vec3 ultimo_punto = resultado.back();
		float distancia_minima = FLT_MAX;
		glm::vec3 mejor_punto;
		for (glm::vec3 punto : puntos) {
			if (std::find(resultado.begin(), resultado.end(), punto) != resultado.end()) {
				// This point has been added to result
				continue;
			}
			float distancia = glm::distance(ultimo_punto, punto);
			if (distancia < distancia_minima) {
				distancia_minima = distancia;
				mejor_punto = punto;
			}
		}
		resultado.push_back(mejor_punto);
	}

	return resultado;
}

/**
 * The function draws vectors between points in a vector and adds them to a list.
 * 
 * @param points A vector of glm::vec3 objects representing the points in 3D space that the vectors
 * will be drawn between.
 */
void drawVectors(std::vector<glm::vec3> points)
{
	double numberOfPoints = points.size();
	for (unsigned int i = 1; i < numberOfPoints; i++) {
		VectorMesh* vecs = new VectorMesh(points[i].x, points[i].y, points[i].z, 
			glm::vec3(points[i-1].x, points[i-1].y, points[i-1].z));
		vecs->drawVector();
		vectorMeshList.push_back(vecs);
		nVectors++;
	}
}

/**
 * This function draws vectors using Bresenham's algorithm based on a list of 3D points.
 * 
 * @param points A vector of glm::vec3 points representing the endpoints of the vectors to be drawn
 * using Bresenham's line algorithm.
 */
void drawVectorsBresenh(std::vector<glm::vec3> points)
{
	double numberOfPoints = points.size();
	for (unsigned int i = 1; i < numberOfPoints; i++) {
		VectorMesh* vecs = new VectorMesh(points[i].x, points[i].y, points[i].z,
			glm::vec3(points[i - 1].x, points[i - 1].y, points[i - 1].z));
		vecs->drawVector();
		vectorMeshList.push_back(vecs);
		nVectors++;
	}
	// links final vector with initial one
	VectorMesh* vecs = new VectorMesh(points[0].x, points[0].y, points[0].z,
		glm::vec3(points[nVectors].x, points[nVectors].y, points[nVectors].z));
	vecs->drawVector();
	vectorMeshList.push_back(vecs);
	nVectors++;
}

/**
 * This function renders a list of vectors using their respective meshes.
 */
void renderVectors() 
{
	for (unsigned int i = 0; i < nVectors; i++) {
		vectorMeshList[i]->renderVector();
	}
}


/**
 * This function draws a circle using the midpoint algorithm and reorders the points to cover all four
 * quadrants.
 * 
 * @param x_center The x-coordinate of the center of the circle.
 * @param y_center The y-coordinate of the center of the circle.
 * @param points A vector of 3D points that represent the circumference of a circle.
 */
void drawMidPointCircle(double x_center, double y_center, std::vector<glm::vec3> points)
{
	// List of points to reorder points later
	std::vector<glm::vec3> listPoints;

	double numberOfPoints = points.size();
	// Quadrant - 1 x = +, y = +
	// we store here Quadrant 2's points
	listPoints.push_back(glm::vec3(-points[0].x + 2 * x_center, points[0].y, points[0].z));
	for (unsigned int i = 1; i < numberOfPoints; i++) {
		VectorMesh* vecs = new VectorMesh(points[i].x, points[i].y, points[i].z,
			glm::vec3(points[i - 1].x, points[i - 1].y, points[i - 1].z));
		vecs->drawVector();
		listPoints.push_back(glm::vec3(-points[i].x + 2 * x_center, points[i].y, points[i].z));
		printf("point1: (%f, %f, %f)\n", points[i].x, points[i].y, points[i].z);
		vectorMeshList.push_back(vecs);
		nVectors++;
	}

	// reorder points
	points = reorder_points_adjacent(listPoints);
	listPoints.clear();

	// Quadrant - 2 x = -, y = +
	// we store here Quadrant 3's points
	// NOTE: x is store as it comes due to points vector has its x-axis values stored as negative.
	listPoints.push_back(glm::vec3(points[0].x, -points[0].y + 2 * y_center, points[0].z));
	for (unsigned int i = 1; i < numberOfPoints; i++) {
		VectorMesh* vecs = new VectorMesh(points[i].x, points[i].y, points[i].z,
			glm::vec3(points[i - 1].x, points[i - 1].y, points[i - 1].z));
		vecs->drawVector();
		listPoints.push_back(glm::vec3(points[i].x, -points[i].y + 2 * y_center, points[i].z));
		printf("point2: (%f, %f, %f)\n", points[i].x, points[i].y, points[i].z);
		vectorMeshList.push_back(vecs);
		nVectors++;
	}

	// reorder points
	points = reorder_points_adjacent(listPoints);
	listPoints.clear();

	// Quadrant - 3 x = -, y = -
	// we store here Quadrant 4's points
	// NOTE: same logic applied as before, we must take into account the previous signs.
	listPoints.push_back(glm::vec3(-points[0].x + 2 * x_center, points[0].y, points[0].z));
	for (unsigned int i = 1; i < numberOfPoints; i++) {
		VectorMesh* vecs = new VectorMesh(points[i].x, points[i].y, points[i].z,
			glm::vec3(points[i - 1].x, points[i - 1].y, points[i - 1].z));
		vecs->drawVector();
		listPoints.push_back(glm::vec3(-points[i].x + 2 * x_center, points[i].y, points[i].z));
		printf("point3: (%f, %f, %f)\n", points[i].x, points[i].y, points[i].z);
		vectorMeshList.push_back(vecs);
		nVectors++;
	}

	// reorder points
	points = reorder_points_adjacent(listPoints);
	listPoints.clear();

	// Quadrant - 4 x = +, y = -
	for (unsigned int i = 1; i < numberOfPoints; i++) {
		VectorMesh* vecs = new VectorMesh(points[i].x, points[i].y, points[i].z,
			glm::vec3(points[i - 1].x, points[i - 1].y, points[i - 1].z));
		vecs->drawVector();
		printf("point4: (%f, %f, %f)\n", points[i].x, points[i].y, points[i].z);
		vectorMeshList.push_back(vecs);
		nVectors++;
	}
}

/**
 * The function renders a circle by rendering a list of vectors.
 */
void renderCircle()
{
	for (unsigned int i = 0; i < nVectors; i++) {
		vectorMeshList[i]->renderVector();
	}
}

/**
 * The function prints out the values of a 4x4 matrix.
 * 
 * @param matrix The parameter "matrix" is a 4x4 matrix of type glm::mat4. It is a matrix used for
 * transformations in computer graphics, such as translation, rotation, and scaling. The function
 * "printMatrix" takes this matrix as input and prints its values to the console.
 */
void printMatrix(glm::mat4 matrix) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

/**
 * The function creates objects such as meshes, planes, and points, and draws them using various
 * algorithms.
 */
void CreateObjects()
{
	unsigned int surfaceIndices[] = {
		0, 2, 3,
		0, 3, 1,
	};

	GLfloat surfaceVertices[] = {
		// x		y			z
		-deltaPlane, 0.0f, deltaPlane,
		deltaPlane, 0.0f, deltaPlane,
		-deltaPlane, 0.0f, -deltaPlane,
		deltaPlane, 0.0f, -deltaPlane,
	};


	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(surfaceVertices, surfaceIndices, 24, 6);
	meshList.push_back(obj1);

	plane = new CartesianMesh(51, deltaPlane, deltaPlane);
	plane->drawPlane();

	// BIA = Basic incremental algorithm.
	if (algorithm_name == "BIA")
	{
		std::cout << "Ingrese la coordenada x del punto inicial:\n";
		std::cin >> ox;
		std::cout << "Ingrese la coordenada y del punto inicial:\n";
		std::cin >> oy;
		std::cout << "Ingrese la coordenada x del punto final:\n";
		std::cin >> oxf;
		std::cout << "Ingrese la coordenada y del punto final:\n";
		std::cin >> oyf;
		
		points = mathGL.drawLineBasic(ox, oy, oxf, oyf);

		PointMesh* pointMesh = new PointMesh(points);
		pointMesh->drawPoints();
		pointsList.push_back(pointMesh);
		printf("points: %d\n", points.size());

		drawVectors(points);
	}
	// DDA = Digital Differential Analyzer.
	else if (algorithm_name == "DDA")
	{
		std::cout << "Ingrese la coordenada x del punto inicial:\n";
		std::cin >> ox;
		std::cout << "Ingrese la coordenada y del punto inicial:\n";
		std::cin >> oy;
		std::cout << "Ingrese la coordenada x del punto final:\n";
		std::cin >> oxf;
		std::cout << "Ingrese la coordenada y del punto final:\n";
		std::cin >> oyf;

		points = mathGL.drawLineDDA(ox, oy, oxf, oyf);

		PointMesh* pointMesh = new PointMesh(points);
		pointMesh->drawPoints();
		pointsList.push_back(pointMesh);
		printf("points: %d\n", points.size());

		drawVectors(points);
	}
	// BA = Bresenham algorithm.
	else if (algorithm_name == "BA")
	{
		std::cout << "Ingrese la coordenada x del punto inicial:\n";
		std::cin >> ox;
		std::cout << "Ingrese la coordenada y del punto inicial:\n";
		std::cin >> oy;
		std::cout << "Ingrese la coordenada x del punto final:\n";
		std::cin >> oxf;
		std::cout << "Ingrese la coordenada y del punto final:\n";
		std::cin >> oyf;

		points = mathGL.drawLineBres(ox, oy, oxf, oyf);

		PointMesh* pointMesh = new PointMesh(points);
		pointMesh->drawPoints();
		pointsList.push_back(pointMesh);
		printf("points: %d\n", points.size());

		drawVectors(points);
	}
	// MPC = Mid point circle algorithm.
	else if (algorithm_name == "MPC")
	{
		std::cout << "Ingrese la coordenada x del centro del circulo:\n";
		std::cin >> ox;
		std::cout << "Ingrese la coordenada y del centro del circulo:\n";
		std::cin >> oy;
		std::cout << "Ingrese el radio del circulo:\n";
		std::cin >> radius;

		points = mathGL.midPointCircleDraw(ox, oy, radius);
		points = reorder_points_adjacent(points);

		PointMesh* pointMesh = new PointMesh(points);
		pointMesh->drawPoints();
		pointsList.push_back(pointMesh);
		printf("points: %d\n", points.size());

		drawMidPointCircle(ox, oy, points);
	}
	// BCA = Bresenham circle algorithm.
	else if (algorithm_name == "BCA")
	{
		std::cout << "Ingrese la coordenada x del centro del circulo:\n";
		std::cin >> ox;
		std::cout << "Ingrese la coordenada y del centro del circulo:\n";
		std::cin >> oy;
		std::cout << "Ingrese el radio del circulo:\n";
		std::cin >> radius;

		points = mathGL.BresenhamCircle(ox, oy, radius);
		points = reorder_points_adjacent(points);

		PointMesh* pointMesh = new PointMesh(points);
		pointMesh->drawPoints();
		pointsList.push_back(pointMesh);
		printf("points: %d\n", points.size());

		drawVectorsBresenh(points);
	}
}

/**
 * The function creates a shader object from vertex and fragment shader files and adds it to a list of
 * shaders.
 */
void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

//------------------------------------------------------------------------------------------------------------

int main()
{
	try {
		// Handles the input UI.
		std::cout << "Cual sera el ancho del espacio coordenado?:\n";
		std::cin >> widthWin;
		std::cout << "Cual sera el alto del espacio coordenado?:\n";
		std::cin >> heightWin;
		std::cout << "Cual algoritmo quieres probar (RECUERDE USAR MAYUSCULAS!)?\nBIA = Basic incremental algorithm.\nDDA = Digital Differential Analyzer.\nBA = Bresenham algorithm.\nMPC = Mid point circle algorithm.\nBCA = Bresenham circle algorithm:\n";
		std::cin >> algorithm_name;
		std::transform(algorithm_name.begin(), algorithm_name.end(), algorithm_name.begin(), ::toupper);
		std::cout << "El algoritmo elegido es: " << algorithm_name << std::endl;

		mainWindow = Window(widthWin, heightWin);
		mainWindow.Initialise();

		CreateObjects();
		CreateShaders();

		camera = Camera(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), -140.0f, -40.0f, 5.0f, 0.5f);

		GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformAmbientIntensity = 0, uniformAmbientColour = 0;
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), nearPlane, farPlane);

		// Loop until window closed
		while (!mainWindow.getShouldClose())
		{
			double now = glfwGetTime(); // SDL_GetPerformanceCounter();
			deltaTime = now - lastTime; // (now - lastTime)*1000/ SDL_GetPerformaceFrequency()
			lastTime = now;

			// Get + Handle User Input
			glfwPollEvents();

			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

			// Clear the window
			glClearColor(windowColor.x / 256, windowColor.y / 256, windowColor.z / 256, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shaderList[0].UseShader();
			uniformModel = shaderList[0].GetModelLocation();
			uniformProjection = shaderList[0].GetProjectionLocation();
			uniformView = shaderList[0].GetViewLocation();
			uniformAmbientColour = shaderList[0].GetAmbientColourLocation();
			uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();

			glm::mat4 model(1.0f);

			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			meshList[0]->RenderMesh();

			// MPC = Mid point circle algorithm.
			// NOTE: at the moment there's no difference between renderCircle and renderVectors functions,
			// but the script is designed like this due to basically it can be extensible for future modifications.
			if (algorithm_name == "MPC")
			{
				renderCircle();
			}
			else 
			{
				renderVectors();
			}
			
			plane->renderPlane();
			pointsList[0]->renderPoints();

			glUseProgram(0);

			mainWindow.swapBuffers();
		}

		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "Se produjo una excepci�n: " << e.what() << std::endl;
	}

}