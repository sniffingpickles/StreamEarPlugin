#include "streamear-output.h"
#include <obs-module.h>
#include <util/platform.h>

#define DEFAULT_PORT 19400

static const char *streamear_output_getname(void *unused)
{
    UNUSED_PARAMETER(unused);
    return obs_module_text("StreamEarOutput");
}

static void *streamear_output_create(obs_data_t *settings, obs_output_t *output)
{
    struct streamear_output *ctx = bzalloc(sizeof(struct streamear_output));
    ctx->output = output;
    ctx->port = (uint16_t)obs_data_get_int(settings, "port");
    if (ctx->port == 0)
        ctx->port = DEFAULT_PORT;
    ctx->active = false;

    ring_buffer_init(&ctx->ring_buffer);

    return ctx;
}

static void streamear_output_destroy(void *data)
{
    struct streamear_output *ctx = data;
    if (ctx->active) {
        tcp_sender_stop(&ctx->tcp_sender);
    }
    bfree(ctx);
}

static bool streamear_output_start(void *data)
{
    struct streamear_output *ctx = data;

    ring_buffer_init(&ctx->ring_buffer);

    if (!tcp_sender_start(&ctx->tcp_sender, &ctx->ring_buffer, ctx->port)) {
        blog(LOG_ERROR, "[StreamEar] Failed to start TCP sender");
        return false;
    }

    ctx->active = true;
    blog(LOG_INFO, "[StreamEar] Output started on port %d", ctx->port);

    if (!obs_output_can_begin_data_capture(ctx->output, OBS_OUTPUT_AUDIO))
        return false;

    obs_output_begin_data_capture(ctx->output, OBS_OUTPUT_AUDIO);
    return true;
}

static void streamear_output_stop(void *data, uint64_t ts)
{
    struct streamear_output *ctx = data;
    UNUSED_PARAMETER(ts);

    obs_output_end_data_capture(ctx->output);
    tcp_sender_stop(&ctx->tcp_sender);
    ctx->active = false;

    blog(LOG_INFO, "[StreamEar] Output stopped");
}

static void streamear_output_encoded_audio(void *data,
    struct encoder_packet *packet)
{
    struct streamear_output *ctx = data;
    if (!ctx->active)
        return;

    /* Push encoded AAC frame into ring buffer */
    ring_buffer_push(&ctx->ring_buffer,
                     packet->data, packet->size,
                     packet->pts);
}

static obs_properties_t *streamear_output_properties(void *unused)
{
    UNUSED_PARAMETER(unused);

    obs_properties_t *props = obs_properties_create();

    obs_properties_add_int(props, "port",
        obs_module_text("SidecarPort"), 1024, 65535, 1);

    obs_properties_add_text(props, "status",
        obs_module_text("ConnectionStatus"), OBS_TEXT_INFO);

    return props;
}

static void streamear_output_defaults(obs_data_t *settings)
{
    obs_data_set_default_int(settings, "port", DEFAULT_PORT);
}

struct obs_output_info streamear_output_info = {
    .id = "streamear_output",
    .flags = OBS_OUTPUT_AUDIO | OBS_OUTPUT_ENCODED,
    .get_name = streamear_output_getname,
    .create = streamear_output_create,
    .destroy = streamear_output_destroy,
    .start = streamear_output_start,
    .stop = streamear_output_stop,
    .encoded_packet = streamear_output_encoded_audio,
    .get_properties = streamear_output_properties,
    .get_defaults = streamear_output_defaults,
};
