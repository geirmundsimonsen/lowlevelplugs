use crate::{global::Global, pluginmodel::PluginModel, shell::{cmd, ShellError}};
use std::time::Instant;

static COMPILER: &'static str = "gcc";
static FLAGS: &'static str = "-std=c23 -O2 -fPIC -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wpedantic";

pub fn compile_c_file(g: &Global, file: &str) -> Result<(), ShellError> {

  let start = Instant::now();
  cmd(None, &format!("{COMPILER} {FLAGS} -c {src}/{file}.c -o {dist}/{file}.o",
    src = g.src_dir,
    dist = g.dist_dir))?;
  let elapsed = start.elapsed();
  println!("compiling {}: {} ms", file, elapsed.as_millis());

  Ok(())
}

pub fn compile_binary_and_copy(g: &Global, plugins: &[&PluginModel]) -> Result<(), ShellError> {
  let binary = "lowlevelplugs.clap";

  let mut object_files: Vec<&str> = vec!["log", "factory", "plugindescs", "tables", "osc", "filter", "util"];
  for model in plugins {
    object_files.push(&model.id)
  }

  object_files.iter().map(|obj_file| format!("{dist}/{obj_file}.o", dist = g.dist_dir)).collect::<Vec<String>>().join(" ");

  let start = Instant::now();
  cmd(None, &format!("{COMPILER} {FLAGS} -shared {src}/main.c -Wl,--no-undefined -o {dist}/{binary} -lm {}",
    object_files.iter().map(|obj_file| format!("{dist}/{obj_file}.o", dist = g.dist_dir)).collect::<Vec<String>>().join(" "),
    src = g.src_dir,
    dist = g.dist_dir
  ))?;
  cmd(None, &format!("sudo cp {dist}/{binary} {}", g.clap_plugin_dir, dist = g.dist_dir))?;
  
  let elapsed = start.elapsed();
  println!("compiling and copying binary: {} ms", elapsed.as_millis());

  Ok(())
}

pub fn compile_and_run_tables(g: &Global) -> Result<(), ShellError> {
  let start = Instant::now();
  cmd(None, &format!("{COMPILER} {FLAGS} {base}/tables/main.c -Wl,--no-undefined -o {base}/tables/build/tables -lm", base = g.base_dir))?;
  cmd(Some(&g.base_dir), "tables/build/tables")?;
  let elapsed = start.elapsed();
  println!("compile/run tables: {} ms", elapsed.as_millis());
  Ok(())
}