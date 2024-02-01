#include "glew.h"
#include "gl_shader.h"
#include "gl_utility.h"


#include "gl_def.h"

SHADER_DEFINE(pp_texround)
SHADER_DEFINE(pp_kawaseblur_up)
SHADER_DEFINE(pp_kawaseblur_down)
SHADER_DEFINE(pp_colorlize)

void GL_ShaderInit() {
	pp_texround.program = R_CompileShaderFile("abcenchance\\shader\\pp_fullscreen.vsh", "abcenchance\\shader\\radar.fsh", nullptr);
	if (pp_texround.program) {
		SHADER_UNIFORM(pp_texround, rad, "rad");
		SHADER_UNIFORM(pp_texround, xys, "xys");
		SHADER_UNIFORM(pp_texround, gamma, "gamma");
	}
	pp_kawaseblur_up.program = R_CompileShaderFile("abcenchance\\shader\\dual_kawase.vsh", "abcenchance\\shader\\dual_kawase_up.fsh", nullptr);
	if (pp_kawaseblur_up.program) {
		SHADER_UNIFORM(pp_kawaseblur_up, iResolution, "iResolution");
		SHADER_UNIFORM(pp_kawaseblur_up, offset, "offset");
		SHADER_UNIFORM(pp_kawaseblur_up, halfpixel, "halfpixel");
	}
	pp_kawaseblur_down.program = R_CompileShaderFile("abcenchance\\shader\\dual_kawase.vsh", "abcenchance\\shader\\dual_kawase_down.fsh", nullptr);
	if (pp_kawaseblur_down.program) {
		SHADER_UNIFORM(pp_kawaseblur_down, iResolution, "iResolution");
		SHADER_UNIFORM(pp_kawaseblur_down, offset, "offset");
		SHADER_UNIFORM(pp_kawaseblur_down, halfpixel, "halfpixel");
	}
	pp_colorlize.program = R_CompileShaderFile("abcenchance\\shader\\pp_fullscreen.vsh", "abcenchance\\shader\\colorlize.fsh", nullptr);
	if (pp_colorlize.program)
		SHADER_UNIFORM(pp_colorlize, ha, "ha");
}