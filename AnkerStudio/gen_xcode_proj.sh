#!/bin/bash
#
# Generate the Xcode project in build_xcode/.
#
# Environment knobs (all optional):
#   ARCH=arm64|x86_64              target architecture (default: arm64). The deps in
#                                  deps/deps_build must have been built for the same
#                                  architecture, i.e. on a Mac of that architecture.
#   MACOS_DEPLOYMENT_TARGET=11.0   minimum macOS version (default: Xcode's default, the
#                                  SDK version)
# Any extra arguments are passed through to cmake, e.g.
#   ./gen_xcode_proj.sh -DOPENSSL_ROOT_DIR="$(brew --prefix openssl@3)"
set -e

ARCH="${ARCH:-arm64}"
case "$ARCH" in
    arm64|x86_64) ;;
    *) echo "unsupported ARCH '$ARCH' (use arm64 or x86_64)" >&2; exit 1 ;;
esac

# Resolve paths relative to this script so the tree can live in any directory
# (e.g. a fresh fork clone) without editing hardcoded paths.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
mkdir -p "$SCRIPT_DIR/build_xcode"
cd "$SCRIPT_DIR/build_xcode"
deps_dir="$SCRIPT_DIR/deps/deps_build/destdir"
export THIRD_PART_ROOT=$deps_dir

extra_args=()
if [ -n "${MACOS_DEPLOYMENT_TARGET:-}" ]; then
    extra_args+=("-DCMAKE_OSX_DEPLOYMENT_TARGET=$MACOS_DEPLOYMENT_TARGET")
fi

cmake .. -GXcode -DCMAKE_PREFIX_PATH="$deps_dir/usr/local" -DSLIC3R_STATIC=1 \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_OSX_ARCHITECTURES="$ARCH" \
    ${extra_args[@]+"${extra_args[@]}"} "$@"

crt_gd_ca_path="$(pwd)/../resources/crt/GD_CA.crt"
make_us_qa_path="$(pwd)/../resources/crt/make-us-qa.crt"
make_us_path="$(pwd)/../resources/crt/make-us.crt"
exec_path="$(pwd)/../build_xcode/src/Debug"
# Ensure the output dir exists so the cp's below create files *inside* it rather
# than a file literally named "Debug" (which then breaks linking of libAnkerNet).
mkdir -p "$exec_path"
cp "$crt_gd_ca_path" "$exec_path"
cp "$make_us_qa_path" "$exec_path"
cp "$make_us_path" "$exec_path"
