#version 430 compatibility
void main() 
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}