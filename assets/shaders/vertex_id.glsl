#version 330 core

in vec4 al_pos;
in float al_user_attr_1;

uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;
out float v_meta;

void main()
{
	v_meta = al_user_attr_1;
	gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * al_pos;
}
