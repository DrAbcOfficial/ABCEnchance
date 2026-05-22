#include "gl_def.h"

#include <IMetaRenderer.h>

pp_colorlize_program_t pp_colorlize;

void GL_ShaderInit()
{
	if (MetaRenderer())
	{
		pp_colorlize.program = MetaRenderer()->CompileShaderFile("abcenchance\\shader\\pp_fullscreen.vsh", "abcenchance\\shader\\colorlize.fsh");
	}
}

void GL_FreeShaders()
{

}