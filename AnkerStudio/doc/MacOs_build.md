# Building M5 FDM Studio on macOS

The default target is **Apple Silicon (arm64)**. Intel (x86_64) builds work too: build
on an Intel Mac and run `gen_xcode_proj.sh` with `ARCH=x86_64` (step 3). The deps and the
Homebrew libraries are built for the machine's own architecture, so build each
architecture natively; there is no universal build.

Prebuilt DMGs for both architectures come out of CI (`.github/workflows/macos.yml`), see
[Prebuilt DMGs](#prebuilt-dmgs-from-ci) below.

## 0. Prerequisites

- A Mac (Apple Silicon or Intel), macOS 11 or newer
- Xcode from the App Store, plus the command line tools (`xcode-select --install`)
- Homebrew packages:

```
brew install cmake git gettext openssl@3 xz zstd jansson dylibbundler
```

`cmake`, `git`, and `gettext` are needed to build. `openssl@3`, `xz`, `zstd`, and
`jansson` are linked by the application. `dylibbundler` is only needed if you want to
package a distributable `.app` (see step 4).

FFmpeg is **not** required. The camera stream is decoded with VideoToolbox on macOS;
FFmpeg is only used by the Windows build.

## 1. Download sources

Clone the repository. Use a directory relatively close to the drive root, so the path is
not too long. Avoid spaces and non-ASCII characters:

```
mkdir src
cd src

git clone https://github.com/bobbyb/eufyMake-PrusaSlicer-Release-ARM.git
```

## 2. Compile the dependencies

```
cd eufyMake-PrusaSlicer-Release-ARM/AnkerStudio/deps
mkdir deps_build
cd deps_build
cmake ..
make -jN
```

`N` is the number of CPU cores, so for example `make -j8` on an 8-core machine. This takes
a while — it builds Boost, wxWidgets, and the other bundled libraries.

The result lands in `AnkerStudio/deps/deps_build/destdir`, which is where the next step
expects to find it.

## 3. Generate the Xcode project

```
cd ../..          # back to AnkerStudio/
./gen_xcode_proj.sh
```

The script resolves every path relative to its own location, so there is nothing to edit
before running it — a fresh clone in any directory works as-is. It creates
`build_xcode/`, runs CMake with the Xcode generator, and copies the pinned certificates
from `resources/crt/` into the build output directory.

Optional environment variables, and extra arguments passed straight to CMake:

```
ARCH=x86_64 ./gen_xcode_proj.sh                 # Intel Mac (default: ARCH=arm64)
MACOS_DEPLOYMENT_TARGET=11.0 ./gen_xcode_proj.sh  # oldest macOS to support (default: SDK version)
./gen_xcode_proj.sh -DOPENSSL_ROOT_DIR="$(brew --prefix openssl@3)"
```

If you set a deployment target, build the deps with the same one
(`cmake .. -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0`).

## 4. Build and run

Open `AnkerStudio/build_xcode/eufyStudio.xcodeproj` in Xcode, select the **eufyStudio**
scheme, and press Run. Debug and Release both work.

To build from the command line instead:

```
xcodebuild -project build_xcode/eufyStudio.xcodeproj -target eufyStudio -configuration Debug -jobs 8
```

The binary lands in `build_xcode/src/Debug/` (or `Release/`).

### Packaging a distributable app

Running the binary directly out of the build directory works, but it is not an application
bundle, so macOS gives it no `Info.plist` and no menu bar. To assemble a real `.app`, use
`make_app.sh`:

```
CONFIG=Debug DO_NOTARIZE=0 DO_DMG=0 DEV_ID="" ./make_app.sh   # quick local bundle
./make_app.sh                                                 # full: Release, signed, notarized, DMG
```

The full pipeline needs a "Developer ID Application" certificate and notarization
credentials stored once with `xcrun notarytool store-credentials`. See the comments at the
top of `make_app.sh`. Pass `DEV_ID` in the environment rather than editing the script, so
your signing identity is not committed. `DEV_ID="-"` signs ad hoc instead (no
certificate needed, no notarization); that is what CI does.

## Prebuilt DMGs from CI

`.github/workflows/macos.yml` builds `M5FDMStudio-macos-arm64.dmg` and
`M5FDMStudio-macos-x86_64.dmg` on native GitHub runners (`macos-15` and `macos-15-intel`)
with a deployment target of macOS 11. Every push uploads them as workflow artifacts and
`v*` tags attach them to the GitHub release. The deps install tree is cached per
architecture, keyed on the contents of `deps/`.

There is no Developer ID in CI, so the app is only ad-hoc signed and not notarized.
Gatekeeper blocks it on first launch: right-click the app and choose **Open**, or run

```
xattr -dr com.apple.quarantine "/Applications/M5 FDM Studio.app"
```

## Network plugin

**No plugin download is needed, on any architecture.**

Upstream ships the network layer — login, device discovery, telemetry, printing — as a
closed-source `libAnkerNet.dylib` that the app downloads on first run. That library is
x86_64 only, so it cannot load on Apple Silicon.

This fork builds a native replacement from source at
`src/slic3r/GUI/AnkerNetModule/AnkerNetNative.*`. It is compiled as `libAnkerNet.dylib`
alongside the executable and loaded through the same `boost::dll` contract, so nothing
needs to be copied into place and the app never prompts to install a plugin. It is built
automatically; `OPEN_SOURCE` is set `ON` in `CMakeLists.txt` and is not a user-facing
option.

Both Debug and Release work. Upstream's requirement to use Release mode for the network
plugin does not apply here.
