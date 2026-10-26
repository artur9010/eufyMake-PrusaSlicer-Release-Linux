# Building M5 FDM Studio on Linux

The Linux build uses GTK 3 and WebKitGTK 4.1. It is tested on x86_64 with GCC 16 and
CMake 4; CI also builds an aarch64 (arm64) Flatpak. Most users should just install the Flatpak from the
[releases page](https://github.com/artur9010/eufyMake-PrusaSlicer-Release-Linux/releases):

```
flatpak install --user M5FDMStudio-x86_64.flatpak   # or M5FDMStudio-aarch64.flatpak
flatpak run io.github.artur9010.M5FDMStudio
```

## 0. Prerequisites

### Debian / Ubuntu

```
sudo apt install build-essential cmake ninja-build git gettext pkg-config \
    autoconf automake libtool m4 \
    libgtk-3-dev libwebkit2gtk-4.1-dev libsoup-3.0-dev \
    libavcodec-dev libavutil-dev libswscale-dev \
    libjansson-dev libssl-dev libcurl4-openssl-dev zlib1g-dev \
    liblzma-dev libzstd-dev libbz2-dev libdbus-1-dev \
    libgl-dev libglu1-mesa-dev libegl-dev libwayland-dev libxkbcommon-dev \
    libfontconfig-dev libfreetype-dev libsecret-1-dev libudev-dev
```

FFmpeg (`libavcodec`) decodes the printer's camera stream, and needs its H.264 decoder,
which distribution builds include.

### NixOS / Nix

`AnkerStudio/shell.nix` provides everything:

```
nix-shell AnkerStudio/shell.nix
```

Run every command below inside that shell.

## 1. Compile the dependencies

Boost, wxWidgets, OCCT, CGAL, OpenVDB, TBB and the other bundled libraries are built as
static libraries by the dependency superbuild. curl and OpenSSL come from the system.

```
cd AnkerStudio
cmake -S deps -B deps/build -DDEP_WX_GTK3=ON
cmake --build deps/build -j1
```

`-j1` applies only to the top level; each dependency builds itself in parallel. This takes
a while. The result lands in `deps/build/destdir/usr/local`.

## 2. Compile the application

```
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DSLIC3R_STATIC=1 \
    -DSLIC3R_GTK=3 \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DCMAKE_PREFIX_PATH=$PWD/deps/build/destdir/usr/local
cmake --build build
```

## 3. Run

```
GDK_BACKEND=x11 build/src/m5-fdm-studio
```

wxWidgets is built without EGL, so the 3D view needs X11. On a Wayland desktop that means
XWayland, which `GDK_BACKEND=x11` selects.

The network plugin (`libAnkerNet.so`) is built next to the executable and loaded from
there, so no plugin download is needed.

## Installing

To install into a prefix with the standard Linux directory layout (resources under
`share/`, the plugin under `lib/`), configure with `-DSLIC3R_FHS=1` and
`-DCMAKE_INSTALL_PREFIX=<prefix>`, then run `cmake --install build`.

## Building the Flatpak

The manifest is `AnkerStudio/pack/flatpak/io.github.artur9010.M5FDMStudio.yml`. It
builds on the GNOME 51 runtime and needs network access while building, because the
dependency superbuild downloads its own sources. From the repository root:

```
flatpak install --user flathub org.gnome.Sdk//51 org.gnome.Platform//51
flatpak-builder --user --force-clean --repo=repo build-dir \
    AnkerStudio/pack/flatpak/io.github.artur9010.M5FDMStudio.yml
flatpak build-bundle repo M5FDMStudio.flatpak io.github.artur9010.M5FDMStudio
```

GitHub Actions builds the bundle for x86_64 and aarch64 on every push (`.github/workflows/linux.yml`) and
attaches it to releases for `v*` tags.
