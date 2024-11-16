#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 3) in vec3 v_normal;
layout(location = 2) in vec2 v_tex_coord;
layout(location = 1) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float time;

// Output
// TODO(student): Output values to fragment shader
out vec3 frag_color;
out vec3 frag_position;
//out vec3 frag_position;
//out vec3 frag_normal;
//out vec2 frag_tex_coord;

void main()
{
    // TODO(student): Compute gl_Position
    vec3 aux_pos, aux_color;
    aux_pos = v_position + cos(time / 2); //vec3(v_position.x * cos(time), v_position.y * sin(time / 2.f), v_position.z * cos(time / 2.f));
    aux_color = vec3(v_color.x * sin(time), v_color.y * sin(time / 2.f), v_color.z * cos(time / 2.f));

    // TODO(student): Send output to fragment shader
    frag_color = aux_color;
    frag_position = aux_pos;

    gl_Position = Projection * View * Model * vec4(frag_position, 1.0);
}
