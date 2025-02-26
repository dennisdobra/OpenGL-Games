#version 330 core

// Inputs from the vertex shader
in vec3 v_texCoord;

// Outputs
out vec4 fragColor;

// 2D Random Function
float random(in vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// 2D Noise Function
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

// Fractal Noise Function
float fractalNoise(in vec2 st) {
    float value = 0.0;
    float amplitude = 0.5;  // Lower amplitude for smoother noise
    float frequency = 1.0;  // Lower frequency for larger patterns

    for (int i = 0; i < 4; i++) {
        value += amplitude * noise(st * frequency);
        frequency *= 1.5;  // Less aggressive scaling
        amplitude *= 0.6;  // Softer fade
    }

    return value;
}

void main() {
    // Normalize coordinates
    vec2 st = vec2(v_texCoord.x, v_texCoord.z);

    // Generate noise
    float n = fractalNoise(st * 0.4) * 1.2; // Larger scale for smoother and larger blobs

    vec3 taupe = vec3(0.54, 0.47, 0.39);
    vec3 lightBrown = vec3(0.76, 0.60, 0.42);

    // Smooth transition between the two colors
    vec3 finalColor = mix(taupe, lightBrown, smoothstep(0.4, 0.6, n));

    // Output the final fragment color
    fragColor = vec4(finalColor, 1.0);
}
