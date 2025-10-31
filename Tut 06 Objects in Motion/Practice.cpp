#include <string>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))

GLuint theProgram;

GLuint modelToCameraMatrix;
GLuint cameraToClipMatrix;

glm::mat4 cameraToClipMatrix(0.0f);

//frustum scale is the cotangent of the FoV/2
float CalcFrustumScale(float fFovDeg){
    const float degToRad = (3.14159f * 2.0f) / 360.0f;
    float fFovRad = fFovDeg * degToRad;
    return 1.0f / tan(fFovDeg / 2.0f);
}

const float fFrustumScale = CalcFrustumScale(45.0f);

//create the program object, this includes linking shaders
void InitializeProgram()
{
    std::vector<GLuint> shaderList;
    shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "Practice.vert"));
    shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "Practice.frag"));

    theProgram = Framework::CreateProgram(shaderList);

    //get uniform locations
    modelToCameraMatrix = glGetUniformLocation(theProgram, "modelToCameraMatrix");
    cameraToClipMatrix = glGetUniformLocation(theProgram, "cameraToClipMatrix");

    //these values are positive by convention, and zNear can't be 0
    float fzNear = 1.0f; float fzFar = 45.0f;

    cameraToClipMatrix[0].x = fFrustumScale;
	cameraToClipMatrix[1].y = fFrustumScale;
	cameraToClipMatrix[2].z = (fzFar + fzNear) / (fzNear - fzFar);
	cameraToClipMatrix[2].w = -1.0f;
	cameraToClipMatrix[3].z = (2 * fzFar * fzNear) / (fzNear - fzFar);

    glUseProgram(theProgram);
    glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
    glUseProgram(0);
}

//vertex position, color and index data
const float vertexData[] =
{
	+1.0f, +1.0f, +1.0f,
	-1.0f, -1.0f, +1.0f,
	-1.0f, +1.0f, -1.0f,
	+1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	+1.0f, +1.0f, -1.0f,
	+1.0f, -1.0f, +1.0f,
	-1.0f, +1.0f, +1.0f,

	GREEN_COLOR,
	BLUE_COLOR,
	RED_COLOR,
	BROWN_COLOR,

	GREEN_COLOR,
	BLUE_COLOR,
	RED_COLOR,
	BROWN_COLOR,
};

const GLshort indexData[] =
{
	0, 1, 2,
	1, 0, 3,
	2, 3, 0,
	3, 2, 1,

	5, 4, 6,
	4, 5, 7,
	7, 6, 4,
	6, 7, 5,
};

GLuint vertexBufferObject;
Gluint indexBufferObject;
Gluint vao;

void InitializeVertexBuffer()
{
    glGenBuffers(1, &vertexBufferObject);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glGenBuffers(1, &indexBufferObject);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//defines 3 offsets for different translation examples

glm::vec3 StationaryOffset(float fElapsedTime){
    return glm::vec3(0.0f, 0.0f, -20.0f);
}

glm::vec3 OvalOffset(float fElapsedTime){
    const float fLoopDuration = 3.0f;
    const float fScale = 3.14159f * 2.0f / fLoopDuration;

    float fCurrTimeThroughLoop = fmodf(fElapsedTime, fLoopDuration);

    return glm::vec3(cosf(fCurrTimeThroughLoop * fScale) * 4.f,
		sinf(fCurrTimeThroughLoop * fScale) * 6.f,
		-20.0f);
}

glm:: vec3 BottomCircleOffset(float fElapsedTime)
{
    const float fLoopDuration = 12.0f;
	const float fScale = 3.14159f * 2.0f / fLoopDuration;

    float fCurrTimeThroughLoop = fmodf(fElapsedTime, fLoopDuration);

    return glm::vec3(cosf(fCurrTimeThroughLoop * fScale) * 5.f,
		-3.5f,
		sinf(fCurrTimeThroughLoop * fScale) * 5.f - 20.0f);
}


struct Instance
{
    //defines a type named OffsetFunc
    //it's a function that has a float parameter and returns a glm::vec3
	typedef glm::vec3(*OffsetFunc)(float);

	OffsetFunc CalcOffset;

	glm::mat4 ConstructMatrix(float fElapsedTime)
	{
		glm::mat4 theMat(1.0f);

		theMat[3] = glm::vec4(CalcOffset(fElapsedTime), 1.0f);

		return theMat;
	}
};