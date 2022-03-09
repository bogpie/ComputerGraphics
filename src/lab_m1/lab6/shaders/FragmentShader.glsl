#version 330

// Input
// TODO(student): Get values from vertex shader
in vec3 frag_position;
in vec3 frag_normal;
in vec3 frag_coord;
in vec3 frag_color;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    // DONE(student): Write pixel out color
    out_color = vec4(frag_color, 1);

}
