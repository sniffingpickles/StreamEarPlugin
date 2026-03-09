#include <obs-module.h>
#include <obs-frontend-api.h>
#include "streamear-output.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-streamear", "en-US")

static obs_output_t *streamear_out = NULL;
static obs_encoder_t *streamear_enc = NULL;
static bool relay_active = false;

static void streamear_start_relay(void)
{
    if (relay_active)
        return;

    /* Create AAC encoder for audio capture */
    obs_data_t *enc_settings = obs_data_create();
    obs_data_set_int(enc_settings, "bitrate", 128);
    streamear_enc = obs_audio_encoder_create(
        "ffmpeg_aac", "streamear_aac_enc", enc_settings, 0, NULL);
    obs_data_release(enc_settings);

    if (!streamear_enc) {
        blog(LOG_ERROR, "[StreamEar] Failed to create AAC encoder");
        return;
    }

    obs_encoder_set_audio(streamear_enc, obs_get_audio());

    /* Create the StreamEar output */
    obs_data_t *out_settings = obs_data_create();
    obs_data_set_int(out_settings, "port", 19400);
    streamear_out = obs_output_create(
        "streamear_output", "StreamEar Relay", out_settings, NULL);
    obs_data_release(out_settings);

    if (!streamear_out) {
        blog(LOG_ERROR, "[StreamEar] Failed to create output");
        obs_encoder_release(streamear_enc);
        streamear_enc = NULL;
        return;
    }

    obs_output_set_audio_encoder(streamear_out, streamear_enc, 0);

    if (obs_output_start(streamear_out)) {
        relay_active = true;
        blog(LOG_INFO, "[StreamEar] Audio relay started on port 19400");
    } else {
        blog(LOG_ERROR, "[StreamEar] Failed to start output");
        obs_output_release(streamear_out);
        obs_encoder_release(streamear_enc);
        streamear_out = NULL;
        streamear_enc = NULL;
    }
}

static void streamear_stop_relay(void)
{
    if (!relay_active)
        return;

    if (streamear_out) {
        obs_output_stop(streamear_out);
        obs_output_release(streamear_out);
        streamear_out = NULL;
    }

    if (streamear_enc) {
        obs_encoder_release(streamear_enc);
        streamear_enc = NULL;
    }

    relay_active = false;
    blog(LOG_INFO, "[StreamEar] Audio relay stopped");
}

static void on_tools_menu_clicked(void *data)
{
    (void)data;
    if (relay_active)
        streamear_stop_relay();
    else
        streamear_start_relay();
}

static void on_frontend_event(enum obs_frontend_event event, void *data)
{
    (void)data;
    /* Auto-start when streaming starts, auto-stop when OBS exits */
    switch (event) {
    case OBS_FRONTEND_EVENT_STREAMING_STARTED:
        if (!relay_active) {
            blog(LOG_INFO, "[StreamEar] Auto-starting relay (stream started)");
            streamear_start_relay();
        }
        break;
    case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
        if (relay_active) {
            blog(LOG_INFO, "[StreamEar] Auto-stopping relay (stream stopped)");
            streamear_stop_relay();
        }
        break;
    case OBS_FRONTEND_EVENT_EXIT:
        streamear_stop_relay();
        break;
    default:
        break;
    }
}

bool obs_module_load(void)
{
    obs_register_output(&streamear_output_info);

    obs_frontend_add_tools_menu_item(
        "StreamEar Audio Relay", on_tools_menu_clicked, NULL);

    obs_frontend_add_event_callback(on_frontend_event, NULL);

    blog(LOG_INFO, "[StreamEar] Plugin loaded (v1.1.0)");
    return true;
}

void obs_module_unload(void)
{
    streamear_stop_relay();
    blog(LOG_INFO, "[StreamEar] Plugin unloaded");
}
