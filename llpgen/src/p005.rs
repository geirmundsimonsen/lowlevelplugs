use crate::pluginmodel::{Feature, Oversampling, Param, PluginModel, Voices};

pub fn model() -> PluginModel {
  let faust_source = r#"
  process = os.lf_pulsetrain(freq, 0.1) * 0.5 : ve.korg35LPF(lpf_freq, lpf_q) : fi.dcblockerat(2);
  "#;
  
  PluginModel::new("p005", "test", Feature::InstrumentStereo, Voices(16), Oversampling(16), faust_source, vec![
    Param::new(1, "lpf_freq", 0.0, 1.0, 0.0),
    Param::new(2, "lpf_q", 0.0, 1.0, 0.0).transfer_fn("val * 9.3 + 0.707")
  ])
}