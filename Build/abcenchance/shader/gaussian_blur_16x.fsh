uniform float du;
uniform vec2 res;
uniform sampler2D tex;
varying vec2 vet0;

float gaussian(float x, float sigma) {
    return exp(-0.5 * x * x / (sigma * sigma)) / (2.0 * 3.1415926 * sigma * sigma);
}

void main() {
    vec2 uv = gl_FragCoord.xy / res;
    vec4 color = vec4(0.0);
    float weightSum = 0.0;
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            vec2 offset = vec2(i, j) * du;
            float weight = gaussian(length(offset), du);
            color += texture2D(tex, uv + offset / res) * weight;
            weightSum += weight;
        }
    }
    color /= weightSum;
    gl_FragColor = color;
}