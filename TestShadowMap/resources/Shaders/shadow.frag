#version 430 core

in VOUT {
	vec3 normal;
}v;

layout(location = 0) out vec4 frag_color;

void main()
{
	frag_color = vec4(v.normal, 1.f);
}