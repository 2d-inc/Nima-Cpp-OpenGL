attribute vec2 VertexPosition;
attribute vec2 VertexTexCoord;
uniform mat4 ProjectionMatrix;
uniform mat4 WorldMatrix;
uniform mat4 ViewMatrix;
varying vec2 TexCoord;

void main(void) 
{
	TexCoord = VertexTexCoord;
	vec4 pos = ViewMatrix * WorldMatrix * vec4(VertexPosition.x, VertexPosition.y, 0.0, 1.0);
	gl_Position = ProjectionMatrix * vec4(pos.xyz, 1.0);
}