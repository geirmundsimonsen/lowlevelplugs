
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
  int iRec0[2];
} faust_p000SIG0;

static faust_p000SIG0* newfaust_p000SIG0() { return (faust_p000SIG0*)calloc(1, sizeof(faust_p000SIG0)); }
static void deletefaust_p000SIG0(faust_p000SIG0* dsp) { free(dsp); }

int getNumInputsfaust_p000SIG0(faust_p000SIG0* __restrict__ dsp) {
  return 0;
}
int getNumOutputsfaust_p000SIG0(faust_p000SIG0* __restrict__ dsp) {
  return 1;
}

static void instanceInitfaust_p000SIG0(faust_p000SIG0* dsp, int sample_rate) {
  
  {
    int l0;
    for (l0 = 0; l0 < 2; l0 = l0 + 1) {
      dsp->iVec0[l0] = 0;
    }
  }
  
  {
    int l1;
    for (l1 = 0; l1 < 2; l1 = l1 + 1) {
      dsp->iRec0[l1] = 0;
    }
  }
}

static void fillfaust_p000SIG0(faust_p000SIG0* dsp, int count, double* table) {
  
  {
    int i1;
    for (i1 = 0; i1 < count; i1 = i1 + 1) {
      dsp->iVec0[0] = 1;
      dsp->iRec0[0] = (dsp->iVec0[1] + dsp->iRec0[1]) % 65536;
      table[i1] = sin(9.587379924285257e-05 * (double)(dsp->iRec0[0]));
      dsp->iVec0[1] = dsp->iVec0[0];
      dsp->iRec0[1] = dsp->iRec0[0];
    }
  }
}

static double ftbl0faust_p000SIG0[65536];

typedef struct {
  int iVec1[2];
  double freq;
  int fSampleRate;
  double fConst0;
  double fRec1[2];
  double patch_volume;
} faust_p000;

faust_p000* newfaust_p000() { 
  faust_p000* dsp = (faust_p000*)calloc(1, sizeof(faust_p000));
  return dsp;
}

void deletefaust_p000(faust_p000* dsp) { 
  free(dsp);
}

int getSampleRatefaust_p000(faust_p000* __restrict__ dsp) {
  return dsp->fSampleRate;
}

int getNumInputsfaust_p000(faust_p000* __restrict__ dsp) {
  return 0;
}
int getNumOutputsfaust_p000(faust_p000* __restrict__ dsp) {
  return 1;
}

void classInitfaust_p000(int sample_rate) {
  faust_p000SIG0* sig0 = newfaust_p000SIG0();
  instanceInitfaust_p000SIG0(sig0, sample_rate);
  fillfaust_p000SIG0(sig0, 65536, ftbl0faust_p000SIG0);
  deletefaust_p000SIG0(sig0);
}

void instanceResetUserInterfacefaust_p000(faust_p000* dsp) {
  dsp->freq = (double)(0.0);
  dsp->patch_volume = (double)(0.0);
}

void instanceClearfaust_p000(faust_p000* dsp) {
  
  {
    int l2;
    for (l2 = 0; l2 < 2; l2 = l2 + 1) {
      dsp->iVec1[l2] = 0;
    }
  }
  
  {
    int l3;
    for (l3 = 0; l3 < 2; l3 = l3 + 1) {
      dsp->fRec1[l3] = 0.0;
    }
  }
}

void instanceConstantsfaust_p000(faust_p000* dsp, int sample_rate) {
  dsp->fSampleRate = sample_rate;
  dsp->fConst0 = 1.0 / (double)(dsp->fSampleRate);
}
  
void instanceInitfaust_p000(faust_p000* dsp, int sample_rate) {
  instanceConstantsfaust_p000(dsp, sample_rate);
  instanceResetUserInterfacefaust_p000(dsp);
  instanceClearfaust_p000(dsp);
}

void initfaust_p000(faust_p000* dsp, int sample_rate) {
  classInitfaust_p000(sample_rate);
  instanceInitfaust_p000(dsp, sample_rate);
}

void framefaust_p000(faust_p000* dsp, double* __restrict__ inputs, double* __restrict__ outputs) {
  double fSlow0 = dsp->fConst0 * (double)(dsp->freq);
  double fSlow1 = 0.5 * (double)(dsp->patch_volume);
  dsp->iVec1[0] = 1;
  double fTemp0 = ((1 - dsp->iVec1[1]) ? 0.0 : fSlow0 + dsp->fRec1[1]);
  dsp->fRec1[0] = fTemp0 - floor(fTemp0);
  outputs[0] = (double)(fSlow1 * ftbl0faust_p000SIG0[(int)(65536.0 * dsp->fRec1[0])]);
  dsp->iVec1[1] = dsp->iVec1[0];
  dsp->fRec1[1] = dsp->fRec1[0];
}



typedef struct {
  double l;
  double r;
} StereoOut;

typedef struct {
  int pitch;
  faust_p000 faust;
  TabPlay rel_env;
  bool active;
  bool release;
} Voice;

static Voice voice_init(int pitch) {
  Voice v = {0};
  v.rel_env = tabplay_init(768000);
  initfaust_p000(&v.faust, 768000);
  v.faust.freq = midipitch2freq(pitch);
  v.rel_env.s = 0.05;
  v.rel_env.wt = et_fall_lin;

  v.pitch = pitch;
  v.active = true;
  return v;
}

