#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projView;

uniform vec2 texcoordoffset;
uniform vec2 textframecount;

void main()
{
  gl_Position = projView * model * vec4(aPos.xyz, 1);
  TexCoords   = aTexCoords / textframecount + texcoordoffset;
}
