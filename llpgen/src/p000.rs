use crate::pluginmodel::{Feature, Oversampling, Param, PluginModel, Voices};

pub fn model() -> PluginModel {
  let faust_source = r#"
  process = os.oscsin(freq) * 0.5 * patch_volume;
  "#;
  
  PluginModel::new("p000", "Sine Test -18db", Feature::InstrumentStereo, Voices(16), Oversampling(16), faust_source, vec![
    Param::new(1, "patch_volume", 0.0, 1.0, 1.0),
  ])
}