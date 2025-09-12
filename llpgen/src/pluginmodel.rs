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

#[derive(Copy, Clone)]
pub struct Oversampling(pub i32);

impl std::fmt::Display for Oversampling {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.0)
    }
}

#[derive(Copy, Clone)]
pub struct Voices(pub i32);

impl std::fmt::Display for Voices {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.0)
    }
}

impl std::ops::Mul<i32> for Oversampling {
    type Output = i32;
    fn mul(self, rhs: i32) -> i32 {
        self.0 * rhs
    }
}

impl std::ops::Mul<Oversampling> for i32 {
    type Output = i32;
    fn mul(self, rhs: Oversampling) -> i32 {
        self * rhs.0
    }
}

pub struct PluginModel {
  pub id: String,
  pub plugin_id: String,
  pub name: String,
  pub feature: Feature, 
  pub voices: Voices,
  pub oversample: Oversampling,
  pub faust_source: String,
  pub params: std::vec::Vec<Param>
}

impl PluginModel {
  pub fn new(id: &str, name: &str, feature: Feature, voices: Voices, oversample: Oversampling, faust_source: &str, params: std::vec::Vec<Param>) -> PluginModel {
    PluginModel { id: id.to_string(), name: name.to_string(), plugin_id: format!("com.nevrofon.{}", id), feature, voices, oversample, faust_source: faust_source.to_string(), params }
  }

  pub fn generate_c_file(&self, g: &Global) -> Result<(), ShellError> {
    let mut faust_dsp = File::create("faust.dsp").expect("Error creating file");
    write!(faust_dsp, "{}", faust::add_imports_and_params(self)).expect("Error writing to file");
    faust_dsp.flush().expect("Failed to flush");
    
    let faust_c = cmd(None, &format!("faust -lang c -double -os -cn faust_{id} faust.dsp", id = self.id))?;
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