#include "../clap/include/clap/clap.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "log.h"
#include "util.h"
#include "osc.h"
#include "filter.h"
#include "tables.h"

#include "p004-faust.c"

#define PLG P004
#define PLG_TICK P004_tick
#define PLG_CREATE create_P004
#define SR 48000
#define OS 16



typedef struct {
  double l;
  double r;
} StereoOut;

typedef struct {
  int pitch;
  mydsp faust;
  TabPlay rel_env;
  bool active;
  bool release;
} Voice;

static Voice voice_init(int pitch) {
  Voice v = {0};
  v.rel_env = tabplay_init(SR*OS);
  initmydsp(&v.faust, SR*OS);
  v.faust.freq = midipitch2freq(pitch);
  v.rel_env.s = 0.05;
  v.rel_env.wt = et_fall_lin;

  v.pitch = pitch;
  v.active = true;
  return v;
}

typedef struct {
  Voice voices[16];
  FixedBLP8 fixed_lpf_l;
  FixedBLP8 fixed_lpf_r;
  double cc1;
  double cc2;
  double cc3;
  double cc4;
  double cc5;
  double cc6;
  double cc7;
  double cc8;
  double param1;
} PLG;

static StereoOut voice_tick(Voice* v, PLG* p) {
  StereoOut so = { 0 };
  
  v->faust.lpf_freq = p->param1;
  v->faust.lpf_q = p->cc7;
  framemydsp(&v->faust, 0, &so.l);
  so.r = so.l;

  if (v->release) {
    auto rel = tabplay_tick(&v->rel_env);
    so.l *= rel;
    so.r *= rel;
    if (v->rel_env.phase == 1) { v->active = false; }
  }
  return so;
}

static void update_cc_params(PLG* p, int cc_num, double cc_val) {
  if (cc_num == 1) {
    p->cc1 = cc_val;
  } else if (cc_num == 2) {
    p->cc2 = cc_val;
  } else if (cc_num == 3) {
    p->cc3 = cc_val;
  } else if (cc_num == 4) {
    p->cc4 = cc_val;
  } else if (cc_num == 5) {
    p->cc5 = cc_val;
  } else if (cc_num == 6) {
    p->cc6 = pow(cc_val * 0.7 + 0.3, 2);
  } else if (cc_num == 7) {
    p->cc7 = cc_val * 9.3 + 0.707;
  } else if (cc_num == 8) {
    p->cc8 = cc_val;
  }
}

static void update_params(PLG* p, clap_id param_id, double val) {
  if (param_id == 1) {
    p->param1 = val;
  }
}

static void add_voice_at_pitch(PLG* p, int pitch) {
  for (int i = 0; i < 16; i++) {
    if (!p->voices[i].active) {
      p->voices[i] = voice_init(pitch);
      break;
    }
  }
}

static void release_voice_at_pitch(PLG* p, int pitch) {
  for (int i = 0; i < 16; i++) {
    if (p->voices[i].active && p->voices[i].pitch == pitch) {
      p->voices[i].release = true;
      break;
    }
  }
}

StereoOut PLG_TICK(PLG* p) {
  StereoOut out = { 0 };
  for (int i = 0; i < OS; i++) { // oversampling block
    out.l = 0;
    out.r = 0;
    for (int v = 0; v < 16; v++) {
      if (p->voices[v].active) {
        StereoOut voiceOut;
        voiceOut = voice_tick(&p->voices[v], p);
        
        out.l += voiceOut.l;
        out.r += voiceOut.r;
      }
    }
    p->fixed_lpf_l.in = out.l;
    p->fixed_lpf_r.in = out.r;
    //out.l = fixedblp8_tick(&p->fixed_lpf_l);
    //out.r = fixedblp8_tick(&p->fixed_lpf_r);
    // An attempt to be "smart" and avoid processing when not needed, but...
    // DANGEROUS. The signal is - many times - below -80db for a single sample.
    // Keeping this code as a comment, as something *not* to do.
    // Result: Distortions of a strange and hard-to-debug kind.
    /*if (fabs(out.l) < 0.0001 || fabs(out.r) < 0.0001) {
      break; // cut the remaining oversampling passes if no sound
    }*/
  }
  out.l *= 0.25;
  out.r *= 0.25;
  return out;
}

static uint32_t plugin_audio_ports_count(const clap_plugin_t* plugin, bool is_input) {
  return 1;
}

