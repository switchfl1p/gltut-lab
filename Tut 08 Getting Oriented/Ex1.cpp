#include <string>
#include <vector>
#include <stack>
#include <math.h>
#include <stdio.h>
#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/Timer.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))

GLuint theProgram;
GLuint positionAttrib;
GLuint colorAttrib;

GLuint modelToCameraMatrixUnif;
GLuint cameraToClipMatrixUnif;
GLuint baseColorUnif;

glm::mat4 cameraToClipMatrix(0.0f);

float CalcFrustumScale(float fFovDeg)
{
	const float degToRad = 3.14159f * 2.0f / 360.0f;
	float fFovRad = fFovDeg * degToRad;
	return 1.0f / tan(fFovRad / 2.0f);
}

const float fFrustumScale = CalcFrustumScale(20.0f);

void InitializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "Tut08PosColorLocalTransform.vert"));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "Tut08ColorMultUniform.frag"));

	theProgram = Framework::CreateProgram(shaderList);

	positionAttrib = glGetAttribLocation(theProgram, "position");
	colorAttrib = glGetAttribLocation(theProgram, "color");

	modelToCameraMatrixUnif = glGetUniformLocation(theProgram, "modelToCameraMatrix");
	cameraToClipMatrixUnif = glGetUniformLocation(theProgram, "cameraToClipMatrix");
	baseColorUnif = glGetUniformLocation(theProgram, "baseColor");

	float fzNear = 1.0f; float fzFar = 600.0f;

	cameraToClipMatrix[0].x = fFrustumScale;
	cameraToClipMatrix[1].y = fFrustumScale;
	cameraToClipMatrix[2].z = (fzFar + fzNear) / (fzNear - fzFar);
	cameraToClipMatrix[2].w = -1.0f;
	cameraToClipMatrix[3].z = (2 * fzFar * fzNear) / (fzNear - fzFar);

	glUseProgram(theProgram);
	glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
	glUseProgram(0);
}

Framework::Mesh *g_pShip = NULL;

static glm::fquat g_Orients[] =
{
	glm::fquat(0.7071f, 0.7071f, 0.0f, 0.0f),
	glm::fquat(0.5f, 0.5f, -0.5f, 0.5f),
	glm::fquat(-0.4895f, -0.7892f, -0.3700f, -0.02514f),
	glm::fquat(0.4895f, 0.7892f, 0.3700f, 0.02514f),

	glm::fquat(0.3840f, -0.1591f, -0.7991f, -0.4344f),
	glm::fquat(0.5537f, 0.5208f, 0.6483f, 0.0410f),
	glm::fquat(0.0f, 0.0f, 1.0f, 0.0f),
};

static char g_OrientKeys[] =
{
	'q',
	'w',
	'e',
	'r',

	't',
	'y',
	'u',
};

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();

	try
	{
		g_pShip = new Framework::Mesh("Ship.xml");
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
}

glm::vec4 Vectorize(const glm::fquat theQuat)
{
	glm::vec4 ret;

	ret.x = theQuat.x;
	ret.y = theQuat.y;
	ret.z = theQuat.z;
	ret.w = theQuat.w;

	return ret;
}

glm::fquat Lerp(const glm::fquat &v0, const glm::fquat &v1, float alpha, bool useShortPath = true)
{
    glm::fquat start = v0;
    glm::fquat end = v1;
    
    if (useShortPath) {
        // Choose the representation that gives the shortest path
        if (glm::dot(v0, v1) < 0.0f) {
            end = -v1;
        }
    }
    
    glm::vec4 startVec = Vectorize(start);
    glm::vec4 endVec = Vectorize(end);
    glm::vec4 interp = glm::mix(startVec, endVec, alpha);
    
    printf("alpha: %f, (%f, %f, %f, %f)\n", alpha, interp.w, interp.x, interp.y, interp.z);
    
    interp = glm::normalize(interp);
    return glm::fquat(interp.w, interp.x, interp.y, interp.z);
}

glm::fquat Slerp(const glm::fquat &v0, const glm::fquat &v1, float alpha, bool useShortPath = true)
{
    glm::fquat start = v0;
    glm::fquat end = v1;
    
    float dot = glm::dot(start, end);
    
    if (useShortPath && dot < 0.0f) {
        // Use the short path by negating one quaternion
        end = -end;
        dot = -dot;
    }
    
    const float DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD)
        return Lerp(start, end, alpha, useShortPath);
    
    dot = glm::clamp(dot, -1.0f, 1.0f);
    float theta_0 = acosf(dot);
    float theta = theta_0 * alpha;
    
    // Corrected line: use multiplication instead of addition
    glm::fquat relative = end * glm::inverse(start);
    relative = glm::normalize(relative);
    
    return start * glm::fquat(cos(theta), sin(theta) * relative.x, sin(theta) * relative.y, sin(theta) * relative.z);
}

