#include "glew.h"
#include "gl_shader.h"
#include "gl_utility.h"
#include "gl_def.h"

#include <IMetaRenderer.h>

SHADER_DEFINE(pp_colorlize)

void GL_ShaderInit()
{
	if (MetaRenderer())
	{
		pp_colorlize.program = MetaRenderer()->CompileShaderFile("abcenchance\\shader\\pp_fullscreen.vsh", "abcenchance\\shader\\colorlize.fsh");
		if (pp_colorlize.program) {
			SHADER_UNIFORM(pp_colorlize, ha, "ha");
		}
	}
}

void GL_FreeShaders()
{

}