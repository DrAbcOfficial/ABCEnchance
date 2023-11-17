#include "vpx/vpx_decoder.h"

vpx_codec_err_t (*vpx_codec_dec_init_ver)(vpx_codec_ctx_t *ctx,
                                       vpx_codec_iface_t *iface,
                                       const vpx_codec_dec_cfg_t *cfg,
                                       vpx_codec_flags_t flags, int ver);

vpx_codec_err_t(*vpx_codec_peek_stream_info)(vpx_codec_iface_t *iface,
                                           const uint8_t *data,
                                           unsigned int data_sz,
                                           vpx_codec_stream_info_t *si);

vpx_codec_err_t(*vpx_codec_get_stream_info)(vpx_codec_ctx_t *ctx,
                                          vpx_codec_stream_info_t *si);

vpx_codec_err_t(*vpx_codec_decode)(vpx_codec_ctx_t *ctx, const uint8_t *data,
                                 unsigned int data_sz, void *user_priv,
                                 long deadline);

vpx_image_t * (*vpx_codec_get_frame)(vpx_codec_ctx_t *ctx, vpx_codec_iter_t *iter);

vpx_codec_err_t(*vpx_codec_register_put_frame_cb)(vpx_codec_ctx_t *ctx,
                                                vpx_codec_put_frame_cb_fn_t cb,
                                                void *user_priv);

vpx_codec_err_t(*vpx_codec_register_put_slice_cb)(vpx_codec_ctx_t *ctx,
                                                vpx_codec_put_slice_cb_fn_t cb,
                                                void *user_priv);

vpx_codec_err_t(*vpx_codec_set_frame_buffer_functions)(
    vpx_codec_ctx_t *ctx, vpx_get_frame_buffer_cb_fn_t cb_get,
    vpx_release_frame_buffer_cb_fn_t cb_release, void *cb_priv);

vpx_codec_iface_t* (*vpx_codec_vp8_dx)(void);

vpx_codec_iface_t* (*vpx_codec_vp9_dx)(void);