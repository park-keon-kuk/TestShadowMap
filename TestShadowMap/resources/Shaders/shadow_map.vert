#version 430 core

layout(location = 0) in vec3 vertex;
// layout(location = 1) in vec3 normal;
// layout(location = 2) in vec2 texCoord;

layout(location = 0) uniform pmat;
layout(location = 1) uniform vmat;
layout(location = 2) uniform mmat;

void main()
{
	gl_Position = pmat * vmat * mmat * vec4(vertex, 1.f);
}