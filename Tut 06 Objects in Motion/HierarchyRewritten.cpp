#include <string>
#include <vector>
#include <stack>
#include <math.h>
#include <stdio.h>
#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))

GLuint theProgram;
GLuint positionAttrib;
GLuint colorAttrib;

GLuint modelToCameraMatrixUnif;
GLuint cameraToClipMatrixUnif;

glm::mat4 cameraToClipMatrix(0.0f);

float CalcFrustumScale(float fFovDeg)
{
	const float degToRad = 3.14159f * 2.0f / 360.0f;
	float fFovRad = fFovDeg * degToRad;
	return 1.0f / tan(fFovRad / 2.0f);
}

const float fFrustumScale = CalcFrustumScale(45.0f);

void InitializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "Tut06PosColorLocalTransform.vert"));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "Tut06ColorPassthrough.frag"));

	theProgram = Framework::CreateProgram(shaderList);

	positionAttrib = glGetAttribLocation(theProgram, "position");
	colorAttrib = glGetAttribLocation(theProgram, "color");

	modelToCameraMatrixUnif = glGetUniformLocation(theProgram, "modelToCameraMatrix");
	cameraToClipMatrixUnif = glGetUniformLocation(theProgram, "cameraToClipMatrix");

	float fzNear = 1.0f; float fzFar = 100.0f;

	cameraToClipMatrix[0].x = fFrustumScale;
	cameraToClipMatrix[1].y = fFrustumScale;
	cameraToClipMatrix[2].z = (fzFar + fzNear) / (fzNear - fzFar);
	cameraToClipMatrix[2].w = -1.0f;
	cameraToClipMatrix[3].z = (2 * fzFar * fzNear) / (fzNear - fzFar);

	glUseProgram(theProgram);
	glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
	glUseProgram(0);
}

const int numberOfVertices = 24;

#define RED_COLOR 1.0f, 0.0f, 0.0f, 1.0f
#define GREEN_COLOR 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE_COLOR 	0.0f, 0.0f, 1.0f, 1.0f

#define YELLOW_COLOR 1.0f, 1.0f, 0.0f, 1.0f
#define CYAN_COLOR 0.0f, 1.0f, 1.0f, 1.0f
#define MAGENTA_COLOR 	1.0f, 0.0f, 1.0f, 1.0f

const float vertexData[] =
{
	//Front
	+1.0f, +1.0f, +1.0f,
	+1.0f, -1.0f, +1.0f,
	-1.0f, -1.0f, +1.0f,
	-1.0f, +1.0f, +1.0f,

	//Top
	+1.0f, +1.0f, +1.0f,
	-1.0f, +1.0f, +1.0f,
	-1.0f, +1.0f, -1.0f,
	+1.0f, +1.0f, -1.0f,

	//Left
	+1.0f, +1.0f, +1.0f,
	+1.0f, +1.0f, -1.0f,
	+1.0f, -1.0f, -1.0f,
	+1.0f, -1.0f, +1.0f,

	//Back
	+1.0f, +1.0f, -1.0f,
	-1.0f, +1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	+1.0f, -1.0f, -1.0f,

	//Bottom
	+1.0f, -1.0f, +1.0f,
	+1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, +1.0f,

	//Right
	-1.0f, +1.0f, +1.0f,
	-1.0f, -1.0f, +1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, +1.0f, -1.0f,


	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,

	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,

	RED_COLOR,
	RED_COLOR,
	RED_COLOR,
	RED_COLOR,

	YELLOW_COLOR,
	YELLOW_COLOR,
	YELLOW_COLOR,
	YELLOW_COLOR,

	CYAN_COLOR,
	CYAN_COLOR,
	CYAN_COLOR,
	CYAN_COLOR,

	MAGENTA_COLOR,
	MAGENTA_COLOR,
	MAGENTA_COLOR,
	MAGENTA_COLOR,
};

const GLshort indexData[] =
{
	0, 1, 2,
	2, 3, 0,

	4, 5, 6,
	6, 7, 4,

	8, 9, 10,
	10, 11, 8,

	12, 13, 14,
	14, 15, 12,

	16, 17, 18,
	18, 19, 16,

	20, 21, 22,
	22, 23, 20,
};

GLuint vertexBufferObject;
GLuint indexBufferObject;
GLuint vao;

void InitializeVAO()
{
	glGenBuffers(1, &vertexBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBufferObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	size_t colorDataOffset = sizeof(float) * 3 * numberOfVertices;
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glEnableVertexAttribArray(positionAttrib);
	glEnableVertexAttribArray(colorAttrib);
	glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorDataOffset);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);

	glBindVertexArray(0);
}

inline float DegToRad(float fAngDeg)
{
	const float fDegToRad = 3.14159f * 2.0f / 360.0f;
	return fAngDeg * fDegToRad;
}

