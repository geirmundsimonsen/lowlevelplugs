#include "../clap/include/clap/clap.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"
#include "util.h"
#include "osc.h"
#include "tables.h"

#define PLG P000
#define PLG_TICK P000_tick
#define PLG_CREATE create_P000

typedef struct {
  int pitch;
  double freq;
  tabplay_t rel_env;
  osc_t osc;
  bool active;
  bool release;
} Voice;

static double voice_tick(Voice* self) {
  self->osc.freq = self->freq;
  double out = osc_tick(&self->osc);
  if (self->release) {
    auto rel = tabplay_tick(&self->rel_env);
    out *= rel;
    if (self->rel_env.phase == 1) { self->active = false; }
  }
  return out;
}

static Voice voice(int pitch) {
  Voice p = { pitch, midipitch2freq(pitch), { 0.0, 0.05, et_fall_lin }, { 0.0, 0.0, wt_sin }, true, false };
  return p;
}

typedef struct {
  Voice voices[16];
} PLG;

static void add_voice_at_pitch(PLG* self, int pitch) {
  for (int i = 0; i < 16; i++) {
    if (!self->voices[i].active) {
      self->voices[i] = voice(pitch);
      break;
    }
  }
}

static void release_voice_at_pitch(PLG* self, int pitch) {
  for (int i = 0; i < 16; i++) {
    if (self->voices[i].active && self->voices[i].pitch == pitch) {
      self->voices[i].release = true;
      break;
    }
  }
}

double PLG_TICK(PLG* self) {
  auto out = 0.0;
  for (int i = 0; i < 16; i++) {
    if (self->voices[i].active) {
      out += voice_tick(&self->voices[i]);
    }
  }
  out *= 0.25;
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

static uint32_t plugin_note_ports_count(const clap_plugin_t *plugin, bool is_input) {
  return 1;
}

static bool plugin_note_ports_get(const clap_plugin_t* plugin, uint32_t index, bool is_input, clap_note_port_info_t* info) {
  if (index > 0) { return false; }
  info->id = 0;
  snprintf(info->name, sizeof(info->name), "%s", "Note Port 1");
  info->supported_dialects = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI_MPE | CLAP_NOTE_DIALECT_MIDI2;
  info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
  return true;
}

static const clap_plugin_note_ports_t plugin_note_ports = {
  .count = plugin_note_ports_count,
  .get = plugin_note_ports_get,
};

static bool plugin_init(const struct clap_plugin* plugin) {
  write_log("plugin_init");
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

      // hdr->time - sample index within buffer
      if (hdr->type == 0) { // NOTE ON
        const clap_event_note_t *ev = (const clap_event_note_t *)hdr;
        add_voice_at_pitch((PLG*)plugin->plugin_data, ev->key);
      } else if (hdr->type == 1) { // NOTE OFF
        const clap_event_note_t *ev = (const clap_event_note_t *)hdr;
        release_voice_at_pitch((PLG*)plugin->plugin_data, ev->key);
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

      auto out = PLG_TICK((PLG*)plugin->plugin_data);
      
      L += out;
      R += out;

      process->audio_outputs[0].data32[0][i] = L;
      process->audio_outputs[0].data32[1][i] = R;
    }
  }

  return CLAP_PROCESS_CONTINUE;
}

static const void *plugin_get_extension(const struct clap_plugin *plugin, const char *id) {
  write_log("plugin_get_extension");
  //if (!strcmp(id, CLAP_EXT_LATENCY))
  //   return &s_my_plug_latency;
  if (!strcmp(id, CLAP_EXT_AUDIO_PORTS)) { return &plugin_audio_ports; }
  if (!strcmp(id, CLAP_EXT_NOTE_PORTS)) { return &plugin_note_ports; }
  //if (!strcmp(id, CLAP_EXT_STATE))
  //   return &s_my_plug_state;
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