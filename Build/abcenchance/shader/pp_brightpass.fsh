#version 430 compatibility
uniform sampler2D tex;
void main() 
{
	vec4 vColor = texture2D(tex, gl_TexCoord[0].xy);
    if(vColor.r == 0.0 && vColor.g == 1.0 && vColor.b == 0.0) discard;
    vColor.r = pow(vColor.r, 0.45);
    vColor.g = pow(vColor.g, 0.45);
    vColor.b = pow(vColor.b, 0.45);
	gl_FragColor = max(vColor, vec4(0.0, 0.0, 0.0, 1.0));
}