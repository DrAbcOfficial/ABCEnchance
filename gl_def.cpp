#include "glew.h"
#include "gl_shader.h"
#include "gl_utility.h"


#include "gl_def.h"

SHADER_DEFINE(pp_texround)
SHADER_DEFINE(pp_gaussianblur)
SHADER_DEFINE(pp_colorlize)

void GL_ShaderInit() {
	pp_texround.program = R_CompileShaderFile("abcenchance\\shader\\pp_fullscreen.vsh", "abcenchance\\shader\\radar.fsh", nullptr);
	if (pp_texround.program) {
		SHADER_UNIFORM(pp_texround, rad, "rad");
		SHADER_UNIFORM(pp_texround, xys, "xys");
		SHADER_UNIFORM(pp_texround, gamma, "gamma");
	}
	pp_gaussianblur.program = R_CompileShaderFile("abcenchance\\shader\\pp_fullscreen.vsh", "abcenchance\\shader\\gaussian_blur_16x.fsh", nullptr);
	if (pp_gaussianblur.program) {
		SHADER_UNIFORM(pp_gaussianblur, du, "du");
		SHADER_UNIFORM(pp_gaussianblur, res, "res");
	}
	pp_colorlize.program = R_CompileShaderFile("abcenchance\\shader\\pp_fullscreen.vsh", "abcenchance\\shader\\colorlize.fsh", nullptr);
	if (pp_colorlize.program)
		SHADER_UNIFORM(pp_colorlize, ha, "ha");
}