static bool plugin_audio_ports_get(const clap_plugin_t* plugin, uint32_t index, bool is_input, clap_audio_port_info_t* info) {
  if (index > 0) { return false; }
  info->id = 0;
  snprintf(info->name, sizeof(info->name), "%s", "Main");
  info->channel_count = 2;
  info->flags = CLAP_AUDIO_PORT_IS_MAIN;
  info->port_type = CLAP_PORT_STEREO;
  info->in_place_pair = CLAP_INVALID_ID;
  return true;
}

static const clap_plugin_audio_ports_t plugin_audio_ports = {
  plugin_audio_ports_count,
  plugin_audio_ports_get,
};

static uint32_t plugin_note_ports_count(const clap_plugin_t* plugin, bool is_input) {
  return 1;
}

static bool plugin_note_ports_get(const clap_plugin_t* plugin, uint32_t index, bool is_input, clap_note_port_info_t* info) {
  if (index > 0) { return false; }
  info->id = 0;
  snprintf(info->name, sizeof(info->name), "%s", "Note Port 1");
  info->supported_dialects = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI | CLAP_NOTE_DIALECT_MIDI_MPE | CLAP_NOTE_DIALECT_MIDI2;
  info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
  return true;
}

static const clap_plugin_note_ports_t plugin_note_ports = {
  .count = plugin_note_ports_count,
  .get = plugin_note_ports_get,
};

static uint32_t plugin_params_count(const clap_plugin_t* plugin) {
  write_log("plugin_params_count");
  return 1;
}

// Copies the parameter's info to param_info.
// Returns true on success.
// [main-thread]
static bool plugin_params_get_info(const clap_plugin_t* plugin, uint32_t param_index, clap_param_info_t* param_info) {
  write_log("plugin_params_get_info");
  if (param_index == 0) {
    write_log("plugin_params_get_info index 0");
    param_info->default_value = 1;
    param_info->id = param_index + 1;
    param_info->max_value = 1;
    param_info->min_value = 0;
    //param_info->flags = CLAP_PARAM_IS_READONLY;
    snprintf(param_info->name, sizeof(param_info->name), "%s", "Param 1");
  }
  return true;
}

// Writes the parameter's current value to out_value.
// Returns true on success.
// [main-thread]
static bool plugin_params_get_value(const clap_plugin_t* plugin, clap_id param_id, double* out_value) {
  write_log("plugin_params_get_value");
  return true;
}

// Fills out_buffer with a null-terminated UTF-8 string that represents the parameter at the
// given 'value' argument. eg: "2.3 kHz". The host should always use this to format parameter
// values before displaying it to the user.
// Returns true on success.
// [main-thread]
static bool plugin_params_value_to_text(const clap_plugin_t* plugin, clap_id param_id, double value, char* out_buffer, uint32_t out_buffer_capacity) {
  write_log("plugin_params_value_to_text");
  return true;
}

// Converts the null-terminated UTF-8 param_value_text into a double and writes it to out_value.
// The host can use this to convert user input into a parameter value.
// Returns true on success.
// [main-thread]
static bool plugin_params_text_to_value(const clap_plugin_t* plugin, clap_id param_id, const char* param_value_text, double* out_value) {
  write_log("plugin_params_text_to_value");
  return true;
}

// Flushes a set of parameter changes.
// This method must not be called concurrently to clap_plugin->process().
//
// Note: if the plugin is processing, then the process() call will already achieve the
// parameter update (bi-directional), so a call to flush isn't required, also be aware
// that the plugin may use the sample offset in process(), while this information would be
// lost within flush().
//
// [active ? audio-thread : main-thread]
static void plugin_params_flush(const clap_plugin_t* plugin, const clap_input_events_t* in, const clap_output_events_t* out) {
  write_log("plugin_params_flush");
}

static const clap_plugin_params_t plugin_params = {
  .count = plugin_params_count,
  .flush = plugin_params_flush,
  .get_info = plugin_params_get_info,
  .get_value = plugin_params_get_value,
  .text_to_value = plugin_params_text_to_value,
  .value_to_text = plugin_params_value_to_text,
};

static bool plugin_init(const struct clap_plugin* plugin) {
  write_log("plugin_init");
  PLG* data = plugin->plugin_data;
  data->fixed_lpf_l = fixedblp8_init(SR*OS, 13000);
  data->fixed_lpf_r = fixedblp8_init(SR*OS, 13000);
  for (int i = 0; i < 128; i++) {
    update_cc_params(data, i, 0);
  }
  return true;
}

static void plugin_destroy(const struct clap_plugin* plugin) {
  write_log("plugin_destroy");
  free((PLG*)plugin->plugin_data);
  free((struct clap_plugin*)plugin);
}

