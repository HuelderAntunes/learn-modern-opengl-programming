#include <iostream>
#include <sstream>

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

const char* APP_TITLE = "Modern OpenGL - Hello Triangle";
const int gWindowWidth = 800;
const int gWindowHeight = 600;

GLFWwindow* gWindow;


void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void showFPS(GLFWwindow* window);
bool initOpenGL();


const GLchar* vertexShaderSrc = 
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"layout (location = 1) in vec3 color;"
"out vec3 vert_color;"
"void main() {"
"	vert_color = color;"
"	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);"
"}";

const GLchar* fragmentShaderSrc =
"#version 330 core\n"
"in vec3 vert_color;"
"out vec4 frag_color;"
"void main() {"
"	frag_color = vec4(vert_color, 1.0f);"
"}";


int main() {
	
	if (!initOpenGL()) {
		std::cerr << "GLFW intilization failed" << std::endl;
	}

	GLfloat vert_pos[] = {
		// position        //color
		 0.0f,  0.5f, 0.0f,  //top
		 0.5f, -0.5f, 0.0f, //right
		-0.5f, -0.5f, 0.0f//left
	};
	GLfloat vert_color[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	// Ver Buffer Object

	GLuint vbo, vbo2, vao;

	// position
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert_color), vert_color, GL_STATIC_DRAW);

	// colors
	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert_color), vert_color, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, NULL);
	glEnableVertexAttribArray(0);

	// color 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(sizeof(GLfloat)*3));
	glEnableVertexAttribArray(1);



	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShaderSrc, NULL);
	glCompileShader(vs);
	
	GLint result;
	GLchar infoLog[512];

	glGetShaderiv(vs, GL_COMPILE_STATUS, &result);

	if (!result) {
		glGetShaderInfoLog(vs, sizeof(infoLog), NULL, infoLog);
		std::cout << "Error! Vertex shader failed to compile. " << infoLog << std::endl;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &result);

	if (!result) {
		glGetShaderInfoLog(fs, sizeof(infoLog), NULL, infoLog);
		std::cout << "Error! Fragment shader failed to compile. " << infoLog << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
		std::cout << "Error! Shader Program linker failure" << infoLog << std::endl;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	/* Main Loop */
	while(!glfwWindowShouldClose(gWindow)) {
		showFPS(gWindow);

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(gWindow);
	}

	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	glfwTerminate();
	return 0;
}

bool initOpenGL() {

	if (!glfwInit()) {
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	

	if (gWindow == NULL) {
		std::cerr << "Failed to create GLDW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(gWindow);

	glfwSetKeyCallback(gWindow, glfw_onKey);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW initialization failed" << std::endl;
		return false;
	}
	
	glClearColor(0.23f, 0.38f, 0.80f, 1.0f);
	

	return true;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void showFPS(GLFWwindow* window) {
	static double previusSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // return number of seconds since glfw started, as double
	
	elapsedSeconds = currentSeconds - previusSeconds;

	// limit text
	if (elapsedSeconds > 0.25) {
		previusSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed
			<< APP_TITLE << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		frameCount = 0;
	}

	frameCount++;
}