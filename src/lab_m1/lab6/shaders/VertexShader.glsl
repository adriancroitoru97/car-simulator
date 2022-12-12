#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float elapsedTime;

// Output
// TODO(student): Output values to fragment shader
out vec3 frag_color;

void main()
{
    // TODO(student): Send output to fragment shader
    //frag_color = v_color;
    frag_color = vec3(sin(elapsedTime) / 2 + 0.5f);
    frag_color.x += cos(elapsedTime) / 2 + 0.7f;
    frag_color.y += cos(elapsedTime) / 2;

    // TODO(student): Compute gl_Position
    vec3 temp = v_position;
    temp.x = temp.x + sin(elapsedTime) / 2 + 0.5;
    gl_Position = Projection * View * Model * vec4(temp, 1.0);
}