inline float Clamp(float fValue, float fMinValue, float fMaxValue)
{
	if(fValue < fMinValue)
		return fMinValue;

	if(fValue > fMaxValue)
		return fMaxValue;

	return fValue;
}

glm::mat3 RotateX(float fAngDeg)
{
	float fAngRad = DegToRad(fAngDeg);
	float fCos = cosf(fAngRad);
	float fSin = sinf(fAngRad);

	glm::mat3 theMat(1.0f);
	theMat[1].y = fCos; theMat[2].y = -fSin;
	theMat[1].z = fSin; theMat[2].z = fCos;
	return theMat;
}

glm::mat3 RotateY(float fAngDeg)
{
	float fAngRad = DegToRad(fAngDeg);
	float fCos = cosf(fAngRad);
	float fSin = sinf(fAngRad);

	glm::mat3 theMat(1.0f);
	theMat[0].x = fCos; theMat[2].x = fSin;
	theMat[0].z = -fSin; theMat[2].z = fCos;
	return theMat;
}

glm::mat3 RotateZ(float fAngDeg)
{
	float fAngRad = DegToRad(fAngDeg);
	float fCos = cosf(fAngRad);
	float fSin = sinf(fAngRad);

	glm::mat3 theMat(1.0f);
	theMat[0].x = fCos; theMat[1].x = -fSin;
	theMat[0].y = fSin; theMat[1].y = fCos;
	return theMat;
}

class MatrixStack
{
public:
	MatrixStack()
		: m_currMat(1.0f)
	{
	}

	const glm::mat4 &Top()
	{
		return m_currMat;
	}

	void RotateX(float fAngDeg)
	{
		m_currMat = m_currMat * glm::mat4(::RotateX(fAngDeg));
	}

	void RotateY(float fAngDeg)
	{
		m_currMat = m_currMat * glm::mat4(::RotateY(fAngDeg));
	}

	void RotateZ(float fAngDeg)
	{
		m_currMat = m_currMat * glm::mat4(::RotateZ(fAngDeg));
	}

	void Scale(const glm::vec3 &scaleVec)
	{
		glm::mat4 scaleMat(1.0f);
		scaleMat[0].x = scaleVec.x;
		scaleMat[1].y = scaleVec.y;
		scaleMat[2].z = scaleVec.z;

		m_currMat = m_currMat * scaleMat;
	}

	void Translate(const glm::vec3 &offsetVec)
	{
		glm::mat4 translateMat(1.0f);
		translateMat[3] = glm::vec4(offsetVec, 1.0f);

		m_currMat = m_currMat * translateMat;
	}

	void Push()
	{
		m_matrices.push(m_currMat);
	}

	void Pop()
	{
		m_currMat = m_matrices.top();
		m_matrices.pop();
	}

private:
	glm::mat4 m_currMat;
	std::stack<glm::mat4> m_matrices;
};

//write new node class here

class Node
{
public:
    Node(MatrixStack *m_stackParam, glm::vec3 angleParam, glm::vec3 scaleParam, glm::vec3 posParam, bool drawParam)
	: m_stack(m_stackParam),
	angle(angleParam),
	scale(scaleParam),
	position(posParam),
	drawBool(drawParam){};

	//recursive render function
    void renderNode(){
		
		m_stack->Push();
		m_stack->Translate(position);
		if (angle.x != 0) m_stack->RotateX(angle.x);
		if (angle.y != 0) m_stack->RotateY(angle.y);
		if (angle.z != 0) m_stack->RotateZ(angle.z);
		m_stack->Scale(scale);

		//if the node needs to be drawn
		if (drawBool) {
			glUseProgram(theProgram);
			glBindVertexArray(vao);
			glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(m_stack->Top()));
			glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
			glBindVertexArray(0);
			glUseProgram(0);
			m_stack->Pop();
		}

		//if(this->position != glm::vec3(3.0f, -5.0f, -40.0f)) m_stack->Pop();

		//Stopping Condition: Node has no children
		if (children.empty()) {
			return;
		}

		for (Node* n : children){
			n->renderNode();
		}

		m_stack->Pop();
	}

	void attachChild(Node *child){
		children.push_back(child);
	}

private:
    std::vector<Node*> children;
    MatrixStack *m_stack;
    glm::vec3 angle;
    glm::vec3 scale;
    glm::vec3 position;
	bool drawBool; //tells us weather to draw the node or not
};

