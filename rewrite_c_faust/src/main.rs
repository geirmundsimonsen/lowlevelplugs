
fn main() {
  let mut args = std::env::args();
  args.next();
  let c_filename = args.next().unwrap();
  let src_dir = args.next().unwrap();
  let out_dir = args.next().unwrap();
  let mut c_file = String::from_utf8(std::fs::read(src_dir + "/" + &c_filename).unwrap()).unwrap();

  let mut start_idx = 0;
  let mut add_horiz_vec: Vec<String> = Vec::new();

  loop {
    if let Some(idx) = c_file[start_idx..].find("ui_interface->addHorizontalSlider") {
      let end_idx = c_file[start_idx+idx..].find("\n").unwrap();
      add_horiz_vec.push(c_file[start_idx+idx..start_idx+idx+end_idx].to_string());
      start_idx = start_idx+idx+end_idx;
    } else {
      break;
    }
  }

  for add_horiz in &add_horiz_vec {
    let idx = add_horiz.find("\"").unwrap();
    let end_idx = add_horiz[idx+1..].find("\"").unwrap();
    let param_name = &add_horiz[idx+1..idx+1+end_idx];
    let idx = add_horiz.find("fHslider").unwrap();
    let end_idx = add_horiz[idx..].find(",").unwrap();
    let h_slider = &add_horiz[idx..idx+end_idx];

    c_file = c_file.replace(h_slider, param_name);
  }
  
  loop {
    if let Some(idx) = c_file.find("#") {
      let end_idx = c_file[idx..].find("\n").unwrap();
      c_file.replace_range(idx..=idx+end_idx, "");
    } else {
      break
    }
  }

  loop {
    if let Some(idx) = c_file.find("/*") {
      let end_idx = c_file[idx..].find("*/").unwrap();
      c_file.replace_range(idx..=idx+end_idx+1, "");
    } else {
      break
    }
  }

  let idx = c_file.find("extern").unwrap();
  let end_idx = c_file[idx..].find("\n").unwrap();
  c_file.replace_range(idx..=idx+end_idx, "");

  let idx = c_file.find("void metadata").unwrap();
  let end_idx = c_file[idx..].find("}").unwrap();
  c_file.replace_range(idx..=idx+end_idx, "");

  let idx = c_file.find("void buildUserInterface").unwrap();
  let end_idx = c_file[idx..].find("}").unwrap();
  c_file.replace_range(idx..=idx+end_idx, "");

  let idx = c_file.find("void computemydsp").unwrap();
  c_file.replace_range(idx.., "");

  c_file = c_file.replace("fminl(1.92e+05L, fmaxl(1.0L, (quad)(dsp->fSampleRate)))", "(quad)(dsp->fSampleRate)");
  c_file = c_file.replace("fmin(1.92e+05, fmax(1.0, (double)(dsp->fSampleRate)))", "(double)(dsp->fSampleRate)");

  c_file = c_file.replace("RESTRICT", "__restrict__");
  c_file = c_file.replace("FAUSTFLOAT", "double");

  std::fs::write(out_dir + "/" + &c_filename, c_file).unwrap();
}
