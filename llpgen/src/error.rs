use crate::shell::ShellError;

#[derive(Debug)]
#[allow(dead_code)]
pub enum AppError {
  Shell(ShellError),
}

impl From<ShellError> for AppError {
  fn from(e: ShellError) -> Self { AppError::Shell(e) }
}

impl std::fmt::Display for AppError {
  fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
    match self {
      AppError::Shell(e) => write!(f, "{}", e),
    }
  }
}