#version 330

smooth in vec4 theColor;

out vec4 outputColor;

void main()
{
	float lerpValue = gl_FragCoord.y / 500.0f;
	vec4 fragMixColour = mix(vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(0.2f, 0.2f, 0.2f, 1.0f), lerpValue);
	outputColor = theColor * fragMixColour;
}
