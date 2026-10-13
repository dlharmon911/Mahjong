#version 330 core

in float v_meta;

void main()
{   
    if (v_meta < 0)
    {
        discard;
    }

    float red = clamp(v_meta / 255.0, 0.0, 1.0);

    gl_FragColor = vec4(red, 0.0, 0.0, 1.0);
}
