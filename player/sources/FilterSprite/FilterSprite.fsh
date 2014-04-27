#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D u_texture;
varying vec2 v_texCoord;

uniform float keep_a;

void main(void)
{
    vec4 color = texture2D(u_texture, v_texCoord);
    
    if (color.a < keep_a) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    } else {
        gl_FragColor = color;
    }
}