#version 330 core

uniform mat4 ScreenMatrix;
uniform mat4 WorldMatrix;

layout (location = 0) in vec3 position;
out vec2 TexCoord;

void main()
{
    gl_Position = ScreenMatrix * (WorldMatrix * (vec4(position.x, position.y, 1, 1)));
    TexCoord = (position.xy + 1) * 0.5;
};