struct SceneData{
	SceneData()
		: posBase(glm::vec3(3.0f, -5.0f, -40.0f))
		, angBase(-45.0f)
		, posBaseLeft(glm::vec3(2.0f, 0.0f, 0.0f))
		, posBaseRight(glm::vec3(-2.0f, 0.0f, 0.0f))
		, scaleBaseZ(3.0f)
		, angUpperArm(-33.75f)
		, sizeUpperArm(9.0f)
		, posLowerArm(glm::vec3(0.0f, 0.0f, 8.0f))
		, angLowerArm(146.25f)
		, lenLowerArm(5.0f)
		, widthLowerArm(1.5f)
		, posWrist(glm::vec3(0.0f, 0.0f, 5.0f))
		, angWristRoll(0.0f)
		, angWristPitch(67.5f)
		, lenWrist(2.0f)
		, widthWrist(2.0f)
		, posLeftFinger(glm::vec3(1.0f, 0.0f, 1.0f))
		, posRightFinger(glm::vec3(-1.0f, 0.0f, 1.0f))
		, angFingerOpen(180.0f)
		, lenFinger(2.0f)
		, widthFinger(0.5f)
		, angLowerFinger(45.0f)
	{}

	void renderScene(){
		glm::vec3 noScale(1.0f, 1.0f, 1.0f);
		glm::vec3 noRotation(0.0f, 0.0f, 0.0f);
		glm::vec3 noTranslation(0.0f, 0.0f, 0.0f);

		MatrixStack modelToCameraStack;

		Node baseNode(&modelToCameraStack, 
			glm::vec3(0.0f,angBase, 0.0f),
			noScale,
			posBase,
			false);
		
		Node leftBase(&modelToCameraStack, 
			noRotation,
			glm::vec3(1.0f, 1.0f, scaleBaseZ),
			posBaseLeft,
			true);
		
		Node rightBase(&modelToCameraStack, 
			noRotation,
			glm::vec3(1.0f, 1.0f, scaleBaseZ),
			posBaseRight,
			true);
		
		Node upperArmNode(&modelToCameraStack, 
			glm::vec3(angUpperArm,0.0f, 0.0f),
			noScale,
			noTranslation,
			false);
		
		Node upperArmDraw(&modelToCameraStack, 
			noRotation,
			glm::vec3(1.0f, 1.0f, sizeUpperArm / 2.0f),
			glm::vec3(0.0f, 0.0f, (sizeUpperArm / 2.0f) - 1.0f),
			true);
		
		Node lowerArmNode(&modelToCameraStack, 
			glm::vec3(angLowerArm,0.0f, 0.0f),
			noScale,
			posLowerArm,
			false);
		
		Node lowerArmDraw(&modelToCameraStack, 
			noRotation,
			glm::vec3(widthLowerArm / 2.0f, widthLowerArm / 2.0f, lenLowerArm / 2.0f),
			glm::vec3(0.0f, 0.0f, lenLowerArm / 2.0f),
			true);
		
		Node wristNode(&modelToCameraStack, 
			glm::vec3(angWristPitch,0.0f, angWristRoll),
			noScale,
			posWrist,
			false);
		
		Node wristDraw(&modelToCameraStack, 
			noRotation,
			glm::vec3(widthWrist / 2.0f, widthWrist/ 2.0f, lenWrist / 2.0f),
			noTranslation,
			true);
		
		Node leftFingerNode(&modelToCameraStack, 
			glm::vec3(0.0f, angFingerOpen, 0.0f),
			noScale,
			posLeftFinger,
			false);

		Node rightFingerNode(&modelToCameraStack, 
			glm::vec3(0.0f, -angFingerOpen, 0.0f),
			noScale,
			posRightFinger,
			false);
		
		Node leftUpperFingerDraw(&modelToCameraStack, 
			noRotation,
			glm::vec3(glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f)),
			glm::vec3(0.0f, 0.0f, lenFinger / 2.0f),
			true);
		
		Node leftLowerFingerNode(&modelToCameraStack, 
			glm::vec3(0.0f, -angLowerFinger, 0.0f),
			noScale,
			glm::vec3(0.0f, 0.0f, lenFinger),
			false);

