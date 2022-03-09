#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 enemyColor;
uniform float time;
uniform int isExploding;

out vec3 color;

float rand(vec3 position) {
    return fract((position.x + position.y + position.z) * 1234);
}

void main()
{
    color = enemyColor;
    vec3 pos = v_position;
    float randomNumber = rand(v_position);

    if (isExploding == 1)
       pos += randomNumber * (v_normal + 0.01) * abs(cos(time * (pos.x + pos.y + pos.z)));

    gl_Position = Projection * View * Model * vec4(pos, 1.0);
}