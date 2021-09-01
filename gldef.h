#pragma once
#define glBind(tx) gHookFuncs.GL_Bind(tx)

typedef struct pp_radarlight_program_s {
	GLuint program;
	GLuint tex;
	GLuint ovc;
}pp_radarlight_program_t;