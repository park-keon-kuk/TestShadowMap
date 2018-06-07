#version 430 core

in VOUT {
	vec3 normal;
	vec4 light_space_position;
}v;

layout(binding = 0) uniform sampler2D shadow_map;

layout(location = 0) out vec4 frag_color;

void main()
{
	frag_color = vec4(v.normal, 1.f);

	vec3 proj_coord = v.light_space_position.xyz / v.light_space_position.w;
	proj_coord = proj_coord * 0.5f + 0.5f;

	float nearest = texture(shadow_map, proj_coord.st).r;
	
	if(proj_coord.z > nearest) {
		frag_color = vec4(0.f, 0.f, 0.f, 1.f);
	}
}