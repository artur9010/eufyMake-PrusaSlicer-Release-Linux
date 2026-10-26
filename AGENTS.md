# AGENTS.md

Notes for anyone (human or AI agent) working on this codebase. Read this before you
change code; it records the non-obvious things that cost time to discover.

## What this is

M5 FDM Studio: a fork of eufyMake Studio (itself a PrusaSlicer 2.x fork) for AnkerMake
M5/M5C printers. This repo is the **Linux fork**
(`git@github.com:artur9010/eufyMake-PrusaSlicer-Release-Linux.git`, remote `origin`), built
on top of the Apple Silicon fork (remote `upstream`, `bobbyb/eufyMake-PrusaSlicer-Release-ARM`).

Supported targets: **macOS arm64/x86_64** and **Linux x86_64/aarch64 (GTK 3)**. Windows is not
built or tested here. The only shipped Linux artifact is a Flatpak bundle.

Everything that matters lives under `AnkerStudio/`:

| Path | What |
|---|---|
| `AnkerStudio/deps/` | Dependency superbuild (Boost, wxWidgets, OCCT, CGAL, OpenVDB, TBB, ...), static libs |
| `AnkerStudio/src/libslic3r/` | Slicing engine (upstream PrusaSlicer, avoid touching) |
| `AnkerStudio/src/slic3r/GUI/` | wxWidgets GUI; Anker additions mostly in `Common/`, `Network/`, `AnkerNetModule/` |
| `AnkerStudio/src/slic3r/GUI/AnkerNetModule/` | Native, open-source replacement for Anker's closed network plugin (MQTT, PPPP P2P video, HTTP, H.264 decode). Protocol notes in `PROTOCOL.md` |
| `AnkerStudio/pack/flatpak/` | Flatpak manifest, metainfo, deps source list generator |
| `AnkerStudio/doc/` | `Linux_build.md`, `MacOs_build.md` (the build guides; keep them current) |
| `.github/workflows/linux.yml` | CI: Flatpak build matrix (x86_64 + aarch64), release upload on `v*` tags |
| `.github/workflows/macos.yml` | CI: DMG build matrix (arm64 + x86_64), release upload on `v*` tags |

## Building on Linux

Full guide: `AnkerStudio/doc/Linux_build.md`. Short version, from `AnkerStudio/`:

```sh
# NixOS: every command below inside this shell (it also sets the runtime env: TLS
# modules, GSettings schemas).
nix-shell shell.nix

# 1. deps (hours the first time; -j1 is only the top level, each dep builds in parallel)
cmake -S deps -B deps/build -DDEP_WX_GTK3=ON
cmake --build deps/build -j1

# 2. app
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    -DSLIC3R_STATIC=1 -DSLIC3R_GTK=3 -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DCMAKE_PREFIX_PATH=$PWD/deps/build/destdir/usr/local
cmake --build build

# 3. run (GDK_BACKEND=x11 is required, see quirks)
GDK_BACKEND=x11 build/src/m5-fdm-studio --datadir /some/scratch/dir
```

Flatpak (from the repo root):

```sh
python3 AnkerStudio/pack/flatpak/gen-deps-sources.py   # after touching anything in deps/
flatpak-builder --user --force-clean --repo=repo build-dir \
    AnkerStudio/pack/flatpak/io.github.artur9010.M5FDMStudio.yml
flatpak build-bundle repo M5FDMStudio.flatpak io.github.artur9010.M5FDMStudio
```

The full Flatpak build needs a lot of RAM (OCCT/CGAL/PCH compile units); on a small
machine it can get OOM-killed. CI is the reference build.

There are no meaningful automated tests (`SLIC3R_BUILD_TESTS` is off). Verify by building
and running the app; a CLI slice (`m5-fdm-studio --export-gcode model.stl`) is a quick
smoke test of the engine.

## Toolchain quirks (current GCC 15/16, CMake 4)

- **CMake 4** rejects old `cmake_minimum_required`; pass `-DCMAKE_POLICY_VERSION_MINIMUM=3.5`
  to the app and to any third-party CMake project (e.g. jansson in the Flatpak).
