#pragma once

#include "gl_common.h"
#include <vector>

GLuint GL_CompileShaderFileEx(const CCompileShaderArgs* args);
GLuint GL_CompileShaderFile(const char* vsfile, const char* fsfile, const char* vsdefine = nullptr, const char* fsdefine = nullptr);
void GL_UseProgram(GLuint program);
void GL_FreeShaders();

#define SHADER_DEFINE(name) name##_program_t name;
#define SHADER_UNIFORM(name, loc, locstring) name.loc = glGetUniformLocation(name.program, locstring);
#define SHADER_ATTRIB(name, loc, locstring) name.loc = glGetAttribLocation(name.program, locstring);
