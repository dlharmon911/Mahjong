#version 330 core

in vec4 al_pos;
in vec4 al_color;
in vec3 al_user_attr_0;
in float al_user_attr_1;
in vec2 al_texcoord;

uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;

out vec4 v_color;
out vec3 v_normal;
out vec3 v_frag_position;
out vec2 v_texcoord;
out float v_meta;

void main()
{
    v_color = al_color;
    v_normal = mat3(transpose(inverse(u_model_matrix))) * al_user_attr_0;
    v_frag_position = vec3(u_model_matrix * vec4(al_pos.xyz, 1.0));
	v_texcoord = al_texcoord;
    v_meta = al_user_attr_1;

   gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * al_pos;
}
