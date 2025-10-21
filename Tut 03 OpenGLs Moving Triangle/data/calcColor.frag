#version 330

out vec4 outputColor;

uniform float fragLoopDuration;
uniform float time;

const vec4 firstColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
const vec4 secondColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
const float PI = radians(180.0);
const float TWO_PI = 2*PI;

void main()
{
	// float currTime = mod(time, fragLoopDuration);
	// float currLerp = currTime / fragLoopDuration;
	
	// if(currTime <= 5){
	// 	outputColor = mix(firstColor,secondColor,currLerp);
	// }
	// else if(currTime > 5){
	// 	outputColor = mix(secondColor,firstColor,currLerp);
	// }
	
	//without using explicit conditional statements 
	float currTime = mod(time, fragLoopDuration); // [0-10)
	float theta = currTime * (TWO_PI/fragLoopDuration);
	float currLerp = cos(theta) * 0.5 + 0.5;

	outputColor = mix(secondColor,firstColor,currLerp);

}	
