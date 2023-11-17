varying vec2 vet0;
void main() 
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
    vet0 = gl_Vertex.xy;
}