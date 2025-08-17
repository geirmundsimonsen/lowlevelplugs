#include "../clap/include/clap/entry.h"

#include "log.h"
#include "factory.h"

bool clap_init(const char* plugin_path) {
  clear_log();
  write_log("init");
  return true;
}

void clap_deinit() {
  write_log("deinit");
}

const void* clap_get_factory(const char* factory_id) {
  write_log("get_factory");
  return get_factory_pointer();
}

const clap_plugin_entry_t clap_entry = {
  CLAP_VERSION_INIT,
  clap_init,
  clap_deinit,
  clap_get_factory
};