class Orientation
{
public:
    Orientation()
        : m_ixCurrOrient(0)
        , m_bSlerp(false)
        , m_bUseShortPath(true)  // Default to short path
    {
    }

    bool ToggleSlerp()
    {
        m_bSlerp = !m_bSlerp;
        return m_bSlerp;
    }

    bool TogglePath()
    {
        m_bUseShortPath = !m_bUseShortPath;
        return m_bUseShortPath;
    }

    glm::fquat GetOrient() const
    {
        glm::fquat result = g_Orients[m_ixCurrOrient];
        for (const auto& anim : animDeque) {
            result = anim.GetOrient(result, m_bSlerp, m_bUseShortPath);
        }

        return result;
    }

    bool IsAnimating() const {return !animDeque.empty();}

    void UpdateTime()
    {
        // Update all animations and remove finished ones
        auto it = animDeque.begin();
        while (it != animDeque.end()) {
            bool bIsFinished = it->UpdateTime();
            if (bIsFinished) {
                // When an animation finishes, update the base orientation
                m_ixCurrOrient = it->GetFinalIx();
                it = animDeque.erase(it); // Remove finished animation
            } else {
                ++it;
            }
        }
    }

    void AnimateToOrient(int ixDestination)
    {
        if (animDeque.empty() && m_ixCurrOrient == ixDestination)
            return;

        Animation newAnim;
        newAnim.StartAnimation(ixDestination, 1.0f);
        animDeque.push_back(newAnim);
    }

    bool IsUsingShortPath() const { return m_bUseShortPath; }

private:
    class Animation
    {
    public:
        //Returns true if the animation is over.
        bool UpdateTime()
        {
            return m_currTimer.Update();
        }

        glm::fquat GetOrient(const glm::fquat &initial, bool bSlerp, bool useShortPath) const
        {
            if(bSlerp)
            {
                return Slerp(initial, g_Orients[m_ixFinalOrient], m_currTimer.GetAlpha(), useShortPath);
            }
            else
            {
                return Lerp(initial, g_Orients[m_ixFinalOrient], m_currTimer.GetAlpha(), useShortPath);
            }
        }

        void StartAnimation(int ixDestination, float fDuration)
        {
            m_ixFinalOrient = ixDestination;
            m_currTimer = Framework::Timer(Framework::Timer::TT_SINGLE, fDuration);
        }

        int GetFinalIx() const {return m_ixFinalOrient;}

    private:
        int m_ixFinalOrient;
        Framework::Timer m_currTimer;
    };

    int m_ixCurrOrient;
    bool m_bSlerp;
    bool m_bUseShortPath;  // New member to track path choice
    std::deque<Animation> animDeque;
};

Orientation g_orient;

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void display()
{
	g_orient.UpdateTime();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutil::MatrixStack currMatrix;
	currMatrix.Translate(glm::vec3(0.0f, 0.0f, -200.0f));
	currMatrix.ApplyMatrix(glm::mat4_cast(g_orient.GetOrient()));

	glUseProgram(theProgram);
	currMatrix.Scale(3.0, 3.0, 3.0);
	currMatrix.RotateX(-90);
	//Set the base color for this object.
	glUniform4f(baseColorUnif, 1.0, 1.0, 1.0, 1.0);
	glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(currMatrix.Top()));

	g_pShip->Render("tint");

	glUseProgram(0);

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

void ApplyOrientation(int iIndex)
{
	
	g_orient.AnimateToOrient(iIndex);
}


//Called whenever a key on the keyboard was pressed.
//The key is given by the ''key'' parameter, which is in ASCII.
//It's often a good idea to have the escape key (ASCII value 27) call glutLeaveMainLoop() to 
//exit the program.
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        glutLeaveMainLoop();
        return;
    case 32:  // Space bar - toggle interpolation type
        {
            bool bSlerp = g_orient.ToggleSlerp();
            printf(bSlerp ? "Slerp\n" : "Lerp\n");
        }
        break;
    case 'p':  // 'p' key - toggle between short and long path
    case 'P':
        {
            bool bShortPath = g_orient.TogglePath();
            printf(bShortPath ? "Short path\n" : "Long path\n");
        }
        break;
    }

    for(int iOrient = 0; iOrient < ARRAY_COUNT(g_OrientKeys); iOrient++)
    {
        if(key == g_OrientKeys[iOrient])
            ApplyOrientation(iOrient);
    }
}

unsigned int defaults(unsigned int displayMode, int &width, int &height) {return displayMode;}
