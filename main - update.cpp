/*
 * Final Project
 *  main.cpp
 *
 *  Created on: August 14, 2019
 *      Author: trevor.rouse_snhu
 */

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> // Include the freeGLUT header file

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#define WINDOW_TITLE "Modern OpenGL"

#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

bool mouseDetected = true;
bool orbit = false;
bool zoom = false;
GLchar currentKey;
GLfloat cameraSpeed = 0.0005f;
GLfloat lastMouseX = 400, lastMouseY = 300;
GLfloat mouseXOffset, mouseYOffset, yaw = 0.0f, pitch = 0.0f;
GLfloat sensitivity = 0.005f;
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO;

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraUpY = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraForwardZ = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 front = glm::vec3(10.0f, 0.0f, 0.0f);

void Cleanup(void);
void Initializeglut(void);
void Orbit(int mouseXOffset, int mouseYOffset);
void Setup(void);
void UCreateBuffers(void);
void UCreateShader(void);
void UMouseClick(int button, int state, int x, int y);
void UMouseMove(int x, int y);
void URenderGraphics(void);
void UResizeWindow(int, int);
void Zoom(int mouseYOffset);

const GLchar * vertexShaderSource = GLSL(330,
		layout (location = 0) in vec3 position;
		layout (location = 1) in  vec3 color;

		out vec3 mobileColor;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

	void main(){
	gl_Position = projection * view * model * vec4(position, 1.0f);
	mobileColor = color;
	}
);

const GLchar * fragmentShaderSource = GLSL(330,

		in vec3 mobileColor;

		out vec4 gpuColor;

	void main(){

		gpuColor = vec4(mobileColor, 1.0);

	}
);




/* Main function required for Immediate mode */
int main(int argc, char* argv[]) {
	Initializeglut();
	setup();
	glutMainLoop();
	cleanup();

	return 0;
}

void Initializeglut(void){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);
	glutReshapeFunc(UResizeWindow);
}

void Cleanup(void){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Setup(void){
	glewExperimental = GL_TRUE;
				if(glewInit() != GLEW_OK)
				{
					cout << "Failed to initialize GLEW" << endl;
					return -1;
				}

	UCreateShader();
	UCreateBuffers();

	glUseProgram(shaderProgram);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glutDisplayFunc(URenderGraphics);

	glutMouseFunc(UMouseClick);
	glutMotionFunc(UMouseMove);
}

void Zoom(int mouseYOffset){
	if(mouseYOffset > 0)
	{
		cameraPosition += cameraSpeed *cameraForwardZ;
	}
	else
	{
		cameraPosition -= cameraSpeed *cameraForwardZ;
	}
}

void Orbit(int mouseXOffset, int mouseYOffset){
	yaw += mouseXOffset;
	pitch += mouseYOffset;

	if(pitch > 89.0f)
		pitch = 89.0f;

	if(pitch < -89.0f)
		pitch = -89.0f;

	front.x = 10.0f * cos(yaw);
	front.y = 10.0f * sin(pitch);
	front.z = cos(pitch) * sin(yaw) * 10.0f;
}

void UResizeWindow(int width, int height){
	WindowWidth = width;
	WindowHeight = height;
	glViewport(0,0, WindowWidth, WindowHeight);
}

void URenderGraphics(void){
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);

	cameraForwardZ = front;

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

	glm::mat4 view;
	view = glm::lookAt(cameraPosition - cameraForwardZ, cameraPosition, cameraUpY);

	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	GLuint modelLocation = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLocation = glGetUniformLocation(shaderProgram, "view");
	GLuint projectionLocation = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();
	glDrawArrays(GL_TRIANGLES, 0, 294);
	glBindVertexArray(0);
	glutSwapBuffers();
}

void UMouseClick(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && glutGetModifiers() == GLUT_ACTIVE_ALT){
		orbit = true;
	}
	else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && glutGetModifiers() == GLUT_ACTIVE_ALT){
		zoom = true;
	}
	else
	{
		orbit = false;
		zoom = false;
	}
}

void UMouseMove(int x, int y)
{
	if(mouseDetected)
	{
		lastMouseX = x;
		lastMouseY = y;
		mouseDetected = false;
	}

	mouseXOffset = x - lastMouseX;
	mouseYOffset = lastMouseY - y;

	lastMouseX = x;
	lastMouseY = y;

	mouseXOffset *= sensitivity;
	mouseYOffset *= sensitivity;

	if(orbit)
	{
		orbit(mouseXOffset , mouseYOffset);
	}
	else if(zoom)
	{
		zoom(mouseYOffset);
	}
}

void UCreateShader(void){
	GLint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShaderId);

	GLint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShaderId);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShaderId);
	glAttachShader(shaderProgram, fragmentShaderId);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
}


