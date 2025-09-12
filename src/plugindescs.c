
#include "plugindescs.h"

#include <string.h>

static const char* instrument_stereo[] = { CLAP_PLUGIN_FEATURE_INSTRUMENT, CLAP_PLUGIN_FEATURE_STEREO, nullptr };
static const char* instrument_mono[] = { CLAP_PLUGIN_FEATURE_INSTRUMENT, CLAP_PLUGIN_FEATURE_MONO, nullptr };
static const char* effect_stereo[] = { CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, CLAP_PLUGIN_FEATURE_STEREO, nullptr };
static const char* effect_mono[] = { CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, CLAP_PLUGIN_FEATURE_STEREO, nullptr };

typedef struct plugin {
  clap_plugin_descriptor_t desc;
  clap_plugin_t*(*create)(const clap_plugin_descriptor_t*);
} plugin_t;

extern clap_plugin_t* p005_create(const clap_plugin_descriptor_t*);

plugin_t plugins[] = {
{ { CLAP_VERSION_INIT, "com.nevrofon.p005", "p005 - test", "Nevrofon", "", "", "", "", "",  instrument_stereo }, p005_create },
};

int get_plugin_count() {
  return 1;
}

const clap_plugin_descriptor_t* get_plugindesc_by_index(int index) {
  return &plugins[index].desc;
}

clap_plugin_t* create_plugin(const char* plugin_id) {
  const plugin_t* p = NULL;

  for (int i = 0; i < 1; i++) {
    if (strcmp(plugin_id, plugins[i].desc.id) == 0) {
      p = &plugins[i];
      break;
    }
  }
  if (!p) return NULL;
  return p->create(&p->desc);
}

