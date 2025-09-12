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

typedef struct {
  int pitch;
  double freq;
  Pulse pulse1;
  Pulse pulse2;
  K35_LPF lpfL;
  K35_LPF lpfR;
  TabPlay rel_env;
  bool active;
  bool release;
} Voice;

static Voice voice_init(int pitch) {
  Voice v = {0};
  v.pulse1 = pulse_init(SR*OS);
  v.pulse2 = pulse_init(SR*OS);
  v.lpfL = k35_lpf_init(SR*OS);
  v.lpfR = k35_lpf_init(SR*OS);
  v.rel_env = tabplay_init(SR*OS);

  v.freq = midipitch2freq(pitch);
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
  double freq_deviation;
  double cross_pulse_duty_strength;
  double duty;
  double pulse2vol;
  double stereo_spread;
  double filter_freq;
  double filter_q;
  double cross_filter_freq_fb;
} PLG;

static StereoOut voice_tick(Voice* v, PLG* p) {
  StereoOut so = { 0 };
  
  double pulse1 = pulse_tick(&v->pulse1);
  double pulse2 = pulse_tick(&v->pulse2);
  v->pulse1.freq = v->freq + p->freq_deviation;
  v->pulse2.freq = v->freq - p->freq_deviation;
  v->pulse1.duty = p->duty + 0.5 + pulse2 * p->cross_pulse_duty_strength * (0.5 - p->duty);
  v->pulse2.duty = p->duty + 0.5 + pulse1 * p->cross_pulse_duty_strength * (0.5 - p->duty);
  pulse1 *= 0.3;
  pulse2 *= 0.3;
  v->lpfL.in = pulse1;
  v->lpfR.in = pulse2 * p->pulse2vol;
  
  v->lpfL.q = p->filter_q;
  v->lpfR.q = p->filter_q;
  double lpfL = k35_lpf_tick(&v->lpfL);
  double lpfR = k35_lpf_tick(&v->lpfR);
  v->lpfL.freq = p->filter_freq + lpfR * p->filter_freq * p->cross_filter_freq_fb;
  v->lpfR.freq = p->filter_freq + lpfL * p->filter_freq * p->cross_filter_freq_fb;

  so.l = lpfL;
  so.r = lpfR;
  so.l += lpfR * (1-p->stereo_spread);
  so.r += lpfL * (1-p->stereo_spread);

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
    p->freq_deviation = pow(cc_val, 2) * 10;
  } else if (cc_num == 2) {
    p->duty = pow(cc_val, 0.75) * 0.5;
  } else if (cc_num == 3) {
    p->cross_pulse_duty_strength = cc_val;
  } else if (cc_num == 4) {
    p->pulse2vol = cc_val;
  } else if (cc_num == 5) {
    p->stereo_spread = cc_val;
  } else if (cc_num == 6) {
    p->filter_freq = pow(cc_val, 2) * 10000 + 40;
  } else if (cc_num == 7) {
    p->filter_q = cc_val + 1 * 9;
  } else if (cc_num == 8) {
    p->cross_filter_freq_fb = cc_val * 3;
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
    for (int i = 0; i < 16; i++) {
      if (p->voices[i].active) {
        StereoOut voiceOut;
        voiceOut = voice_tick(&p->voices[i], p);
        out.l += voiceOut.l;
        out.r += voiceOut.r;
      }
    }
    p->fixed_lpf_l.in = out.l;
    p->fixed_lpf_r.in = out.r;
    out.l = fixedblp8_tick(&p->fixed_lpf_l);
    out.r = fixedblp8_tick(&p->fixed_lpf_r);
    if (fabs(out.l) < 0.0001 || fabs(out.r) < 0.0001) {
      break; // cut the remaining oversampling passes if no sound
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
      
      if (hdr->type == 0) { // NOTE ON
        const clap_event_note_t *ev = (const clap_event_note_t*)hdr;
        add_voice_at_pitch((PLG*)plugin->plugin_data, ev->key);
      } else if (hdr->type == 1) { // NOTE OFF
        const clap_event_note_t *ev = (const clap_event_note_t*)hdr;
        release_voice_at_pitch((PLG*)plugin->plugin_data, ev->key);
      } else if (hdr->type == 10) { // MIDI
        const clap_event_midi_t *ev = (const clap_event_midi_t*)hdr;
        if (ev->data[0] == 176) { // cc ch. 1
          update_cc_params((PLG*)plugin->plugin_data, ev->data[1], (double)ev->data[2] / 127.0); 
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