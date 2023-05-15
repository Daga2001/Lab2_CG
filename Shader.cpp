#include "Shader.h"

/**
 * This is a constructor function for a Shader class that initializes some variables to zero.
 */
Shader::Shader()
{
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;
}

/**
 * This function compiles a shader from vertex and fragment code provided as strings.
 * 
 * @param vertexCode A string containing the source code for the vertex shader.
 * @param fragmentCode The fragmentCode parameter is a C-style string that contains the source code for
 * the fragment shader.
 */
void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
	CompileShader(vertexCode, fragmentCode);
}

/**
 * This function creates a shader program by reading and compiling vertex and fragment shader code from
 * files.
 * 
 * @param vertexLocation A string representing the file path of the vertex shader source code.
 * @param fragmentLocation The file path or location of the fragment shader source code file.
 */
void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);
}

/**
 * This function reads the contents of a file located at a given file path and returns it as a string.
 * 
 * @param fileLocation A pointer to a character array that represents the file location or path of the
 * file to be read.
 * 
 * @return The function `ReadFile` returns a `std::string` object that contains the contents of a file
 * located at the specified file location. If the file cannot be opened or read, an empty string is
 * returned.
 */
std::string Shader::ReadFile(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open()) {
		printf("Failed to read %s! File doesn't exist.", fileLocation);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

/**
 * This function compiles and links a shader program and sets uniform variables for projection, model,
 * view, ambient colour, and ambient intensity.
 * 
 * @param vertexCode A string containing the source code for the vertex shader.
 * @param fragmentCode A string containing the source code for the fragment shader.
 * 
 * @return The function does not have a return type, but it does have several return statements that
 * will exit the function early if certain conditions are met.
 */
void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
	shaderID = glCreateProgram();

	if (!shaderID)
	{
		printf("Error creating shader program!\n");
		return;
	}

	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	uniformProjection = glGetUniformLocation(shaderID, "projection");
	uniformModel = glGetUniformLocation(shaderID, "model");
	uniformView = glGetUniformLocation(shaderID, "view");
	uniformAmbientColour = glGetUniformLocation(shaderID, "directionalLight.colour");
	uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.ambientIntensity");
}

/**
 * This function returns the location of the projection uniform in a shader program.
 * 
 * @return the value of the variable `uniformProjection`, which is likely a GLuint (an unsigned integer
 * used to represent OpenGL object handles) representing the location of a uniform variable in an
 * OpenGL shader program. This function is likely part of a larger class or set of functions used to
 * manage and interact with OpenGL shaders.
 */
GLuint Shader::GetProjectionLocation()
{
	return uniformProjection;
}

/**
 * The function returns the location of the uniform variable "uniformModel" in the shader program.
 * 
 * @return the value of the variable `uniformModel`, which is likely a GLuint (unsigned integer)
 * representing the location of a uniform variable in a shader program.
 */
GLuint Shader::GetModelLocation()
{
	return uniformModel;
}

/**
 * This function returns the location of the "view" uniform variable in a shader program.
 * 
 * @return The function `GetViewLocation()` is returning a `GLuint` variable named `uniformView`.
 */
GLuint Shader::GetViewLocation()
{
	return uniformView;
}

/**
 * This function returns the location of the uniform variable for ambient colour in a shader program.
 * 
 * @return The function `GetAmbientColourLocation()` is returning a `GLuint` variable named
 * `uniformAmbientColour`.
 */
GLuint Shader::GetAmbientColourLocation()
{
	return uniformAmbientColour;
}

/**
 * This function returns the location of the uniform variable for ambient intensity in a shader
 * program.
 * 
 * @return The function `GetAmbientIntensityLocation` is returning a `GLuint` variable named
 * `uniformAmbientIntensity`.
 */
GLuint Shader::GetAmbientIntensityLocation()
{
	return uniformAmbientIntensity;
}

/**
 * This function sets the current shader program to be used for rendering.
 */
void Shader::UseShader()
{
	glUseProgram(shaderID);
}

/**
 * The function clears the shader program and resets the uniform variables to zero.
 */
void Shader::ClearShader()
{
	if (shaderID != 0)
	{
		glDeleteProgram(shaderID);
		shaderID = 0;
	}

	uniformModel = 0;
	uniformProjection = 0;
}


/**
 * This function compiles and attaches a shader to a program in OpenGL.
 * 
 * @param theProgram The ID of the shader program to which the shader will be attached.
 * @param shaderCode A string containing the source code of the shader to be compiled and attached to
 * the program.
 * @param shaderType The type of shader being compiled, which can be either GL_VERTEX_SHADER or
 * GL_FRAGMENT_SHADER.
 * 
 * @return The function does not return anything, it is a void function.
 */
void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader);
}

/**
 * This is a destructor function for the Shader class that calls the ClearShader function.
 */
Shader::~Shader()
{
	ClearShader();
}
