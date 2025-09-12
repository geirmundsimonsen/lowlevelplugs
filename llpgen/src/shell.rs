use std::process::Command;

#[derive(Debug)]
pub struct ShellError {
  pub cmd: String,
  pub stderr: String,
}

impl std::fmt::Display for ShellError {
  fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
    write!(f, "Shell command failed: {}\n{}", self.cmd, self.stderr)
  }
}

impl std::error::Error for ShellError {}

pub fn cmd(current_dir: Option<&str>, cmd: &str) -> Result<String, ShellError> {
  let mut args = cmd.split_ascii_whitespace();
  let mut proc = Command::new(args.next().expect("no cmd"));
  while let Some(arg) = args.next() {
    proc.arg(arg);
  }
  if let Some(dir) = current_dir {
    proc.current_dir(dir);
  }
  let output = proc.output().unwrap();
  if !output.status.success() {
    let stderr = String::from_utf8_lossy(&output.stderr).to_string();
    return Err(ShellError { cmd: cmd.to_string(), stderr });
  }
  Ok(String::from_utf8_lossy(&output.stdout).to_string())
}