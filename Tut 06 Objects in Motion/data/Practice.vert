#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

smooth out vec4 theColor;

uniform mat4 cameraToClipMatrix;
uniform mat4 modelToCameraMatrix;

void main()
{
    //calculates position in camera space
    vec4 cameraPos = modelToCameraMatrix * position;
    //calculates position in clip space and gets sent to frag shader
    gl_Position = cameraToClipMatrix * cameraPos;
    theColor = color;
}