static bool plugin_activate(const struct clap_plugin* plugin, double sample_rate, uint32_t min_frames_count, uint32_t max_frames_count) {
  write_log("plugin_activate");
  return true;
}

static void plugin_deactivate(const struct clap_plugin* plugin) {
  write_log("plugin_deactivate");
}

static bool plugin_start_processing(const struct clap_plugin* plugin) {
  write_log("plugin_start_processing");
  return true;
}

static void plugin_stop_processing(const struct clap_plugin* plugin) {
  write_log("plugin_stop_processing");
}

static void plugin_reset(const struct clap_plugin* plugin) {
  write_log("plugin_reset");
}

static clap_process_status plugin_process(const struct clap_plugin* plugin, const clap_process_t* process) {
  const uint32_t nframes = process->frames_count;
  const uint32_t num_ev = process->in_events->size(process->in_events);
  uint32_t       ev_index = 0;
  uint32_t       next_ev_frame = num_ev > 0 ? 0 : nframes;

  for (uint32_t i = 0; i < nframes;) {
    while (ev_index < num_ev && next_ev_frame == i) {
      const clap_event_header_t *hdr = process->in_events->get(process->in_events, ev_index);
      if (hdr->time != i) {
        next_ev_frame = hdr->time;
        break;
      }
      
      if (hdr->type == CLAP_EVENT_NOTE_ON) {
        const clap_event_note_t *ev = (const clap_event_note_t*)hdr;
        add_voice_at_pitch((PLG*)plugin->plugin_data, ev->key);
      } else if (hdr->type == CLAP_EVENT_NOTE_OFF) {
        const clap_event_note_t *ev = (const clap_event_note_t*)hdr;
        release_voice_at_pitch((PLG*)plugin->plugin_data, ev->key);
      } else if (hdr->type == CLAP_EVENT_MIDI) {
        const clap_event_midi_t *ev = (const clap_event_midi_t*)hdr;
        if (ev->data[0] == 176) { // cc ch. 1
          update_cc_params((PLG*)plugin->plugin_data, ev->data[1], (double)ev->data[2] / 127.0); 
        }
      } else if (hdr->type == CLAP_EVENT_PARAM_VALUE) {
        const clap_event_param_value_t *ev = (const clap_event_param_value_t*)hdr;
        update_params((PLG*)plugin->plugin_data, ev->param_id, ev->value);
      } else if (hdr->type == CLAP_EVENT_PARAM_MOD) {
        const clap_event_param_mod_t *ev = (const clap_event_param_mod_t*)hdr;
      }

      ev_index++;
      if (ev_index == num_ev) {
        next_ev_frame = nframes;
        break;
      }
    }

    for (; i < next_ev_frame; ++i) {
      float L = process->audio_inputs[0].data32[0][i];
      float R = process->audio_inputs[0].data32[1][i];

      StereoOut out = PLG_TICK((PLG*)plugin->plugin_data);
      
      L += out.l;
      R += out.r;

      process->audio_outputs[0].data32[0][i] = L;
      process->audio_outputs[0].data32[1][i] = R;
    }
  }

  return CLAP_PROCESS_CONTINUE;
}

static const void *plugin_get_extension(const struct clap_plugin *plugin, const char *id) {
  write_log("plugin_get_extension");
  if (!strcmp(id, CLAP_EXT_AUDIO_PORTS)) { return &plugin_audio_ports; }
  if (!strcmp(id, CLAP_EXT_NOTE_PORTS)) { return &plugin_note_ports; }
  if (!strcmp(id, CLAP_EXT_PARAMS)) { return &plugin_params; }
  return NULL;
}

static void plugin_on_main_thread(const struct clap_plugin *plugin) {
  write_log("plugin_on_main_thread");
}

const clap_plugin_t* PLG_CREATE(const clap_plugin_descriptor_t* plugindesc) {
  clap_plugin_t* plugin = (clap_plugin_t*)calloc(1, sizeof(*plugin));
  plugin->desc = plugindesc;
  plugin->init = plugin_init;
  plugin->destroy = plugin_destroy;
  plugin->activate = plugin_activate;
  plugin->deactivate = plugin_deactivate;
  plugin->start_processing = plugin_start_processing;
  plugin->stop_processing = plugin_stop_processing;
  plugin->reset = plugin_reset;
  plugin->process = plugin_process;
  plugin->get_extension = plugin_get_extension;
  plugin->on_main_thread = plugin_on_main_thread;
  PLG* data = calloc(1, sizeof(*data));
  plugin->plugin_data = data;

  return plugin;
}
