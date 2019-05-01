#version 330

uniform mat4 model;
uniform mat4 projView;

layout(location = 0) in vec2 vPos;

uniform vec4 debugColor;

out vec4 f_color;

void main(void)
{
  f_color = vec4(debugColor.rgb, 1.0);
  gl_Position = projView * model * vec4(vPos, 0.0f, 1.0f);
}