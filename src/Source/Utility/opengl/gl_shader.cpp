#include <metahook.h>
#include <glew.h>
#include "gl_shader.h"

#include "plugins.h"
#include "exportfuncs.h"

#include <string>
#include <sstream>
#include <regex>
#include <ScopeExit/ScopeExit.h>
#include <IMetaRenderer.h>

class CCompileShaderContext
{
public:
	std::string vscode;
	std::string gscode;
	std::string fscode;
};

std::vector<GLuint> g_ShaderTable;

void GL_FreeShaders(){
	for (auto prog : g_ShaderTable){
		glDeleteProgram(prog);
	}
	g_ShaderTable.clear();
}

void GL_CheckShaderError(GLuint shader, const char* code, const char* filename)
{
	int iStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &iStatus);

	if (GL_FALSE == iStatus)
	{
		int nInfoLength = 0;
		glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &nInfoLength);

		std::string info;
		info.resize(nInfoLength);

		glGetInfoLogARB(shader, nInfoLength, NULL, (char*)info.c_str());

		SYS_ERROR("Shader %s compiled with error:\n%s", filename, info.c_str());
		return;
	}
}

GLuint R_CompileShaderObject(int type, const char* code, const char* filename)
{
	auto obj = glCreateShader(type);

	glShaderSource(obj, 1, &code, NULL);

	glCompileShader(obj);

	GL_CheckShaderError(obj, code, filename);

	return obj;
}

GLuint R_CompileShader(const CCompileShaderArgs* args, const CCompileShaderContext* context)
{
	GLuint shader_objects[32] = {};
	int shader_object_used = 0;

#define APPEND_SHADER_STAGE(stage, GLStage) if (context->stage##code.size() > 0)\
	{\
		shader_objects[shader_object_used] = R_CompileShaderObject(GLStage, context->stage##code.c_str(), args->stage##file);\
		shader_object_used++;\
	}

	APPEND_SHADER_STAGE(vs, GL_VERTEX_SHADER);
	APPEND_SHADER_STAGE(gs, GL_GEOMETRY_SHADER);
	APPEND_SHADER_STAGE(fs, GL_FRAGMENT_SHADER);

#undef APPEND_SHADER_STAGE

	GLuint program = glCreateProgram();
	for (int i = 0; i < shader_object_used; ++i)
		glAttachShader(program, shader_objects[i]);
	glLinkProgram(program);

	GLint iStatus = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &iStatus);

	if (GL_FALSE == iStatus)
	{
		int nInfoLength;
		char szCompilerLog[1024] = { 0 };
		glGetProgramInfoLog(program, sizeof(szCompilerLog), &nInfoLength, szCompilerLog);

		SYS_ERROR("Shader linked with error:\n%s\n", szCompilerLog);
	}

	for (int i = 0; i < shader_object_used; ++i)
	{
		glDetachShader(program, shader_objects[i]);
		glDeleteShader(shader_objects[i]);
	}

	g_ShaderTable.emplace_back(program);

	gEngfuncs.Con_DPrintf("R_CompileShaderObject [%d] ...\n", program);

	return program;
}

void R_CompileShaderAppendInclude(std::string& str, const char* filename)
{
	std::regex pattern("#include[< \"]+([a-zA-Z_\\.]+)[> \"]");
	std::smatch result;
	std::regex_search(str, result, pattern);

	std::string skipped;

	std::string::const_iterator searchStart(str.cbegin());

	while (std::regex_search(searchStart, str.cend(), result, pattern) && result.size() >= 2)
	{
		std::string prefix = result.prefix();
		std::string suffix = result.suffix();

		auto includeFileName = result[1].str();

		char slash = '/';

		std::string includePath = filename;
		for (size_t j = includePath.length() - 1; j > 0; --j)
		{
			if (includePath[j] == '\\' || includePath[j] == '/')
			{
				slash = includePath[j];
				includePath.resize(j);
				break;
			}
		}

		includePath += slash;
		includePath += includeFileName;

		auto pFile = (char*)gEngfuncs.COM_LoadFile(includePath.c_str(), 5, NULL);
		if (pFile)
		{
			std::string wbinding(pFile);

			gEngfuncs.COM_FreeFile(pFile);

			if (searchStart != str.cbegin())
			{
				str = skipped + prefix;
			}
			else
			{
				str = prefix;
			}
			str += wbinding;

			auto currentLength = str.length();

			str += suffix;

			skipped = str.substr(0, currentLength);
			searchStart = str.cbegin() + currentLength;
			continue;
		}

		searchStart = result.suffix().first;
	}
}

void R_CompileShaderAppendDefine(std::string& str, const std::string& def)
{
	std::regex pattern("(#version [0-9a-z ]+)");
	std::smatch result;
	std::regex_search(str, result, pattern);

	if (result.size() >= 1)
	{
		std::string prefix = result[0];
		std::string suffix = result.suffix();

		str = prefix;
		str += "\n\n";
		str += def;
		str += "\n\n";
		str += suffix;
	}
	else
	{
		std::string suffix = str;

		str = def;
		str += "\n\n";
		str += suffix;
	}
}

GLuint GL_CompileShaderFileEx(const CCompileShaderArgs* args)
{
	CCompileShaderContext context = {};

#define LOAD_SHADER_STAGE(stage, macro)\
	if (args->stage##file){\
		auto stage##code = (char *)gEngfuncs.COM_LoadFile(args->stage##file, 5, 0);\
		if (!stage##code)\
		{\
			SYS_ERROR("GL_CompileShaderFileEx: \"%s\" not found!", args->stage##file);\
			return 0;\
		}\
		SCOPE_EXIT{ gEngfuncs.COM_FreeFile(stage##code); };\
		gEngfuncs.Con_DPrintf("GL_CompileShaderFileEx: compiling %s...\n", args->stage##file);\
		context.stage##code = stage##code;\
		R_CompileShaderAppendDefine(context.stage##code, macro);\
		if (args->stage##define)\
		{\
			R_CompileShaderAppendDefine(context.stage##code, args->stage##define);\
		}\
		if (context.stage##code.find("#include") != std::string::npos)\
		{\
			R_CompileShaderAppendInclude(context.stage##code, args->stage##file);\
		}\
	}

	LOAD_SHADER_STAGE(vs, "#define IS_VERTEX_SHADER\n")
	LOAD_SHADER_STAGE(gs, "#define IS_GEOMETRY_SHADER\n")
	LOAD_SHADER_STAGE(fs, "#define IS_FRAGMENT_SHADER\n")

#undef LOAD_SHADER_STAGE

	return R_CompileShader(args, &context);
}

GLuint GL_CompileShaderFile(const char* vsfile, const char* fsfile, const char* vsdefine, const char* fsdefine)
{
	CCompileShaderArgs args = {};
	args.vsfile = vsfile;
	args.fsfile = fsfile;
	args.vsdefine = vsdefine;
	args.fsdefine = fsdefine;

	return GL_CompileShaderFileEx(&args);
}

void GL_UseProgram(GLuint program)
{
	glUseProgram(program);
}