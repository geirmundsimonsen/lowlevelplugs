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

#define PLG P003
#define PLG_TICK P003_tick
#define PLG_CREATE create_P003
#define SR 48000
#define OS 16

typedef struct {
  double l;
  double r;
} StereoOut;

static double freq_deviation;
static double cross_pulse_duty_strength;
static double duty;
static double pulse2vol;
static double stereo_spread;
static double filter_freq;
static double filter_q;
static void update_cc_params(int cc_num, double cc_val) {
  if (cc_num == 1) {
    freq_deviation = pow(cc_val, 2) * 10;
  } else if (cc_num == 2) {
    duty = pow(cc_val, 0.75) * 0.5;
  } else if (cc_num == 3) {
    cross_pulse_duty_strength = cc_val;
  } else if (cc_num == 4) {
    pulse2vol = cc_val;
  } else if (cc_num == 5) {
    stereo_spread = cc_val;
  } else if (cc_num == 6) {
    filter_freq = pow(cc_val, 2) * 10000 + 40;
  } else if (cc_num == 7) {
    filter_q = cc_val + 1 * 9;
  }
}

typedef struct {
  int pitch;
  double freq;
  Pulse pulse1;
  Pulse pulse2;
  K35_LPF lpfL;
  K35_LPF lpfR;
  FixedBLP8 blpL;
  FixedBLP8 blpR;
  TabPlay rel_env;
  bool active;
  bool release;
} Voice;

static StereoOut voice_tick(Voice* self) {
  StereoOut so = { 0 };
  for (int i = 0; i < OS; i++) {
    double pulse1 = pulse_tick(&self->pulse1);
    double pulse2 = pulse_tick(&self->pulse2);
    self->pulse1.freq = self->freq + freq_deviation;
    self->pulse2.freq = self->freq - freq_deviation;
    self->pulse1.duty = duty + 0.5 + pulse2 * cross_pulse_duty_strength * (0.5 - duty);
    self->pulse2.duty = duty + 0.5 + pulse1 * cross_pulse_duty_strength * (0.5 - duty);
    pulse1 *= 0.3;
    pulse2 *= 0.3;
    self->lpfL.in = pulse1;
    self->lpfR.in = pulse2 * pulse2vol;
    
    self->lpfL.freq = filter_freq;
    self->lpfR.freq = filter_freq;
    self->lpfL.q = filter_q;
    self->lpfR.q = filter_q;
    double lpfL = k35_lpf_tick(&self->lpfL);
    double lpfR = k35_lpf_tick(&self->lpfR);

    self->blpL.in = lpfL;
    self->blpR.in = lpfR;
    lpfL = fixedblp8_tick(&self->blpL);
    lpfR = fixedblp8_tick(&self->blpR);
    
    if (i == OS-1) {
      so.l = lpfL;
      so.r = lpfR;
      so.l += lpfR * (1-stereo_spread);
      so.r += lpfL * (1-stereo_spread);
    }
  }

  if (self->release) {
    auto rel = tabplay_tick(&self->rel_env);
    so.l *= rel;
    so.r *= rel;
    if (self->rel_env.phase == 1) { self->active = false; }
  }
  return so;
}

static Voice voice_init(int pitch) {
  Voice v = {0};
  v.pulse1 = pulse_init(SR*OS);
  v.pulse2 = pulse_init(SR*OS);
  v.lpfL = k35_lpf_init(SR*OS);
  v.lpfR = k35_lpf_init(SR*OS);
  v.blpL = fixedblp8_init(SR*OS, 12000);
  v.blpR = fixedblp8_init(SR*OS, 12000);
  v.rel_env = tabplay_init(SR);

  v.freq = midipitch2freq(pitch);
  v.rel_env.s = 0.05;
  v.rel_env.wt = et_fall_lin;

  v.pitch = pitch;
  v.active = true;
  return v;
}

typedef struct {
  Voice voices[16];
} PLG;

static void add_voice_at_pitch(PLG* self, int pitch) {
  for (int i = 0; i < 16; i++) {
    if (!self->voices[i].active) {
      self->voices[i] = voice_init(pitch);
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

StereoOut PLG_TICK(PLG* self) {
  StereoOut out = { 0 };
  for (int i = 0; i < 16; i++) {
    if (self->voices[i].active) {
      StereoOut voiceOut = voice_tick(&self->voices[i]);
      out.l += voiceOut.l;
      out.r += voiceOut.r;
    }
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

static uint32_t plugin_note_ports_count(const clap_plugin_t *plugin, bool is_input) {
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
      
      if (hdr->type == 0) { // NOTE ON
        const clap_event_note_t *ev = (const clap_event_note_t*)hdr;
        add_voice_at_pitch((PLG*)plugin->plugin_data, ev->key);
      } else if (hdr->type == 1) { // NOTE OFF
        const clap_event_note_t *ev = (const clap_event_note_t*)hdr;
        release_voice_at_pitch((PLG*)plugin->plugin_data, ev->key);
      } else if (hdr->type == 10) { // MIDI
        const clap_event_midi_t *ev = (const clap_event_midi_t*)hdr;
        if (ev->data[0] == 176) { // cc ch. 1
          update_cc_params(ev->data[1], (double)ev->data[2] / 127.0); 
        }
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