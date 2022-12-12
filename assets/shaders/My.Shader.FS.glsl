#version 330

// Input
in vec3 frag_color;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    // Calculate the final color
    out_color = frag_color;
}
