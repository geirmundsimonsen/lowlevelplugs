use std::time::Instant;

use crate::{compile::{compile_and_run_tables, compile_binary_and_copy, compile_c_file}, global::Global, pluginmodel::write_plugindesc_c_file};

mod global;
mod error;
mod shell;
mod pluginmodel;
mod faust;
mod cgen;
mod compile;
mod p000;
mod p005;

fn main() {
  if let Err(e) = run() {
    eprintln!("{e}");
    std::process::exit(1);
  }
}

fn run() -> Result<(), error::AppError> {
  let g = Global::new("/p1/code/c/lowlevelplugs");
  let mut plugins = vec![];

  let p000 = p000::model(); plugins.push(&p000);
  let p005 = p005::model(); plugins.push(&p005);
  
  let start = Instant::now();
  write_plugindesc_c_file(&g, &plugins)?;
  //p000.generate_c_file(&g)?;
  p005.generate_c_file(&g)?;
  let elapsed = start.elapsed();
  println!("generating files: {} ms", elapsed.as_millis());

  compile_and_run_tables(&g)?;
  compile_c_file(&g, "clap_default_fns")?;
  compile_c_file(&g, "log")?;
  compile_c_file(&g, "factory")?;
  compile_c_file(&g, "plugindescs")?;
  compile_c_file(&g, "tables")?;
  compile_c_file(&g, "osc")?;
  compile_c_file(&g, "filter")?;
  compile_c_file(&g, "util")?;
  p000.compile_c_file(&g)?;
  p005.compile_c_file(&g)?;
  compile_binary_and_copy(&g, &plugins)?;

  Ok(())
}
