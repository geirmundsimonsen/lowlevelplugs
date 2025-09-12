set -euo pipefail

cd llpgen
cargo run
reaper /p1/audio/reaper/testclapplugin/testclapplugin.RPP