#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in int aTextureID;
layout(location = 4) in vec3 aColor;

uniform vec3 uScale;
uniform vec3 uModelTransform;
uniform vec3 uTransform;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vNormal;
out vec2 vUV;
out vec4 vColor;
flat out int vTextureID;

void main() {
    vec4 worldPos = vec4(aPosition, 1.0);
    worldPos *= vec4(uScale, 1.0);
    worldPos += vec4(uTransform, 0.0);
    worldPos += vec4(uModelTransform, 0.0);

    gl_Position = uProjection * uView * worldPos;
    vColor = vec4(aColor, 1.0); // FIXME: support transparency
    vTextureID = aTextureID;
    vUV = aUV;
}
