#include "plugindescs.h"

static const char* instrument_stereo[] = { CLAP_PLUGIN_FEATURE_INSTRUMENT, CLAP_PLUGIN_FEATURE_STEREO, nullptr };
static const char* instrument_mono[] = { CLAP_PLUGIN_FEATURE_INSTRUMENT, CLAP_PLUGIN_FEATURE_MONO, nullptr };
static const char* effect_stereo[] = { CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, CLAP_PLUGIN_FEATURE_STEREO, nullptr };
static const char* effect_mono[] = { CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, CLAP_PLUGIN_FEATURE_STEREO, nullptr };

#define PLUGIN(id, name, type) \
{ { CLAP_VERSION_INIT, "com.nevrofon.P" #id, "P" #id " - " #name, "Nevrofon", "", "", "", "", "", type }, create_P ## id }, \

typedef struct plugin {
  clap_plugin_descriptor_t desc;
  clap_plugin_t*(*create)(const clap_plugin_descriptor_t*);
} plugin_t;

int plugin_id_to_index(const char* id) {
  return id[16] - '0';
}

#define X(id, name, type) extern clap_plugin_t* create_P ## id(const clap_plugin_descriptor_t*);
#include "plugins.def"
#undef X

#define X(id, name, type) PLUGIN(id, name, type)
plugin_t plugins[] = {
  #include "plugins.def"
};
#undef X

int get_plugin_count() {
  return 5;
}

const clap_plugin_descriptor_t* get_plugindesc_by_index(int index) {
  return &plugins[index].desc;
}

const plugin_t* get_plugin_by_index(int index) {
  return &plugins[index];
}

clap_plugin_t* create_plugin(const char* plugin_id) {
  const plugin_t* p = get_plugin_by_index(plugin_id_to_index(plugin_id));
  return p->create(&p->desc);
}
