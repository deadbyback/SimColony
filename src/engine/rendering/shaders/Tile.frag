#version 450 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec4 highlightColor;
uniform float isHighlighted;

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    FragColor = mix(texColor, highlightColor, isHighlighted * highlightColor.a);
}
