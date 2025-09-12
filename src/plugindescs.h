#pragma once

#include "../clap/include/clap/plugin.h"

int get_plugin_count();
const clap_plugin_descriptor_t* get_plugindesc_by_index(int index);
clap_plugin_t* create_plugin(const char* plugin_id, const clap_host_t* host);

