#version 330

// Input
// DONE(student): Get vertex attributes from each location
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec3 vertex_coord;
layout(location = 3) in vec3 vertex_color;
////

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output
// DONE(student): Output values to fragment shader
out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_coord;
out vec3 frag_color;

void main()
{
    // DONE(student): Send output to fragment shader
    frag_position = vertex_position;
    frag_normal = vertex_normal;
    frag_coord = vertex_normal;
    frag_color = vertex_color;

    // DONE(student): Compute gl_Position
	gl_Position = Projection * View * Model * vec4(vertex_position, 1.0);
}
