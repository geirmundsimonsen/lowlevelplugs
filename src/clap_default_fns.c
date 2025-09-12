#include "clap_default_fns.h"

#include <stdio.h>

bool default_plugin_activate(const struct clap_plugin* plugin, double sample_rate, uint32_t min_frames_count, uint32_t max_frames_count) { return true; }
void default_plugin_deactivate(const struct clap_plugin* plugin) {}
bool default_plugin_start_processing(const struct clap_plugin* plugin) { return true; }
void default_plugin_stop_processing(const struct clap_plugin* plugin) {}
void default_plugin_reset(const struct clap_plugin* plugin) {}
void default_plugin_on_main_thread(const struct clap_plugin *plugin) {}

bool default_plugin_params_get_value(const clap_plugin_t* plugin, clap_id param_id, double* out_value) { return false; }
bool default_plugin_params_value_to_text(const clap_plugin_t* plugin, clap_id param_id, double value, char* out_buffer, uint32_t out_buffer_capacity) {
  if (value != 0) {
    snprintf(out_buffer, out_buffer_capacity, "%.4f", value);
    return true;
  }
  return false;
}
bool default_plugin_params_text_to_value(const clap_plugin_t* plugin, clap_id param_id, const char* param_value_text, double* out_value) { return false; }
void default_plugin_params_flush(const clap_plugin_t* plugin, const clap_input_events_t* in, const clap_output_events_t* out) {}

bool one_stereo_audio_port_count_get(const clap_plugin_t* plugin, uint32_t index, bool is_input, clap_audio_port_info_t* info) {
  if (index > 0) { return false; }
  info->id = 0;
  snprintf(info->name, sizeof(info->name), "%s", "Main");
  info->channel_count = 2;
  info->flags = CLAP_AUDIO_PORT_IS_MAIN;
  info->port_type = CLAP_PORT_STEREO;
  info->in_place_pair = CLAP_INVALID_ID;
  return true;
}
uint32_t one_stereo_audio_port_count(const clap_plugin_t* plugin, bool is_input) { return 1; }
const clap_plugin_audio_ports_t one_stereo_audio_port = { .count = one_stereo_audio_port_count, .get = one_stereo_audio_port_count_get };

bool one_note_port_get(const clap_plugin_t* plugin, uint32_t index, bool is_input, clap_note_port_info_t* info) {
  if (index > 0) { return false; }
  info->id = 0;
  snprintf(info->name, sizeof(info->name), "%s", "Note Port 1");
  info->supported_dialects = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI | CLAP_NOTE_DIALECT_MIDI_MPE | CLAP_NOTE_DIALECT_MIDI2;
  info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
  return true;
}
uint32_t one_note_port_count(const clap_plugin_t* plugin, bool is_input) { return 1; }
const clap_plugin_note_ports_t one_note_port = { .count = one_note_port_count, .get = one_note_port_get, };