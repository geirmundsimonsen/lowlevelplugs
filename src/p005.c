
#include "../clap/include/clap/clap.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "clap_default_fns.h"
#include "log.h"
#include "util.h"
#include "osc.h"
#include "filter.h"
#include "tables.h"

typedef struct {
  int iVec0[2];
  int fSampleRate;
  double fConst0;
  double fConst1;
  double fConst2;
  double lpf_freq;
  double lpf_q;
  double freq;
  double fConst3;
  double fRec5[2];
  double fRec1[2];
  double fRec2[2];
  double fRec3[2];
  double fRec4[2];
  double fConst4;
  double fRec0[2];
} faust_p005;

faust_p005* newfaust_p005() { 
  faust_p005* dsp = (faust_p005*)calloc(1, sizeof(faust_p005));
  return dsp;
}

void deletefaust_p005(faust_p005* dsp) { 
  free(dsp);
}

int getSampleRatefaust_p005(faust_p005* __restrict__ dsp) {
  return dsp->fSampleRate;
}

int getNumInputsfaust_p005(faust_p005* __restrict__ dsp) {
  return 0;
}
int getNumOutputsfaust_p005(faust_p005* __restrict__ dsp) {
  return 1;
}

void classInitfaust_p005(int sample_rate) {
}

void instanceResetUserInterfacefaust_p005(faust_p005* dsp) {
  dsp->lpf_freq = (double)(0.0);
  dsp->lpf_q = (double)(0.0);
  dsp->freq = (double)(0.0);
}

void instanceClearfaust_p005(faust_p005* dsp) {
  
  {
    int l0;
    for (l0 = 0; l0 < 2; l0 = l0 + 1) {
      dsp->iVec0[l0] = 0;
    }
  }
  
  {
    int l1;
    for (l1 = 0; l1 < 2; l1 = l1 + 1) {
      dsp->fRec5[l1] = 0.0;
    }
  }
  
  {
    int l2;
    for (l2 = 0; l2 < 2; l2 = l2 + 1) {
      dsp->fRec1[l2] = 0.0;
    }
  }
  
  {
    int l3;
    for (l3 = 0; l3 < 2; l3 = l3 + 1) {
      dsp->fRec2[l3] = 0.0;
    }
  }
  
  {
    int l4;
    for (l4 = 0; l4 < 2; l4 = l4 + 1) {
      dsp->fRec3[l4] = 0.0;
    }
  }
  
  {
    int l5;
    for (l5 = 0; l5 < 2; l5 = l5 + 1) {
      dsp->fRec4[l5] = 0.0;
    }
  }
  
  {
    int l6;
    for (l6 = 0; l6 < 2; l6 = l6 + 1) {
      dsp->fRec0[l6] = 0.0;
    }
  }
}

void instanceConstantsfaust_p005(faust_p005* dsp, int sample_rate) {
  dsp->fSampleRate = sample_rate;
  dsp->fConst0 = (double)(dsp->fSampleRate);
  dsp->fConst1 = 6.283185307179586 / dsp->fConst0;
  dsp->fConst2 = 1.0 - dsp->fConst1;
  dsp->fConst3 = 1.0 / dsp->fConst0;
  dsp->fConst4 = 1.0 / (dsp->fConst1 + 1.0);
}
  
void instanceInitfaust_p005(faust_p005* dsp, int sample_rate) {
  instanceConstantsfaust_p005(dsp, sample_rate);
  instanceResetUserInterfacefaust_p005(dsp);
  instanceClearfaust_p005(dsp);
}

void initfaust_p005(faust_p005* dsp, int sample_rate) {
  classInitfaust_p005(sample_rate);
  instanceInitfaust_p005(dsp, sample_rate);
}

