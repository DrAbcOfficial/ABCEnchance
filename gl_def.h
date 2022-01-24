#pragma once
typedef struct pp_texround_program_s {
	GLuint program;
	GLuint rad;
	GLuint xys;
	GLuint gamma;
}pp_texround_program_t;
typedef struct{
	GLuint program;
	GLfloat du;
}pp_gaussianblurv_program_t, pp_gaussianblurh_program_t;
typedef struct {
	GLuint program;
	GLuint ha;
}pp_colorlize_program_t;

extern pp_texround_program_t pp_texround;
extern pp_gaussianblurh_program_t pp_gaussianblurh;
extern pp_gaussianblurv_program_t pp_gaussianblurv;
extern pp_colorlize_program_t pp_colorlize;

void GL_ShaderInit();