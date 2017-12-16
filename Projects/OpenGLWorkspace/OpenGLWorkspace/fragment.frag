#version 330 core
out vec4 FragColor;
in vec3 vertexColor;
in vec4 locColor;

void main()
{
	
	FragColor = locColor;

}