#version 330 core

out vec4 frag_color;

in float v_meta;

void main()
{
    if (v_meta < 0)
    {
        discard;
    }

	frag_color = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
}
