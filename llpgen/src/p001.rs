use crate::pluginmodel::{Feature, Oversampling, Param, PluginModel, Voices};

pub fn model() -> PluginModel {
  let faust_source = r#"
  process = no.noise * 0.5 * patch_volume + freq * 0.0000000001;
  "#;
  
  PluginModel::new("p001", "Noise test -18db", Feature::InstrumentStereo, Voices(16), Oversampling(16), faust_source, vec![
    Param::new(1, "patch_volume", 0.0, 1.0, 1.0),
  ])
}