void framefaust_p005(faust_p005* dsp, double* __restrict__ inputs, double* __restrict__ outputs) {
  double fSlow0 = tan(dsp->fConst1 * pow(1e+01, 3.0 * (double)(dsp->lpf_freq) + 1.0));
  double fSlow1 = fSlow0 + 1.0;
  double fSlow2 = fSlow0 / fSlow1;
  double fSlow3 = 1.0 - fSlow2;
  double fSlow4 = (double)(dsp->lpf_q) + -0.7071067811865475;
  double fSlow5 = 0.21521822675751856 * fSlow4 * fSlow3;
  double fSlow6 = dsp->fConst3 * (double)(dsp->freq);
  double fSlow7 = 1.0 / fSlow1;
  double fSlow8 = 1.0 / (1.0 - 0.21521822675751856 * (fSlow0 * fSlow4 * fSlow3 / fSlow1));
  double fSlow9 = 2.0 * fSlow2;
  double fSlow10 = 0.21521822675751856 * fSlow4;
  dsp->iVec0[0] = 1;
  double fTemp0 = ((1 - dsp->iVec0[1]) ? 0.0 : fSlow6 + dsp->fRec5[1]);
  dsp->fRec5[0] = fTemp0 - floor(fTemp0);
  double fTemp1 = 0.5 * (2.0 * (double)(dsp->fRec5[0] <= 0.1) + -1.0) - dsp->fRec4[1];
  double fTemp2 = fSlow8 * (dsp->fRec4[1] + fSlow7 * (fSlow0 * fTemp1 + fSlow5 * dsp->fRec1[1] - dsp->fRec3[1])) - dsp->fRec1[1];
  dsp->fRec1[0] = dsp->fRec1[1] + fSlow9 * fTemp2;
  double fTemp3 = dsp->fRec1[1] + fSlow2 * fTemp2;
  dsp->fRec2[0] = fTemp3;
  dsp->fRec3[0] = dsp->fRec3[1] + fSlow9 * (fSlow10 * fTemp3 - dsp->fRec3[1]);
  dsp->fRec4[0] = dsp->fRec4[1] + fSlow9 * fTemp1;
  dsp->fRec0[0] = dsp->fConst4 * (dsp->fRec2[0] - dsp->fRec2[1] + dsp->fConst2 * dsp->fRec0[1]);
  outputs[0] = (double)(dsp->fRec0[0]);
  dsp->iVec0[1] = dsp->iVec0[0];
  dsp->fRec5[1] = dsp->fRec5[0];
  dsp->fRec1[1] = dsp->fRec1[0];
  dsp->fRec2[1] = dsp->fRec2[0];
  dsp->fRec3[1] = dsp->fRec3[0];
  dsp->fRec4[1] = dsp->fRec4[0];
  dsp->fRec0[1] = dsp->fRec0[0];
}



typedef struct {
  double l;
  double r;
} StereoOut;

typedef struct {
  int pitch;
  faust_p005 faust;
  TabPlay rel_env;
  bool active;
  bool release;
} Voice;

static Voice voice_init(int pitch) {
  Voice v = {0};
  v.rel_env = tabplay_init(768000);
  initfaust_p005(&v.faust, 768000);
  v.faust.freq = midipitch2freq(pitch);
  v.rel_env.s = 0.05;
  v.rel_env.wt = et_fall_lin;

  v.pitch = pitch;
  v.active = true;
  return v;
}

typedef struct {
  Voice voices[16];
  double lpf_freq;
  double lpf_q;

  FixedBLP8 fixed_lpf_l;
  FixedBLP8 fixed_lpf_r;
} p005;

static StereoOut voice_tick(Voice* v, p005* p) {
  StereoOut so = { 0 };
  
  v->faust.lpf_freq = p->lpf_freq;
  v->faust.lpf_q = p->lpf_q;

  framefaust_p005(&v->faust, 0, &so.l);
  so.r = so.l;

  if (v->release) {
    auto rel = tabplay_tick(&v->rel_env);
    so.l *= rel;
    so.r *= rel;
    if (v->rel_env.phase == 1) { v->active = false; }
  }
  return so;
}

static void add_voice_at_pitch(p005* p, int pitch) {
  for (int i = 0; i < 16; i++) {
    if (!p->voices[i].active) {
      p->voices[i] = voice_init(pitch);
      break;
    }
  }
}

static void release_voice_at_pitch(p005* p, int pitch) {
  for (int i = 0; i < 16; i++) {
    if (p->voices[i].active && p->voices[i].pitch == pitch) {
      p->voices[i].release = true;
      break;
    }
  }
}