void UCreateBuffers(void)
{
	GLfloat verts[]=
		{
				// Position				Color
				// Bottom
				-0.5f, -0.1f, -0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, -0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f,	 	0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, -0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, -0.1f, -0.5f,		0.0f, 0.0f, 1.0f,

				-0.5f, -0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, -0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, -0.1f, 0.5f,		0.0f, 0.0f, 1.0f,

				-0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, -0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, -0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, -0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,

				0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, -0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, -0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, -0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,

				-0.5f, -0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, -0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, -0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, -0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, -0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, -0.1f, -0.5f,		0.0f, 0.0f, 1.0f,

				-0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, -0.5f,		0.0f, 0.0f, 1.0f,

				// Left
				0.4f, 0.1f, -0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.5f,	 	0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.4f, 1.1f, -0.5f,		0.0f, 0.0f, 1.0f,
				0.4f, 0.1f, -0.5f,		0.0f, 0.0f, 1.0f,

				0.4f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.4f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,

				0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,

				0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,

				0.4f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.4f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.4f, 0.1f, -0.5f,		0.0f, 0.0f, 1.0f,

				0.4f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.4f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.4f, 1.1f, -0.5f,		0.0f, 0.0f, 1.0f,

				// Right
				-0.4f, 0.1f, -0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, -0.5f,	 	0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.4f, 1.1f, -0.5f,		0.0f, 0.0f, 1.0f,
				-0.4f, 0.1f, -0.5f,		0.0f, 0.0f, 1.0f,

				-0.4f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.4f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,

				-0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,

				-0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,

				-0.4f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.4f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.4f, 0.1f, -0.5f,		0.0f, 0.0f, 1.0f,

				-0.4f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.4f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.4f, 1.1f, -0.5f,		0.0f, 0.0f, 1.0f,

				// Back
				-0.5f, 0.1f, 0.4f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.4f,	 	0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.4f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, 0.4f,		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.4f,		0.0f, 0.0f, 1.0f,

				// Snowflake
				-0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.0f, 0.6f, 0.5f,		1.0f, 1.0f, 1.0f,

				0.5f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.0f, 0.6f, 0.5f,		1.0f, 1.0f, 1.0f,

				-0.5f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.0f, 0.6f, 0.5f,		1.0f, 1.0f, 1.0f,

				-0.5f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.0f, 0.6f, 0.5f,		1.0f, 1.0f, 1.0f,

				0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,

				0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,

				-0.5f, 0.1f, 0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, 0.4f,		0.0f, 0.0f, 1.0f,

				-0.5f, 1.1f, 0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, 0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, 0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, 0.4f,		0.0f, 0.0f, 1.0f,

				// Front
				-0.5f, 0.1f, -0.4f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.4f,	 	0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.4f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, -0.4f,		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, -0.4f,		0.0f, 0.0f, 1.0f,

				-0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, -0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f,		0.0f, 0.0f, 1.0f,

				0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.5f,		0.0f, 0.0f, 1.0f,

				0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f,		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.5f,		0.0f, 0.0f, 1.0f,

				-0.5f, 0.1f, -0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, -0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, 0.1f, -0.4f,		0.0f, 0.0f, 1.0f,

				-0.5f, 1.1f, -0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.4f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				0.5f, 1.1f, -0.5f, 		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, -0.5f,		0.0f, 0.0f, 1.0f,
				-0.5f, 1.1f, -0.4f,		0.0f, 0.0f, 1.0f,

				// Handle bottom
				0.5f, 0.3f, -0.05f,		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.4f, -0.05f,		0.0f, 0.0f, 1.0f,
				0.8f, 0.4f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.4f, -0.05f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.3f, -0.05f,		0.0f, 0.0f, 1.0f,

				0.5f, 0.3f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.4f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.4f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.4f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.3f, 0.05f,		0.0f, 0.0f, 1.0f,

				0.5f, 0.4f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.4f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.3f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.3f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.3f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.4f, 0.05f,		0.0f, 0.0f, 1.0f,

				0.8f, 0.4f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.4f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.8f, 0.4f, 0.05f,		0.0f, 0.0f, 1.0f,

				0.5f, 0.3f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.3f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.3f, -0.05f,		0.0f, 0.0f, 1.0f,

				0.5f, 0.4f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.4f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.4f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.4f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.4f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.4f, -0.05f,		0.0f, 0.0f, 1.0f,

				// Handle top
				0.5f, 0.6f, -0.05f,		0.0f, 0.0f, 1.0f,
				0.8f, 0.6f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, -0.05f,		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.7f, -0.05f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.6f, -0.05f,		0.0f, 0.0f, 1.0f,

				0.5f, 0.6f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.6f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.7f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.6f, 0.05f,		0.0f, 0.0f, 1.0f,

				0.5f, 0.7f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.7f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.6f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.6f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.6f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.7f, 0.05f,		0.0f, 0.0f, 1.0f,

				0.8f, 0.7f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.6f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.6f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.6f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, 0.05f,		0.0f, 0.0f, 1.0f,

				0.5f, 0.6f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.6f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.6f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.6f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.6f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.6f, -0.05f,		0.0f, 0.0f, 1.0f,

				0.5f, 0.7f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.5f, 0.7f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.5f, 0.7f, -0.05f,		0.0f, 0.0f, 1.0f,

				// Handle side
				0.7f, 0.3f, -0.05f,		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, -0.05f,		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.7f, 0.7f, -0.05f,		0.0f, 0.0f, 1.0f,
				0.7f, 0.3f, -0.05f,		0.0f, 0.0f, 1.0f,

				0.7f, 0.3f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.7f, 0.7f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.7f, 0.3f, 0.05f,		0.0f, 0.0f, 1.0f,

				0.7f, 0.7f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.7f, 0.7f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.7f, 0.3f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.7f, 0.3f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.7f, 0.3f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.7f, 0.7f, 0.05f,		0.0f, 0.0f, 1.0f,

				0.8f, 0.7f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, 0.05f,		0.0f, 0.0f, 1.0f,

				0.7f, 0.3f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.3f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.7f, 0.3f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.7f, 0.3f, -0.05f,		0.0f, 0.0f, 1.0f,

				0.7f, 0.7f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, -0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.8f, 0.7f, 0.05f, 		0.0f, 0.0f, 1.0f,
				0.7f, 0.7f, 0.05f,		0.0f, 0.0f, 1.0f,
				0.7f, 0.7f, -0.05f,		0.0f, 0.0f, 1.0f
		};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}