typedef struct {
  const clap_host_t* host;
  Voice voices[16];
  double patch_volume;

  FixedBLP8 fixed_lpf_l;
  FixedBLP8 fixed_lpf_r;
} p000;

static StereoOut voice_tick(Voice* v, p000* p) {
  StereoOut so = { 0 };
  
  v->faust.patch_volume = p->patch_volume;

  framefaust_p000(&v->faust, 0, &so.l);
  so.r = so.l;

  if (v->release) {
    auto rel = tabplay_tick(&v->rel_env);
    so.l *= rel;
    so.r *= rel;
    if (v->rel_env.phase == 1) { v->active = false; }
  }
  return so;
}

static void add_voice_at_pitch(p000* p, int pitch) {
  for (int i = 0; i < 16; i++) {
    if (!p->voices[i].active) {
      p->voices[i] = voice_init(pitch);
      break;
    }
  }
}

static void release_voice_at_pitch(p000* p, int pitch) {
  for (int i = 0; i < 16; i++) {
    if (p->voices[i].active && p->voices[i].pitch == pitch) {
      p->voices[i].release = true;
      break;
    }
  }
}

StereoOut p000_tick(p000* p) {
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
  out.l *= 0.25;
  out.r *= 0.25;
  return out;
}

static uint32_t plugin_params_count(const clap_plugin_t* plugin) { return 1; }

static bool plugin_params_get_info(const clap_plugin_t* plugin, uint32_t param_index, clap_param_info_t* param_info) {
  switch (param_index) {
    case 0: {
      param_info->id = 1;
      param_info->min_value = 0;
      param_info->max_value = 1;
      param_info->default_value = 1;
      snprintf(param_info->name, sizeof(param_info->name), "%s", "patch_volume");
    } break;

  }

  return true;
}

static void plugin_params_flush(const clap_plugin_t *plugin, const clap_input_events_t *in, const clap_output_events_t *out) {
  int param_ids[1] = { 1 };
  double param_def_vals[1] = { 1 };

  for (int i = 0; i < 1; i++) {
    clap_event_param_value_t event = {0};
    event.header.size = sizeof(event);
    event.header.type = CLAP_EVENT_PARAM_VALUE;
    event.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
    event.header.flags = 0;
    event.header.time = 0;
    event.param_id = param_ids[i];
    event.value = param_def_vals[i];
    out->try_push(out, &event.header);
  }
}

static const clap_plugin_params_t plugin_params = {
  .count = plugin_params_count,
  .flush = plugin_params_flush,
  .get_info = plugin_params_get_info,
  .get_value = default_plugin_params_get_value,
  .text_to_value = default_plugin_params_text_to_value,
  .value_to_text = default_plugin_params_value_to_text,
};

static bool plugin_init(const struct clap_plugin* plugin) {
  p000* p = plugin->plugin_data;
  p->fixed_lpf_l = fixedblp8_init(768000, 13000);
  p->fixed_lpf_r = fixedblp8_init(768000, 13000);
  return true;
}

static bool plugin_activate(const struct clap_plugin *plugin, double sample_rate, uint32_t min_frames_count, uint32_t max_frames_count) {
  p000* p = plugin->plugin_data;

  p->patch_volume = 1;

  const clap_host_params_t* host_params = (const clap_host_params_t*)p->host->get_extension(p->host, CLAP_EXT_PARAMS);
  host_params->request_flush(p->host);
  return true;
}

static void plugin_destroy(const struct clap_plugin* plugin) {
  free((p000*)plugin->plugin_data);
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
        add_voice_at_pitch((p000*)plugin->plugin_data, ev->key);
      } else if (hdr->type == CLAP_EVENT_NOTE_OFF) {
        const clap_event_note_t *ev = (const clap_event_note_t*)hdr;
        release_voice_at_pitch((p000*)plugin->plugin_data, ev->key);
      } else if (hdr->type == CLAP_EVENT_PARAM_VALUE) {
        const clap_event_param_value_t *ev = (const clap_event_param_value_t*)hdr;
        p000* p = plugin->plugin_data;
        double val = ev->value;
        switch (ev->param_id) {
          case 1: {     
            p->patch_volume = val;
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

      StereoOut out = p000_tick((p000*)plugin->plugin_data);
      
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

const clap_plugin_t* p000_create(const clap_plugin_descriptor_t* plugindesc, const clap_host_t* host) {
  clap_plugin_t* plugin = (clap_plugin_t*)calloc(1, sizeof(*plugin));
  plugin->desc = plugindesc;
  plugin->init = plugin_init;
  plugin->destroy = plugin_destroy;
  plugin->activate = plugin_activate;
  plugin->deactivate = default_plugin_deactivate;
  plugin->start_processing = default_plugin_start_processing;
  plugin->stop_processing = default_plugin_stop_processing;
  plugin->reset = default_plugin_reset;
  plugin->process = plugin_process;
  plugin->get_extension = plugin_get_extension;
  plugin->on_main_thread = default_plugin_on_main_thread;
  p000* data = calloc(1, sizeof(*data));
  plugin->plugin_data = data;
  data->host = host;

  return plugin;
}