- **GCC defaults to C23.** Old autoconf projects (GMP, MPFR) fail `configure` with "no
  working compiler"; their CFLAGS pin `-std=gnu17`. Blosc does too, because its non-SSE2
  path (aarch64) typedefs `bool`. Some breakage only shows up on aarch64, so x86_64
  passing locally doesn't prove the arm64 CI job will.
- **Keep PCH on** (`SLIC3R_PCH`, default). Many Anker sources rely on headers that only the
  precompiled header pulls in; building with PCH off produces a flood of missing-include
  errors.
- **Includes are case-sensitive on Linux.** The code was written on case-insensitive
  macOS/Windows; match real filenames exactly (`Common/` not `common/`, `libslic3r/Utils.hpp`,
  `wx/overlay.h`).
- TBB builds with LTO under GCC; its static archive needs `-ffat-lto-objects` or the app link
  fails with undefined TBB symbols.
- GNU ld is order-sensitive for static libs: wx components are listed dependents first
  (`richtext webview gl html adv xml core net base`).
- Patches in `deps/` are applied with `patch -p1`, **not** `git apply`: inside another git
  repo `git apply` silently skips a/b-style patches and reports success. ExternalProject
  patch steps aren't idempotent; if a patch step half-ran, delete `deps/build/dep_<Name>-prefix`.
- Some upstream download hosts are flaky or block CI IPs (gmplib.org blocks GitHub runners,
  the old Boost jfrog URL changed content). Prefer a stable mirror first, keep the hash.

## GTK / wxWidgets quirks

- **wx is built without EGL**, so the 3D canvas needs GLX: always run with
  `GDK_BACKEND=x11` (the Flatpak sets it; on Wayland this means XWayland).
- **`wxButton` and `wxListBox` cannot host child windows on GTK**; adding one crashes in
  `wxWindow::DoAddChild`. The Anker UI does this a lot on macOS/Windows. Pattern used here:
  on `__WXGTK__` derive from `wxPanel` instead (see `Common/AnkerCompositeButtonBase.hpp`
  and `AnkerChooseDeviceListBoxBase` in `Network/chooseDeviceFrame.h`), and keep the API the
  rest of the code uses.
- **`wxStaticText`/`wxStaticBitmap` have no native window on GTK**, so mouse events go to
  the parent. Panel-based "buttons" must bind `wxEVT_LEFT_DOWN` on themselves and emit
  `wxEVT_BUTTON`.
- wx is the Prusa wx 3.2 fork, patched for WebKitGTK 4.1 / libsoup 3
  (`deps/wxWidgets/wxWidgets-webkit2gtk-4.1.patch`) and built with `wxUSE_PRIVATE_FONTS`.

## Platform `#ifdef` conventions

Anker code is full of `#ifdef _WIN32 ... #elif __APPLE__ ... #endif` chains where Linux falls
through to nothing (uninitialised widgets, missing return values, empty paths). The rule:

- If the Apple branch is POSIX/wx-generic, change `#elif __APPLE__` to `#else` so Linux
  shares it. Use `#ifdef _WIN32` rather than `#ifndef __APPLE__` for Windows-only code.
- Only add a separate `__linux__` branch when behaviour genuinely differs (machine id from
  `/etc/machine-id`, `Os_type` "Linux", `libAnkerNet.so` lookup, resource paths via
  `Slic3r::resources_dir()`).
- **Never change what the macOS path does.** It can't be built or tested from Linux, so
  review every diff to make sure Apple branches are untouched.
- Convert wx strings to paths with `wxString::FromUTF8` / `.ToUTF8()`; never rely on the
  locale encoding.

## Network plugin and video

- `libAnkerNet` (`AnkerNetModule/`) is built from source as `AnkerNetNative`. In Release it is
  copied into `<datadir>/OnlineAnkerNet/Current/libAnkerNet_<ver>.so`; on Linux the bundled
  copy is looked up at `<exe>/../lib/libAnkerNet.so`, then next to the executable. There is
  no plugin download.
