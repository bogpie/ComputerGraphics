#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// DONE(student): Declare any other uniforms
uniform vec3 object_color;
uniform int isSpotlight;
uniform float angle;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // DONE(student) - Calculate needed vectors
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
   
    // DONE(student): Compute specular light component
    float specular_light = 0;
    if (diffuse_light > 0)
    {
    		specular_light = material_ks * pow(max(dot(V, R), 0), material_shininess);

    }
    // DONE(student): If (and only if) the light is a spotlight, we need to do
    // some additional things.

    // DONE(student): Compute the total light. You can just add the components
    // together, but if you're feeling extra fancy, you can add individual
    // colors to the light components. To do that, pick some vec3 colors that
    // you like, and multiply them with the respective light components.

    vec3 light;
    float cut_off = angle;
    float spot_light = dot(-L, light_direction);
    float spot_light_limit = cos(cut_off);

    // Quadratic attentuation
	float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
	float light_att_factor = pow(linear_att, 2);
    
	float defaultAttenuationFactor = 1 / pow(distance(light_position, world_position), 2);

    if(isSpotlight == 1)
    {
    	if (spot_light > cos(cut_off))
		{
			// fragmentul este iluminat de spot, deci se calculeaza valoarea luminii conform  modelului Phong
			// se calculeaza atenuarea luminii	
            light =
                object_color
                * (ambient_light + (defaultAttenuationFactor * light_att_factor)
                * (diffuse_light + specular_light));
		} 
        else 
        {
            // fragmentul nu primeste lumina de tip spot, dar va recepta de la alte obiecte
			light = object_color * ambient_light;
		}
    }
    else 
    {
		light = object_color * (ambient_light + defaultAttenuationFactor * (diffuse_light + specular_light));
    }    
    // DONE(student): Write pixel out color
    out_color = vec4(light, 1);
}
