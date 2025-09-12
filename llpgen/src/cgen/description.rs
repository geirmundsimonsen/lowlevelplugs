use crate::{pluginmodel::{Feature, PluginModel}};

fn get_plugin_entry_points(models: &[&PluginModel]) -> String {
  let mut p = String::new();
  for model in models {
    p += &format!(r#"extern clap_plugin_t* {id}_create(const clap_plugin_descriptor_t*);"#,
      id = model.id,
    );
  }
  p
}

fn get_plugins(models: &[&PluginModel]) -> String {
  let mut p = String::new();
  for model in models {
    p += &format!(r#"{{ {{ CLAP_VERSION_INIT, "{plugin_id}", "{id} - {name}", "Nevrofon", "", "", "", "", "",  {feature} }}, {id}_create }},"#,
      plugin_id = model.plugin_id, id = model.id, name = model.name,
      feature = match model.feature {
        Feature::InstrumentMono => { "instrument_mono" },
        Feature::InstrumentStereo => { "instrument_stereo" },
        Feature::EffectMono => { "effect_mono" },
        Feature::EffectStereo => { "effect_stereo" },
      }
    );
  }
  p
}

pub fn create_c_file(models: &[&PluginModel]) -> String {
let c_file = format!(r#"
#include "plugindescs.h"

#include <string.h>

static const char* instrument_stereo[] = {{ CLAP_PLUGIN_FEATURE_INSTRUMENT, CLAP_PLUGIN_FEATURE_STEREO, nullptr }};
static const char* instrument_mono[] = {{ CLAP_PLUGIN_FEATURE_INSTRUMENT, CLAP_PLUGIN_FEATURE_MONO, nullptr }};
static const char* effect_stereo[] = {{ CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, CLAP_PLUGIN_FEATURE_STEREO, nullptr }};
static const char* effect_mono[] = {{ CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, CLAP_PLUGIN_FEATURE_STEREO, nullptr }};

typedef struct plugin {{
  clap_plugin_descriptor_t desc;
  clap_plugin_t*(*create)(const clap_plugin_descriptor_t*);
}} plugin_t;

{plugin_entry_points}

plugin_t plugins[] = {{
{plugins}
}};

int get_plugin_count() {{
  return {plugin_count};
}}

const clap_plugin_descriptor_t* get_plugindesc_by_index(int index) {{
  return &plugins[index].desc;
}}

clap_plugin_t* create_plugin(const char* plugin_id) {{
  const plugin_t* p = NULL;

  for (int i = 0; i < {plugin_count}; i++) {{
    if (strcmp(plugin_id, plugins[i].desc.id) == 0) {{
      p = &plugins[i];
      break;
    }}
  }}
  if (!p) return NULL;
  return p->create(&p->desc);
}}

"#,
    plugins = get_plugins(models),
    plugin_entry_points = get_plugin_entry_points(models),
    plugin_count = models.len()
  );

  c_file
}