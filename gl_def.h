#pragma once

typedef struct pp_radarlight_program_s {
	GLuint program;
	GLuint rad;
	GLuint xys;
	GLuint gamma;
}pp_radarlight_program_t;

typedef struct pp_gaussianblurh_program_s {
	GLuint program;
	GLuint du;
}pp_gaussianblurh_program_t;
typedef struct pp_gaussianblurv_program_s {
	GLuint program;
	GLuint du;
}pp_gaussianblurv_program_t;