#version 330 core

out vec4 color;
in vec2 TexCoord;

uniform sampler2D InTexture;

void main()
{
	vec2 flipped = vec2(1 - TexCoord.x, TexCoord.y);
	color = texture(InTexture, flipped);
};