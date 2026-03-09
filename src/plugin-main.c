#include <obs-module.h>
#include "streamear-output.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-streamear", "en-US")

bool obs_module_load(void)
{
    obs_register_output(&streamear_output_info);
    blog(LOG_INFO, "[StreamEar] Plugin loaded (v1.0.0)");
    return true;
}

void obs_module_unload(void)
{
    blog(LOG_INFO, "[StreamEar] Plugin unloaded");
}
