use std::fs::File;
use std::io::Write;

use crate::{cgen, compile::compile_c_file, faust, global::Global, shell::{cmd, ShellError}};

pub struct Param {
  pub id: i32,
  pub min: f64,
  pub max: f64,
  pub default: f64,
  pub name: String,
  //pub path: Option<String>,
  pub transfer_fn: Option<String>,
}

impl Param {
  pub fn new(id: i32, name: &str, min: f64, max: f64, default: f64) -> Param {
    Param { id, min, max, default, name: name.to_string(), /*path: None,*/ transfer_fn: None }
  }

  /*pub fn path(mut self, path: &str) -> Param {
    self.path = Some(path.to_string());
    self
  }*/

  pub fn transfer_fn(mut self, transfer_fn: &str) -> Param {
    self.transfer_fn = Some(transfer_fn.to_string());
    self
  }
}

#[allow(dead_code)]
pub enum Feature {
  InstrumentStereo,
  InstrumentMono,
  EffectStereo,
  EffectMono
}

pub struct PluginModel {
  pub id: String,
  pub plugin_id: String,
  pub name: String,
  pub feature: Feature, 
  pub oversample: i32,
  pub faust_source: String,
  pub params: std::vec::Vec<Param>
}

impl PluginModel {
  pub fn new(id: &str, name: &str, feature: Feature, oversample: i32, faust_source: &str, params: std::vec::Vec<Param>) -> PluginModel {
    PluginModel { id: id.to_string(), name: name.to_string(), plugin_id: format!("com.nevrofon.{}", id), feature, oversample, faust_source: faust_source.to_string(), params }
  }

  pub fn generate_c_file(&self, g: &Global) -> Result<(), ShellError> {
    let mut faust_dsp = File::create("faust.dsp").expect("Error creating file");
    write!(faust_dsp, "{}", faust::add_imports_and_params(self)).expect("Error writing to file");
    faust_dsp.flush().expect("Failed to flush");
    
    let faust_c = cmd(None, "faust -lang c -double -os faust.dsp")?;
    cmd(None, "rm faust.dsp")?;
  
    let faust_c = faust::rewrite(faust_c);
  
    let c = cgen::plugin::create_c_file(g, self, &faust_c);
    let mut c_file = File::create(format!("{}/{}.c", g.src_dir, self.id)).expect("Error creating file");
    write!(c_file, "{}", c).expect("Error writing to file");
  
    Ok(())
  }

  pub fn compile_c_file(&self, g: &Global) -> Result<(), ShellError> {
    compile_c_file(g, &format!("{}", self.id))?;
    
    Ok(())
  }
}

pub fn write_plugindesc_c_file(g: &Global, plugins: &[&PluginModel]) -> Result<(), ShellError> {
  let c = cgen::description::create_c_file(plugins);
  let mut c_file = File::create(format!("{}/plugindescs.c", g.src_dir)).expect("Error creating file");
  write!(c_file, "{}", c).expect("Error writing to file");

  Ok(())
}