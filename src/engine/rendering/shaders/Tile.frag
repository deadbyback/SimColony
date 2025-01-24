#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float IsHighlighted;
in vec4 HighlightColor;

uniform sampler2D texture1;

void main()
{
    vec4 texColor = texture(texture1, TexCoord);
    if (IsHighlighted > 0.5) {
        // Смешиваем текстуру с цветом подсветки
        FragColor = mix(texColor, HighlightColor, HighlightColor.a);
    } else {
        FragColor = texColor;
    }
}