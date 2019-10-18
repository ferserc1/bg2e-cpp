$input a_position

$output v_color

#include <bgfx_shader.sh>

void main() {
    gl_Position = mul(u_modelViewProj, vec4(a_position,1.0));
    v_color = vec4(1.0,0.0,0.0,1.0);
}
