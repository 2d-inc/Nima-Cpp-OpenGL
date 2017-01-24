#ifdef GL_ES 
precision highp float;
#endif

uniform vec4 Color;
uniform float Opacity;
uniform sampler2D TextureSampler;
varying vec2 TexCoord;

void main(void)
{
	vec4 color = texture2D(TextureSampler, TexCoord) * Color;
	color *= Opacity;
	gl_FragColor = color;
}