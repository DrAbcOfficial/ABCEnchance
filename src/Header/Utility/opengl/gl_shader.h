#pragma once

#include "gl_common.h"
#include <vector>

#define SHADER_DEFINE(name) name##_program_t name;
#define SHADER_UNIFORM(name, loc, locstring) name.loc = glGetUniformLocation(name.program, locstring);
#define SHADER_ATTRIB(name, loc, locstring) name.loc = glGetAttribLocation(name.program, locstring);
