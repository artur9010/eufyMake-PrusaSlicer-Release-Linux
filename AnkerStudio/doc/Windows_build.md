# Building M5 FDM Studio on Windows

Windows builds are x64 only, made with MSVC (Visual Studio 2022). CI
(`.github/workflows/windows.yml`) is the reference build. Most users should just download
the installer or the portable zip from the
[releases page](https://github.com/artur9010/eufyMake-PrusaSlicer-Release-Linux/releases):

- `M5FDMStudio-windows-x64-setup.exe`: installer (per-user or per-machine)
- `M5FDMStudio-windows-x64.zip`: portable; unpack it anywhere and run `M5FDMStudio.exe`

Neither is code-signed, so SmartScreen warns on first launch ("More info" -> "Run anyway").
The login window needs the Microsoft Edge WebView2 runtime, which Windows 10/11 already
include.

## 0. Prerequisites

- Visual Studio 2022 (Community is enough) with the "Desktop development with C++"
  workload. Run every command below in an **x64 Native Tools Command Prompt for VS 2022**.
- CMake 3.21+ and Ninja (both come with Visual Studio), Git.
- [vcpkg](https://github.com/microsoft/vcpkg) for jansson, OpenSSL and FFmpeg.
- Optional: [Inno Setup 6](https://jrsoftware.org/isinfo.php) to build the installer.

Use short paths without spaces (e.g. `C:\src`, `C:\d`, `C:\b`): parts of the Boost and
OCCT builds still hit the 260-character path limit.

## 1. Dependencies

The superbuild compiles Boost, wxWidgets, OCCT, OpenVDB, TBB, CGAL and friends as static
libraries (GMP/MPFR are prebuilt DLLs in the repository). It takes a few hours.

```
cmake -S AnkerStudio\deps -B C:\d -G "Visual Studio 17 2022" -A x64 -DDEP_DEBUG=OFF
cmake --build C:\d --config Release
```

The result is in `C:\d\destdir\usr\local`.

The rest comes from vcpkg (FFmpeg is a minimal LGPL build with just libavcodec and
libswscale, as shared libraries):

```
vcpkg install --triplet x64-windows-release --x-install-root=C:\vcpkg_installed ^
    jansson openssl "ffmpeg[core,avcodec,swscale]"
```

## 2. The application

```
cmake -S AnkerStudio -B C:\b -G Ninja -DCMAKE_BUILD_TYPE=Release -DSLIC3R_STATIC=1 ^
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 ^
    "-DCMAKE_PREFIX_PATH=C:/d/destdir/usr/local;C:/vcpkg_installed/x64-windows-release"
cmake --build C:\b
```

Output lands in `C:\b\src`: `M5FDMStudio.exe` (GUI launcher), `M5FDMStudio-console.exe`
(CLI, e.g. `--help` or `--export-gcode`), `M5FDMStudio-gcodeviewer.exe`, `eufyStudio.dll`
(the application itself), `AnkerNet.dll` (the open-source network plugin) and
`OCCTWrapper.dll` (STEP import). The build copies GMP/MPFR and `WebView2Loader.dll` there
and links `resources` into it.

To run from the build tree, also copy the DLLs from
`C:\vcpkg_installed\x64-windows-release\bin` next to the executables.

## 3. Package

The "Stage portable package" step of `.github/workflows/windows.yml` lists exactly what
goes into the package: the files above, the vcpkg DLLs, the MSVC runtime DLLs from
`%VCToolsRedistDir%x64\Microsoft.VC143.CRT`, `pack\windows\runtime\mesa\opengl32.dll` into
`mesa\` (software OpenGL fallback) and `resources\`. The installer is then

```
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" /DAppVersion=1.5.26 /DSourceDir=C:\pkg\M5FDMStudio ^
    AnkerStudio\pack\windows\M5FDMStudio.iss
```

## Notes

- The network plugin is `AnkerNet.dll`, built from `src/slic3r/GUI/AnkerNetModule`. At
  startup (Release) it is copied to `<datadir>\OnlineAnkerNet\Current\AnkerNet_<ver>.dll`
  and loaded from there; its FFmpeg/OpenSSL DLLs are found in the application directory.
- Camera video is decoded with FFmpeg, as on Linux (`AnkerH264Decoder_ffmpeg.cpp`).
