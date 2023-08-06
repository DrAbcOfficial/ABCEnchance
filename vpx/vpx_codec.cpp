#include "vpx/vpx_codec.h"

vpx_codec_err_t(*vpx_codec_destroy)(vpx_codec_ctx_t* ctx);
const char* (*vpx_codec_error)(vpx_codec_ctx_t* ctx);
const char* (*vpx_codec_error_detail)(vpx_codec_ctx_t* ctx);