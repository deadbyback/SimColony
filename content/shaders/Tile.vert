#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec2 aInstancePos;
layout (location = 3) in vec2 aInstanceSize;
layout (location = 4) in float aIsHighlighted;
layout (location = 5) in vec4 aHighlightColor;

out vec2 TexCoord;
out float IsHighlighted;
out vec4 HighlightColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec2 pos = aPos * aInstanceSize + aInstancePos;
    gl_Position = projection * view * vec4(pos, 0.0, 1.0);
    TexCoord = aTexCoord;
    IsHighlighted = aIsHighlighted;
    HighlightColor = aHighlightColor;
}
