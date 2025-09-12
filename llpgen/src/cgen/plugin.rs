use crate::{global::Global, pluginmodel::PluginModel};

fn get_param_info(model: &PluginModel) -> String {
  let mut p = String::new();

  let mut idx = 0;
  for param in &model.params {
    p += &format!(r#"    case {idx}: {{
      param_info->id = {id};
      param_info->min_value = {min};
      param_info->max_value = {max};
      param_info->default_value = {default};
      snprintf(param_info->name, sizeof(param_info->name), "%s", "{name}");
    }} break;
"#, id = param.id, min = param.min, max = param.max, default = param.default, name = param.name);

    idx += 1;
  }

  p
}

fn get_param_decl_for_plugin(model: &PluginModel) -> String {
  let mut p = String::new();

  for param in &model.params {    
    p += &format!("  double {name};\n", name = param.name);
  }

  p
}

fn get_plugin_params_to_faust(model: &PluginModel) -> String {
  let mut p = String::new();

  for param in &model.params {    
    p += &format!("  v->faust.{name} = p->{name};\n", name = param.name);
  }

  p
}

fn get_update_params(model: &PluginModel) -> String {
  let mut p = String::new();

  for param in &model.params {
    let val = param.transfer_fn.clone().unwrap_or("val".to_string());
    
    p += &format!(r#"          case {id}: {{     
            p->{name} = {val};
          }} break;
"#, id = param.id, name = param.name);
  }

  p
}

fn get_default_val_for_param(model: &PluginModel) -> String {
  let mut p = String::new();

  for param in &model.params {    
    p += &format!("  p->{name} = {def_val};\n", name = param.name, def_val = param.default);
  }

  p
}

