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

void main()
{   
    int id = int(v_meta);

    if (id < 0)
    {
        discard;
    }


    // ambient
    vec3 ambient = u_light.ambient * u_material.ambient;
    
    // diffuse 
    vec3 normal_vector = normalize(v_normal);
    vec3 light_vector = u_light.position - v_frag_position;
    vec3 light_direction = normalize(light_vector);
    float diffuse_intensity = max(dot(normal_vector, light_direction), 0.0);
    vec3 diffuse = u_light.diffuse * diffuse_intensity * u_material.diffuse;
    
    // specular
    vec3 view_vector = u_camera.position - v_frag_position;
    vec3 view_direction = normalize(view_vector);    
    vec3 reflect_direction = reflect(-light_direction, normal_vector);    
    float specular_intensity = pow(max(dot(view_direction, reflect_direction), 0.0), u_material.shininess);
    vec3 specular = u_light.specular * specular_intensity * u_material.specular;
    
    vec4 tex_color = texture(al_tex, v_texcoord);
    vec3 result = tex_color.rgb;
    
    if (v_texcoord.x < 0.0 || v_texcoord.y < 0.0 || tex_color.a < 0.1)
    {
        result = (ambient + diffuse + specular) * v_color.rgb;
    }

    gl_FragColor = vec4(result, v_color.a);
} 

