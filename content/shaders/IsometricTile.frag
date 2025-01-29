#version 450 core

// Входные данные из вершинного шейдера
in vec2 TexCoord;

// Выходной цвет фрагмента
out vec4 FragColor;

// Uniform-переменная для текстуры
uniform sampler2D tileTexture;
uniform bool isHighlighted;
uniform vec4 highlightColor = vec4(1.0, 1.0, 0.0, 0.3); // Желтый цвет с прозрачностью

void main() {
    vec4 texColor = texture(tileTexture, TexCoord);
    if (isHighlighted) {
        // Смешиваем текстуру с цветом подсветки
        FragColor = mix(texColor, highlightColor, highlightColor.a);
    } else {
        FragColor = texColor;
    }
}