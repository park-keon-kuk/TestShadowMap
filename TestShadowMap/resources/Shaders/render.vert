#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

layout(location = 0) uniform mat4 pmat;
layout(location = 1) uniform mat4 vmat;
layout(location = 2) uniform mat4 mmat;
layout(location = 3) uniform mat4 light_pmat;
layout(location = 4) uniform mat4 light_vmat;


out VOUT {
	vec3 normal;
	vec4 light_space_position;
}v;

void main()
{
	vec4 world_pos = mmat * vec4(vertex, 1.f);

	gl_Position = pmat * vmat * world_pos;

	v.normal = normal * 0.5f + 0.5f;

	v.light_space_position = light_pmat * light_vmat * world_pos;
}