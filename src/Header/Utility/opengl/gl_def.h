#pragma once
typedef struct pp_texround_program_s {
	GLuint program;
	GLuint rad;
	GLuint xys;
	GLuint gamma;
}pp_texround_program_t;
typedef struct{
	GLuint program;
	GLuint iResolution;
	GLuint offset;
	GLuint halfpixel;
}pp_kawaseblur_program_t;
typedef pp_kawaseblur_program_t pp_kawaseblur_up_program_t;
typedef pp_kawaseblur_program_t pp_kawaseblur_down_program_t;
typedef struct {
	GLuint program;
	GLuint ha;
}pp_colorlize_program_t;

extern pp_texround_program_t pp_texround;
extern pp_kawaseblur_up_program_t pp_kawaseblur_up;
extern pp_kawaseblur_down_program_t pp_kawaseblur_down;
extern pp_colorlize_program_t pp_colorlize;

void GL_ShaderInit();