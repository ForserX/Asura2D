#version 330 core

uniform mat4 ScreenMatrix;
uniform mat4 WorldMatrix;

layout (location = 0) in vec3 position;
out vec2 TexCoord;

void main()
{
    gl_Position = ScreenMatrix* (WorldMatrix * vec4(position.x, position.y, position.z, 1.0));
    TexCoord = (vec2(position.x, position.y)+vec2(1,1))*0.5f;
};