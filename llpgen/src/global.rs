pub struct Global {
  pub sample_rate: i32,
  pub base_dir: String,
  pub src_dir: String,
  pub dist_dir: String,
  pub clap_plugin_dir: String
}

impl Global {
  pub fn new(base_dir: &str) -> Global {
    Global {
      sample_rate: 192000,
      base_dir: base_dir.to_string(),
      src_dir: base_dir.to_string() + "/src",
      dist_dir: base_dir.to_string() + "/dist",
      clap_plugin_dir: "/usr/lib/clap".to_string()
    }
  }
}