pub fn create_c_file(g: &Global, model: &PluginModel, faust: &String) -> String {
  let c_file = format!(r#"
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

{faust}

typedef struct {{
  double l;
  double r;
}} StereoOut;

typedef struct {{
  int pitch;
  faust_{plg} faust;
  TabPlay rel_env;
  bool active;
  bool release;
}} Voice;

static Voice voice_init(int pitch) {{
  Voice v = {{0}};
  v.rel_env = tabplay_init({final_sr});
  initfaust_{plg}(&v.faust, {final_sr});
  v.faust.freq = midipitch2freq(pitch);
  v.rel_env.s = 0.05;
  v.rel_env.wt = et_fall_lin;

  v.pitch = pitch;
  v.active = true;
  return v;
}}

typedef struct {{
  const clap_host_t* host;
  Voice voices[{voices}];
{param_decl_for_plugin}
  FixedBLP8 fixed_lpf_l;
  FixedBLP8 fixed_lpf_r;
}} {plg};

static StereoOut voice_tick(Voice* v, {plg}* p) {{
  StereoOut so = {{ 0 }};
  
{plugin_params_to_faust}
  framefaust_{plg}(&v->faust, 0, &so.l);
  so.r = so.l;

  if (v->release) {{
    auto rel = tabplay_tick(&v->rel_env);
    so.l *= rel;
    so.r *= rel;
    if (v->rel_env.phase == 1) {{ v->active = false; }}
  }}
  return so;
}}

static void add_voice_at_pitch({plg}* p, int pitch) {{
  for (int i = 0; i < {voices}; i++) {{
    if (!p->voices[i].active) {{
      p->voices[i] = voice_init(pitch);
      break;
    }}
  }}
}}

static void release_voice_at_pitch({plg}* p, int pitch) {{
  for (int i = 0; i < {voices}; i++) {{
    if (p->voices[i].active && p->voices[i].pitch == pitch) {{
      p->voices[i].release = true;
      break;
    }}
  }}
}}

StereoOut {plg_tick}({plg}* p) {{
  StereoOut out = {{ 0 }};
  for (int i = 0; i < {oversample}; i++) {{ // oversampling block
    out.l = 0;
    out.r = 0;
    for (int v = 0; v < {voices}; v++) {{
      if (p->voices[v].active) {{
        StereoOut voiceOut;
        voiceOut = voice_tick(&p->voices[v], p);
        
        out.l += voiceOut.l;
        out.r += voiceOut.r;
      }}
    }}
    p->fixed_lpf_l.in = out.l;
    p->fixed_lpf_r.in = out.r;
    out.l = fixedblp8_tick(&p->fixed_lpf_l);
    out.r = fixedblp8_tick(&p->fixed_lpf_r);
  }}
  out.l *= 0.25;
  out.r *= 0.25;
  return out;
}}

static uint32_t plugin_params_count(const clap_plugin_t* plugin) {{ return {param_count}; }}

static bool plugin_params_get_info(const clap_plugin_t* plugin, uint32_t param_index, clap_param_info_t* param_info) {{
  switch (param_index) {{
{param_info}
  }}

  return true;
}}

static void plugin_params_flush(const clap_plugin_t *plugin, const clap_input_events_t *in, const clap_output_events_t *out) {{
  int param_ids[{param_count}] = {{ {list_of_param_ids} }};
  double param_def_vals[{param_count}] = {{ {list_of_param_def_vals} }};

  for (int i = 0; i < {param_count}; i++) {{
    clap_event_param_value_t event = {{0}};
    event.header.size = sizeof(event);
    event.header.type = CLAP_EVENT_PARAM_VALUE;
    event.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
    event.header.flags = 0;
    event.header.time = 0;
    event.param_id = param_ids[i];
    event.value = param_def_vals[i];
    out->try_push(out, &event.header);
  }}
}}

static const clap_plugin_params_t plugin_params = {{
  .count = plugin_params_count,
  .flush = plugin_params_flush,
  .get_info = plugin_params_get_info,
  .get_value = default_plugin_params_get_value,
  .text_to_value = default_plugin_params_text_to_value,
  .value_to_text = default_plugin_params_value_to_text,
}};

static bool plugin_init(const struct clap_plugin* plugin) {{
  {plg}* p = plugin->plugin_data;
  p->fixed_lpf_l = fixedblp8_init({final_sr}, 13000);
  p->fixed_lpf_r = fixedblp8_init({final_sr}, 13000);
  return true;
}}

static bool plugin_activate(const struct clap_plugin *plugin, double sample_rate, uint32_t min_frames_count, uint32_t max_frames_count) {{
  {plg}* p = plugin->plugin_data;

{default_val_for_param}
  const clap_host_params_t* host_params = (const clap_host_params_t*)p->host->get_extension(p->host, CLAP_EXT_PARAMS);
  host_params->request_flush(p->host);
  return true;
}}

static void plugin_destroy(const struct clap_plugin* plugin) {{
  free(({plg}*)plugin->plugin_data);
  free((struct clap_plugin*)plugin);
}}

static clap_process_status plugin_process(const struct clap_plugin* plugin, const clap_process_t* process) {{
  const uint32_t nframes = process->frames_count;
  const uint32_t num_ev = process->in_events->size(process->in_events);
  uint32_t       ev_index = 0;
  uint32_t       next_ev_frame = num_ev > 0 ? 0 : nframes;

  for (uint32_t i = 0; i < nframes;) {{
    while (ev_index < num_ev && next_ev_frame == i) {{
      const clap_event_header_t *hdr = process->in_events->get(process->in_events, ev_index);
      if (hdr->time != i) {{
        next_ev_frame = hdr->time;
        break;
      }} 
      
      if (hdr->type == CLAP_EVENT_NOTE_ON) {{
        const clap_event_note_t *ev = (const clap_event_note_t*)hdr;
        add_voice_at_pitch(({plg}*)plugin->plugin_data, ev->key);
      }} else if (hdr->type == CLAP_EVENT_NOTE_OFF) {{
        const clap_event_note_t *ev = (const clap_event_note_t*)hdr;
        release_voice_at_pitch(({plg}*)plugin->plugin_data, ev->key);
      }} else if (hdr->type == CLAP_EVENT_PARAM_VALUE) {{
        const clap_event_param_value_t *ev = (const clap_event_param_value_t*)hdr;
        {plg}* p = plugin->plugin_data;
        double val = ev->value;
        switch (ev->param_id) {{
{update_params}
        }}
      }} else if (hdr->type == CLAP_EVENT_PARAM_MOD) {{
        const clap_event_param_mod_t *ev = (const clap_event_param_mod_t*)hdr;
      }}

      ev_index++;
      if (ev_index == num_ev) {{
        next_ev_frame = nframes;
        break;
      }}
    }}

    for (; i < next_ev_frame; ++i) {{
      float L = process->audio_inputs[0].data32[0][i];
      float R = process->audio_inputs[0].data32[1][i];

      StereoOut out = {plg_tick}(({plg}*)plugin->plugin_data);
      
      L += out.l;
      R += out.r;

      process->audio_outputs[0].data32[0][i] = L;
      process->audio_outputs[0].data32[1][i] = R;
    }}
  }}

  return CLAP_PROCESS_CONTINUE;
}}

static const void *plugin_get_extension(const struct clap_plugin *plugin, const char *id) {{
  if (!strcmp(id, CLAP_EXT_AUDIO_PORTS)) {{ return &one_stereo_audio_port; }}
  if (!strcmp(id, CLAP_EXT_NOTE_PORTS)) {{ return &one_note_port; }}
  if (!strcmp(id, CLAP_EXT_PARAMS)) {{ return &plugin_params; }}
  return NULL;
}}

const clap_plugin_t* {plg_create}(const clap_plugin_descriptor_t* plugindesc, const clap_host_t* host) {{
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
  {plg}* data = calloc(1, sizeof(*data));
  plugin->plugin_data = data;
  data->host = host;

  return plugin;
}}

"#,
  final_sr = g.sample_rate * model.oversample,
  oversample = model.oversample,
  voices = model.voices,
  plg_tick = model.id.to_string() + "_tick",
  plg_create = model.id.to_string() + "_create",
  plg = model.id.to_string(),
  param_count = model.params.len(),
  param_info = get_param_info(model),
  param_decl_for_plugin = get_param_decl_for_plugin(model),
  update_params = get_update_params(model),
  default_val_for_param = get_default_val_for_param(model),
  plugin_params_to_faust = get_plugin_params_to_faust(model),
  list_of_param_ids = model.params.iter().map(|p| p.id.to_string()).collect::<Vec<String>>().join(", "),
  list_of_param_def_vals = model.params.iter().map(|p| p.default.to_string()).collect::<Vec<String>>().join(", ")
  );

  c_file
}

