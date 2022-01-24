#include "glew.h"
#include "gl_shader.h"
#include "gl_utility.h"


#include "gl_def.h"

SHADER_DEFINE(pp_texround)
SHADER_DEFINE(pp_gaussianblurh)
SHADER_DEFINE(pp_gaussianblurv)
SHADER_DEFINE(pp_colorlize)

void GL_ShaderInit() {
	pp_texround.program = R_CompileShaderFile("abcenchance\\shader\\pp_brightpass.vsh", "abcenchance\\shader\\radar.fsh", nullptr);
	if (pp_texround.program) {
		SHADER_UNIFORM(pp_texround, rad, "rad");
		SHADER_UNIFORM(pp_texround, xys, "xys");
		SHADER_UNIFORM(pp_texround, gamma, "gamma");
	}
	pp_gaussianblurh.program = R_CompileShaderFileEx("abcenchance\\shader\\fullscreentriangle.vert.glsl", "abcenchance\\shader\\gaussian_blur_16x.frag.glsl", "", "#define BLUR_HORIZONAL\n", nullptr);
	if(pp_gaussianblurh.program)
		SHADER_UNIFORM(pp_gaussianblurh, du, "du");
	pp_gaussianblurv.program = R_CompileShaderFileEx("abcenchance\\shader\\fullscreentriangle.vert.glsl", "abcenchance\\shader\\gaussian_blur_16x.frag.glsl", "", "#define BLUR_VERTICAL\n", nullptr);
	if (pp_gaussianblurv.program)
		SHADER_UNIFORM(pp_gaussianblurv, du, "du");
	pp_colorlize.program = R_CompileShaderFile("abcenchance\\shader\\pp_brightpass.vsh", "abcenchance\\shader\\colorlize.fsh", nullptr);
	if (pp_colorlize.program)
		SHADER_UNIFORM(pp_colorlize, ha, "ha");
}