#version 330 core

layout (location = 0) in vec3 position;      // Vertex position
layout (location = 1) in vec3 normal;        // Vertex normal
layout (location = 2) in vec2 tex_coord;     // Texture coordinates
layout (location = 3) in vec3 color;         // Vertex color

// Uniforms
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Outputs to fragment shader
out vec3 v_texCoord;       // Output texture coordinates to FragmentShader
out float v_terrainHeight; // Output terrain height to FragmentShader

// 2D Noise Function
float random(in vec2 st) {
    return fract(sin(st.x * 2.0f) + cos(st.y * 4.0f));
}

float noise(in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main() {
    // Adjust terrain height using noise
    vec3 modifiedPosition = position;

    vec2 scaledPosition = modifiedPosition.xz * 0.07f;  // Adjust scale for smoother and larger hills
    float hillHeight = noise(scaledPosition) * 30.0f;   // Increased amplitude for taller hills
    modifiedPosition.y += hillHeight;

    // Pass texture coordinates and height to the fragment shader
    v_texCoord = modifiedPosition;
    v_terrainHeight = hillHeight;

    // Transform vertex position to clip space
    gl_Position = Projection * View * Model * vec4(modifiedPosition, 1.0);
}