		Node leftLowerFingerDraw(&modelToCameraStack, 
			noRotation,
			glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f),
			glm::vec3(0.0f, 0.0f, lenFinger / 2.0f),
			true);
		
		Node rightUpperFingerDraw(&modelToCameraStack, 
			noRotation,
			glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f),
			glm::vec3(0.0f, 0.0f, lenFinger / 2.0f),
			true);
		
		Node rightLowerFingerNode(&modelToCameraStack, 
			glm::vec3(0.0f, angLowerFinger, 0.0f),
			noScale,
			glm::vec3(0.0f, 0.0f, lenFinger),
			false);
		
		Node rightLowerFingerDraw(&modelToCameraStack, 
			noRotation,
			glm::vec3(widthFinger / 2.0f, widthFinger/ 2.0f, lenFinger / 2.0f),
			glm::vec3(0.0f, 0.0f, lenFinger / 2.0f),
			true);
		
		baseNode.attachChild(&leftBase);
		baseNode.attachChild(&rightBase);
		baseNode.attachChild(&upperArmNode);

		upperArmNode.attachChild(&upperArmDraw);
		upperArmNode.attachChild(&lowerArmNode);

		lowerArmNode.attachChild(&lowerArmDraw);
		lowerArmNode.attachChild(&wristNode);

		wristNode.attachChild(&wristDraw);
		wristNode.attachChild(&leftFingerNode);
		wristNode.attachChild(&rightFingerNode);

		leftFingerNode.attachChild(&leftUpperFingerDraw);
		leftFingerNode.attachChild(&leftLowerFingerNode);
		
		leftLowerFingerNode.attachChild(&leftLowerFingerDraw);

		rightFingerNode.attachChild(&rightUpperFingerDraw);
		rightFingerNode.attachChild(&rightLowerFingerNode);

		rightLowerFingerNode.attachChild(&rightLowerFingerDraw);

		baseNode.renderNode();
	}

	#define STANDARD_ANGLE_INCREMENT 11.25f
	#define SMALL_ANGLE_INCREMENT 9.0f

	void AdjBase(bool bIncrement)
	{
		angBase += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angBase = fmodf(angBase, 360.0f);
	}

	void AdjUpperArm(bool bIncrement)
	{
		angUpperArm += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angUpperArm = Clamp(angUpperArm, -90.0f, 0.0f);
	}

	void AdjLowerArm(bool bIncrement)
	{
		angLowerArm += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angLowerArm = Clamp(angLowerArm, 0.0f, 146.25f);
	}

	void AdjWristPitch(bool bIncrement)
	{
		angWristPitch += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angWristPitch = Clamp(angWristPitch, 0.0f, 90.0f);
	}

	void AdjWristRoll(bool bIncrement)
	{
		angWristRoll += bIncrement ? STANDARD_ANGLE_INCREMENT : -STANDARD_ANGLE_INCREMENT;
		angWristRoll = fmodf(angWristRoll, 360.0f);
	}

	void AdjFingerOpen(bool bIncrement)
	{
		angFingerOpen += bIncrement ? SMALL_ANGLE_INCREMENT : -SMALL_ANGLE_INCREMENT;
		angFingerOpen = Clamp(angFingerOpen, 9.0f, 180.0f);
	}

	void WritePose()
	{
		printf("angBase:\t%f\n", angBase);
		printf("angUpperArm:\t%f\n", angUpperArm);
		printf("angLowerArm:\t%f\n", angLowerArm);
		printf("angWristPitch:\t%f\n", angWristPitch);
		printf("angWristRoll:\t%f\n", angWristRoll);
		printf("angFingerOpen:\t%f\n", angFingerOpen);
		printf("\n");
	}

private:
	glm::vec3		posBase;
	float			angBase;

	glm::vec3		posBaseLeft, posBaseRight;
	float			scaleBaseZ;

	float			angUpperArm;
	float			sizeUpperArm;

	glm::vec3		posLowerArm;
	float			angLowerArm;
	float			lenLowerArm;
	float			widthLowerArm;

	glm::vec3		posWrist;
	float			angWristRoll;
	float			angWristPitch;
	float			lenWrist;
	float			widthWrist;

	glm::vec3		posLeftFinger, posRightFinger;
	float			angFingerOpen;
	float			lenFinger;
	float			widthFinger;
	float			angLowerFinger;
};

SceneData scene;

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();
	InitializeVAO();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
}

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene.renderScene();

	glutSwapBuffers();
	glutPostRedisplay();
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape (int w, int h)
{
	cameraToClipMatrix[0].x = fFrustumScale * (h / (float)w);
	cameraToClipMatrix[1].y = fFrustumScale;

	glUseProgram(theProgram);
	glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
	glUseProgram(0);

	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		return;
	case 'a': scene.AdjBase(true); break;
	case 'd': scene.AdjBase(false); break;
	case 'w': scene.AdjUpperArm(false); break;
	case 's': scene.AdjUpperArm(true); break;
	case 'r': scene.AdjLowerArm(false); break;
	case 'f': scene.AdjLowerArm(true); break;
	case 't': scene.AdjWristPitch(false); break;
	case 'g': scene.AdjWristPitch(true); break;
	case 'z': scene.AdjWristRoll(true); break;
	case 'c': scene.AdjWristRoll(false); break;
	case 'q': scene.AdjFingerOpen(true); break;
	case 'e': scene.AdjFingerOpen(false); break;
	case 32: scene.WritePose(); break;
	}
}


unsigned int defaults(unsigned int displayMode, int &width, int &height) {return displayMode;}

