#version 330 core

in vec2 vUV;
in vec4 vColor;
flat in int vTextureID;

uniform sampler2DArray uTexture;

out vec4 FragColor;

void main() {
  FragColor = vTextureID >= 0 ? texture(uTexture, vec3(vUV, float(vTextureID))) * vColor : vColor;
}
