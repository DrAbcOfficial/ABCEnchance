uniform sampler2D tex;
uniform float rad;
uniform vec3 xys;
uniform float gamma;
varying vec2 vet0;
void main() 
{
    vec4 vColor = texture2D(tex, gl_TexCoord[0].xy);
    if(rad > 0)
        vColor.a = 1.0 - step(1, length(2.0*((vet0 - xys.xy) / xys.z - vec2(0.5, 0.5))));
    if(gamma > 0){
        vColor.r = pow(vColor.r, gamma);
        vColor.g = pow(vColor.g, gamma);
        vColor.b = pow(vColor.b, gamma);
    }
    gl_FragColor = vColor;
}