#include "glew.h"
#include "gl_shader.h"
#include "gl_utility.h"
#include "gl_def.h"

#include <IMetaRenderer.h>

SHADER_DEFINE(pp_texround)
SHADER_DEFINE(pp_kawaseblur_up)
SHADER_DEFINE(pp_kawaseblur_down)
SHADER_DEFINE(pp_colorlize)

void GL_ShaderInit()
{
	if (MetaRenderer())
	{
		pp_texround.program = MetaRenderer()->CompileShaderFile("abcenchance\\shader\\pp_fullscreen.vsh", "abcenchance\\shader\\radar.fsh");
		if (pp_texround.program) {
			SHADER_UNIFORM(pp_texround, rad, "rad");
			SHADER_UNIFORM(pp_texround, xys, "xys");
			SHADER_UNIFORM(pp_texround, gamma, "gamma");
		}
		/*
			For blur RT
		*/
		pp_kawaseblur_up.program = MetaRenderer()->CompileShaderFile("abcenchance\\shader\\dual_kawase.vsh", "abcenchance\\shader\\dual_kawase_up.fsh");
		if (pp_kawaseblur_up.program) {
			SHADER_UNIFORM(pp_kawaseblur_up, iResolution, "iResolution");
			SHADER_UNIFORM(pp_kawaseblur_up, offset, "offset");
			SHADER_UNIFORM(pp_kawaseblur_up, halfpixel, "halfpixel");
		}
		pp_kawaseblur_down.program = MetaRenderer()->CompileShaderFile("abcenchance\\shader\\dual_kawase.vsh", "abcenchance\\shader\\dual_kawase_down.fsh");
		if (pp_kawaseblur_down.program) {
			SHADER_UNIFORM(pp_kawaseblur_down, iResolution, "iResolution");
			SHADER_UNIFORM(pp_kawaseblur_down, offset, "offset");
			SHADER_UNIFORM(pp_kawaseblur_down, halfpixel, "halfpixel");
		}
		pp_colorlize.program = MetaRenderer()->CompileShaderFile("abcenchance\\shader\\pp_fullscreen.vsh", "abcenchance\\shader\\colorlize.fsh");
		if (pp_colorlize.program) {
			SHADER_UNIFORM(pp_colorlize, ha, "ha");
		}
	}
	else
	{
		pp_texround.program = GL_CompileShaderFile("abcenchance\\shader\\pp_fullscreen.vsh", "abcenchance\\shader\\radar.fsh");
		if (pp_texround.program) {
			SHADER_UNIFORM(pp_texround, rad, "rad");
			SHADER_UNIFORM(pp_texround, xys, "xys");
			SHADER_UNIFORM(pp_texround, gamma, "gamma");
		}

		pp_kawaseblur_up.program = GL_CompileShaderFile("abcenchance\\shader\\dual_kawase.vsh", "abcenchance\\shader\\dual_kawase_up.fsh");
		if (pp_kawaseblur_up.program) {
			SHADER_UNIFORM(pp_kawaseblur_up, iResolution, "iResolution");
			SHADER_UNIFORM(pp_kawaseblur_up, offset, "offset");
			SHADER_UNIFORM(pp_kawaseblur_up, halfpixel, "halfpixel");
		}

		pp_kawaseblur_down.program = GL_CompileShaderFile("abcenchance\\shader\\dual_kawase.vsh", "abcenchance\\shader\\dual_kawase_down.fsh");
		if (pp_kawaseblur_down.program) {
			SHADER_UNIFORM(pp_kawaseblur_down, iResolution, "iResolution");
			SHADER_UNIFORM(pp_kawaseblur_down, offset, "offset");
			SHADER_UNIFORM(pp_kawaseblur_down, halfpixel, "halfpixel");
		}

		pp_colorlize.program = GL_CompileShaderFile("abcenchance\\shader\\pp_fullscreen.vsh", "abcenchance\\shader\\colorlize.fsh");
		if (pp_colorlize.program) {
			SHADER_UNIFORM(pp_colorlize, ha, "ha");
		}
	}
}