StereoOut p005_tick(p005* p) {
  StereoOut out = { 0 };
  for (int i = 0; i < 16; i++) { // oversampling block
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
    out.l = fixedblp8_tick(&p->fixed_lpf_l);
    out.r = fixedblp8_tick(&p->fixed_lpf_r);
  }
  //out.l *= 0.25;
  //out.r *= 0.25;
  return out;
}

static uint32_t plugin_params_count(const clap_plugin_t* plugin) { return 2; }

static bool plugin_params_get_info(const clap_plugin_t* plugin, uint32_t param_index, clap_param_info_t* param_info) {
  switch (param_index) {
    case 0: {
      param_info->id = 1;
      param_info->min_value = 0;
      param_info->max_value = 1;
      param_info->default_value = 0;
      snprintf(param_info->name, sizeof(param_info->name), "%s", "lpf_freq");
    } break;
    case 1: {
      param_info->id = 2;
      param_info->min_value = 0;
      param_info->max_value = 1;
      param_info->default_value = 0;
      snprintf(param_info->name, sizeof(param_info->name), "%s", "lpf_q");
    } break;

  }

  return true;
}

static const clap_plugin_params_t plugin_params = {
  .count = plugin_params_count,
  .flush = default_plugin_params_flush,
  .get_info = plugin_params_get_info,
  .get_value = default_plugin_params_get_value,
  .text_to_value = default_plugin_params_text_to_value,
  .value_to_text = default_plugin_params_value_to_text,
};

static bool plugin_init(const struct clap_plugin* plugin) {
  p005* data = plugin->plugin_data;
  data->fixed_lpf_l = fixedblp8_init(768000, 13000);
  data->fixed_lpf_r = fixedblp8_init(768000, 13000);
  return true;
}

static void plugin_destroy(const struct clap_plugin* plugin) {
  free((p005*)plugin->plugin_data);
  free((struct clap_plugin*)plugin);
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
        add_voice_at_pitch((p005*)plugin->plugin_data, ev->key);
      } else if (hdr->type == CLAP_EVENT_NOTE_OFF) {
        const clap_event_note_t *ev = (const clap_event_note_t*)hdr;
        release_voice_at_pitch((p005*)plugin->plugin_data, ev->key);
      } else if (hdr->type == CLAP_EVENT_PARAM_VALUE) {
        const clap_event_param_value_t *ev = (const clap_event_param_value_t*)hdr;
        p005* p = plugin->plugin_data;
        double val = ev->value;
        switch (ev->param_id) {
          case 1: {     
            p->lpf_freq = val;
          } break;
          case 2: {     
            p->lpf_q = val * 9.3 + 0.707;
          } break;

        }
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

      StereoOut out = p005_tick((p005*)plugin->plugin_data);
      
      L += out.l;
      R += out.r;

      process->audio_outputs[0].data32[0][i] = L;
      process->audio_outputs[0].data32[1][i] = R;
    }
  }

  return CLAP_PROCESS_CONTINUE;
}

static const void *plugin_get_extension(const struct clap_plugin *plugin, const char *id) {
  if (!strcmp(id, CLAP_EXT_AUDIO_PORTS)) { return &one_stereo_audio_port; }
  if (!strcmp(id, CLAP_EXT_NOTE_PORTS)) { return &one_note_port; }
  if (!strcmp(id, CLAP_EXT_PARAMS)) { return &plugin_params; }
  return NULL;
}

const clap_plugin_t* p005_create(const clap_plugin_descriptor_t* plugindesc, const clap_host_t* host) {
  clap_plugin_t* plugin = (clap_plugin_t*)calloc(1, sizeof(*plugin));
  plugin->desc = plugindesc;
  plugin->init = plugin_init;
  plugin->destroy = plugin_destroy;
  plugin->activate = default_plugin_activate;
  plugin->deactivate = default_plugin_deactivate;
  plugin->start_processing = default_plugin_start_processing;
  plugin->stop_processing = default_plugin_stop_processing;
  plugin->reset = default_plugin_reset;
  plugin->process = plugin_process;
  plugin->get_extension = plugin_get_extension;
  plugin->on_main_thread = default_plugin_on_main_thread;
  p005* data = calloc(1, sizeof(*data));
  plugin->plugin_data = data;

  return plugin;
}

