#version 460 core
layout(location = 0) in vec3 pos;

layout(location = 2) in vec4 inWorldCol0;
layout(location = 3) in vec4 inWorldCol1;
layout(location = 4) in vec4 inWorldCol2;
layout(location = 5) in vec4 inWorldCol3;
layout(location = 6) in vec4 color;

layout(location = 0) out vec4 vColor;

layout(std140, binding = 0) uniform mainCbuffer {
    mat4 view;
    mat4 projection;
};

void main() {
    vColor = color;
    mat4 world = mat4(inWorldCol0,
                      inWorldCol1,
                      inWorldCol2,
                      inWorldCol3);
    gl_Position = projection * view * world * vec4(pos, 1.0);
}
