#pragma once

#include "../clap/include/clap/clap.h"

bool default_plugin_activate(const struct clap_plugin* plugin, double sample_rate, uint32_t min_frames_count, uint32_t max_frames_count);
void default_plugin_deactivate(const struct clap_plugin* plugin);
bool default_plugin_start_processing(const struct clap_plugin* plugin);
void default_plugin_stop_processing(const struct clap_plugin* plugin);
void default_plugin_reset(const struct clap_plugin* plugin);
void default_plugin_on_main_thread(const struct clap_plugin *plugin);

bool default_plugin_params_get_value(const clap_plugin_t* plugin, clap_id param_id, double* out_value);
bool default_plugin_params_value_to_text(const clap_plugin_t* plugin, clap_id param_id, double value, char* out_buffer, uint32_t out_buffer_capacity);
bool default_plugin_params_text_to_value(const clap_plugin_t* plugin, clap_id param_id, const char* param_value_text, double* out_value);
void default_plugin_params_flush(const clap_plugin_t* plugin, const clap_input_events_t* in, const clap_output_events_t* out);

extern const clap_plugin_audio_ports_t one_stereo_audio_port;
extern const clap_plugin_note_ports_t one_note_port;