- Camera H.264 decode: VideoToolbox on macOS (`AnkerH264Decoder.mm`), FFmpeg (libavcodec +
  swscale) on Linux (`AnkerH264Decoder_ffmpeg.cpp`). Same interface: Annex-B in, RGB24 out.
- The GNOME runtime's libavcodec has no software H.264 decoder, so the Flatpak builds a
  minimal FFmpeg itself.
- Running from nix-shell: without `GIO_EXTRA_MODULES` (glib-networking) WebKit/GIO report
  "TLS support is not available"; without GSettings schemas in `XDG_DATA_DIRS` GLib aborts
  when a file chooser opens. `shell.nix` sets both.

## Flatpak quirks

- App ID `io.github.artur9010.M5FDMStudio`, GNOME 51 runtime. The SDK lacks GLU, so the
  manifest builds it.
- The `deps` module runs the superbuild with `--share=network` (fine for a self-published
  bundle, never acceptable on Flathub).
- **flatpak-builder never caches `dir` sources**, so the deps module is fed as individual
  `file` sources from `deps-sources.json`. Regenerate it with `gen-deps-sources.py` whenever
  a file is added or removed under `AnkerStudio/deps/` (CI regenerates it too, but commit
  the result).
- With `builddir: true`, `post-install` runs in the build dir; reference source files via
  `${FLATPAK_BUILDER_BUILDDIR}`.
- Installed names: binary `m5-fdm-studio` (+ `m5-fdm-studio-gcodeviewer` symlink), desktop
  file / icon `M5FDMStudio`, resources under `share/AnkerStudio` (`-DSLIC3R_FHS=1`).

## CI

- `.github/workflows/linux.yml`, matrix x86_64 (`ubuntu-24.04`) and aarch64
  (`ubuntu-24.04-arm`) in the `flathub-infra/flatpak-github-actions:gnome-51` container.
- A cold build takes hours; the flatpak-builder cache is keyed on the manifest + `deps/**`.
  Touching `deps/` invalidates it.
- `.github/workflows/macos.yml`: native runners `macos-15` (arm64) and `macos-15-intel`
  (x86_64; GitHub's last Intel image, retired around fall 2027). A `deps` job builds and
  caches `deps/deps_build/destdir` (key: arch, runner label, deployment target, `deps/**`);
  the `app` job restores it, runs `gen_xcode_proj.sh` (`ARCH=...`), `xcodebuild`, then
  `make_app.sh` with `DEV_ID="-"` (ad-hoc signed, not notarized) and checks the binary arch
  and that no load command points at `/opt/homebrew` or `/usr/local`. Artifacts:
  `M5FDMStudio-macos-{arm64,x86_64}.dmg`, attached to `v*` releases.
- Logs: `gh run view <id> --repo artur9010/eufyMake-PrusaSlicer-Release-Linux --log-failed`.
  The top-level annotation (`xvfb-run failed with exit code 1`) is meaningless; grep the
  log for `Error`/`failed` to find the real cause.

## Known upstream behaviour (not Linux bugs)

- The CLI slice can leave `<name>.gcode.tmp` without renaming it (upstream behaviour).

## Working conventions

- Commits: Conventional Commits (`fix(gtk): ...`, `build(deps): ...`, `ci: ...`,
  `docs: ...`, `feat: ...`), imperative, with a body that explains *why*. One logical change
  per commit.
- Match the surrounding file's style; indentation is mixed (older Anker GUI files use tabs,
  `AnkerNetModule/` and upstream PrusaSlicer use 4 spaces). In upstream PrusaSlicer files keep
  diffs minimal so future merges stay easy.
- Update `AnkerStudio/doc/Linux_build.md` / README when build steps, names or artifacts change.
- **`.gitignore` ignores `*.txt`, `lib`, `bin` and `build*`.** A new `CMakeLists.txt` (or any
  file under a `lib`/`bin` dir) is silently left out: `git add -f` it and check
  `git status`.
- Don't commit build outputs (`AnkerStudio/build`, `AnkerStudio/deps/build`, `build-dir`,
  `repo`, `.flatpak-builder`).
