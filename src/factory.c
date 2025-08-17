#include "factory.h"

#include "../clap/include/clap/factory/plugin-factory.h"
#include "log.h"
#include "plugindescs.h"

uint32_t plugin_factory_get_plugin_count(const struct clap_plugin_factory* factory) {
  write_log("factory_get_plugin_count");
  return get_plugin_count();
}

const clap_plugin_descriptor_t* plugin_factory_get_plugin_descriptor(const struct clap_plugin_factory* factory, uint32_t index) {
  write_log("factory_get_plugin_descriptor");
  return get_plugindesc_by_index(index);
}

const clap_plugin_t* plugin_factory_create_plugin(const struct clap_plugin_factory* factory, const clap_host_t* host, const char* plugin_id) {
  write_log("factory_create_plugin");
  write_log(plugin_id);
  return create_plugin(plugin_id);
}

const clap_plugin_factory_t plugin_factory = {
  plugin_factory_get_plugin_count,
  plugin_factory_get_plugin_descriptor,
  plugin_factory_create_plugin,
};

const void* get_factory_pointer() {
  return &plugin_factory;
}