#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 vertexColor;
out vec4 locColor;

uniform float horizontalOffset;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	gl_Position += vec4(horizontalOffset, 0, 0, 0);
	vertexColor = aColor;
	locColor = gl_Position;
}