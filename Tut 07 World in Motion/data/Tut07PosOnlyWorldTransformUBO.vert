#version 330

layout(location = 0) in vec4 position;

layout(std140) uniform GlobalMatrices
{
	mat4 worldToClipMatrix;
	mat4 worldToCameraMatrix;
};

uniform mat4 modelToWorldMatrix;

void main()
{
	vec4 temp = modelToWorldMatrix * position;
	gl_Position = worldToClipMatrix * temp;
}
