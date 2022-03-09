#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for light properties (in global space)
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;

// Output value to fragment shader
out vec3 color;


void main()
{
    // DONE(student): Compute world space vectors
    vec3 world_position = vec3(Model * vec4(v_position, 1));
    vec3 world_normal = vec3(Model * vec4(v_normal, 0));

    vec3 L = normalize(light_position - world_position);
    vec3 N = normalize(world_normal);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);
    vec3 R = normalize(reflect(-L, N));

    // DONE(student): Define ambient light component
    float global_ambient_light = 0.25;
    float ambient_light = material_kd * global_ambient_light;

    // DONE(student): Compute diffuse light component
    float diffuse_light  = material_kd * max (dot(N,L), 0);

    // DONE(student): Compute specular light 
    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Gouraud (1971) shading method. There is also the Phong (1975) shading
    // method, which we'll use in the future. Don't mix them up!    
    float specular_light = 0;
    if (diffuse_light > 0)
    {
    		specular_light = material_ks * pow(max(dot(V, R), 0), material_shininess);
    }

    // DONE(student): Compute light
    float kc = 0.1;
    float kl = 0.1;
    float kq = 0.1;
    float dist = distance(light_position, world_position);
    float attenuation_factor = 1 / (kc + kl * dist + kq * dist * dist);
    float light = ambient_light + attenuation_factor * ( diffuse_light + specular_light);

    // DONE(student): Send color light output to fragment shader
	color = object_color * light;

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
