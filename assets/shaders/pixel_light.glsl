#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Camera
{
	vec3 position;
	vec3 look_at;
	vec3 up;
};

in vec4 v_color;
in vec3 v_normal;
in vec3 v_frag_position;
in vec2 v_texcoord;
in float v_meta;

uniform Material u_material;
uniform Light u_light;
uniform Camera u_camera;
uniform sampler2D al_tex;
uniform sampler2D u_shadow_map;
uniform mat4 u_light_view;
uniform mat4 u_light_projection;

out vec4 frag_color;

float unpack_depth(vec4 enc)
{
	return enc.r;
}

float shadow_calculation(vec3 world_pos, vec3 normal, vec3 light_dir)
{
	float n_dot_l = clamp(dot(normal, light_dir), 0.0, 1.0);
	vec3 offset_pos = world_pos + normal * (1.0 - n_dot_l) * 0.03;
	vec4 pos_ls = u_light_projection * u_light_view * vec4(offset_pos, 1.0);
	vec3 proj = pos_ls.xyz / pos_ls.w;

	// NDC [-1,1] -> [0,1]; matches gl_FragCoord.z written in the depth pass
	proj = proj * 0.5 + 0.5;

	if (proj.x < 0.0 || proj.x > 1.0 || proj.y < 0.0 || proj.y > 1.0 || proj.z > 1.0)
	{
		return 0.0;
	}

	float tan_theta = sqrt(max(1.0 - n_dot_l * n_dot_l, 0.0)) / max(n_dot_l, 0.1);
	float bias = 0.0015 + 0.0007 * min(tan_theta, 3.0);
	vec2 texel = 1.0 / vec2(textureSize(u_shadow_map, 0));

	float shadow = 0.0;
	for (int x = -2; x <= 2; ++x)
	{
		for (int y = -2; y <= 2; ++y)
		{
			float stored_depth = unpack_depth(texture(u_shadow_map, proj.xy + vec2(x, y) * texel));
			shadow += (proj.z - bias > stored_depth) ? 1.0 : 0.0;
		}
	}

	return shadow / 25.0;
}

void main()
{
    if (v_meta < 0)
    {
        discard;
    }

	vec3 ambient = u_light.ambient * u_material.ambient;

	vec3 normal_vector = normalize(v_normal);

	vec3 light_direction = normalize(u_light.position - v_frag_position);
	float diffuse_intensity = max(dot(normal_vector, light_direction), 0.0);
	vec3 diffuse = u_light.diffuse * diffuse_intensity * u_material.diffuse;

	vec3 view_direction = normalize(u_camera.position - v_frag_position);
	vec3 reflect_direction = reflect(-light_direction, normal_vector);

	float specular_intensity = pow(max(dot(view_direction, reflect_direction), 0.0), u_material.shininess);
	vec3 specular = u_light.specular * specular_intensity * u_material.specular;

	float shadow = shadow_calculation(v_frag_position, normal_vector, light_direction);

    vec4 tex_color = texture(al_tex, v_texcoord);
    vec3 result = tex_color.rgb;

    if (v_texcoord.x < 0.0 || v_texcoord.y < 0.0 || tex_color.a < 0.1)
    {
        result = v_color.rgb;
    }

	result = (ambient + (1.0 - shadow) * (diffuse + specular)) * result;

	frag_color = vec4(result, v_color.a);
}
