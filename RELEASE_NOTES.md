### M5 FDM Studio V0.2.0

This release takes the app beyond Apple Silicon. M5 FDM Studio now builds for **Linux**,
**Windows** and **Intel and Apple Silicon Macs**, and every platform is built automatically
by GitHub Actions from this repository:
https://github.com/artur9010/eufyMake-PrusaSlicer-Release-Linux

Presets, configuration, profiles and the login cache still carry over unchanged from
eufyMake Studio and from 0.1.0.

This is an unofficial community fork. It is not affiliated with, endorsed by, or
supported by Anker or eufyMake.

### Downloads:

| Platform | File |
|---|---|
| Linux x86_64 | `M5FDMStudio-x86_64.flatpak` |
| Linux arm64 | `M5FDMStudio-aarch64.flatpak` |
| Windows x64 | `M5FDMStudio-windows-x64-setup.exe` (installer) or `M5FDMStudio-windows-x64.zip` (portable) |
| macOS Apple Silicon | `M5FDMStudio-macos-arm64.dmg` (macOS 15 or newer) |
| macOS Intel | `M5FDMStudio-macos-x86_64.dmg` (macOS 15 or newer) |

Linux: `flatpak install --user M5FDMStudio-x86_64.flatpak`, then
`flatpak run io.github.artur9010.M5FDMStudio`.

### What's new:

1. **Linux support (x86_64 and arm64)**
The app builds and runs natively on Linux with GTK 3 and WebKitGTK 4.1, packaged as a
Flatpak on the GNOME 51 runtime. Sign-in, the printer list, live monitoring, sending
prints and the live camera all work. The network plugin needed only small portability
fixes. A few Anker widgets that crashed under GTK were reworked (tab buttons, toggle
switches, and the printer chooser that opens when you press Print).

2. **Camera decoding with FFmpeg on Linux and Windows**
The camera stream, decoded by VideoToolbox on macOS, is decoded by FFmpeg (libavcodec)
on Linux and Windows. 720p H.264, on the LAN and remotely.

3. **Windows support (x64)**
The native network plugin is ported to Windows (Winsock, OpenSSL, FFmpeg). The app builds
with current Visual Studio and ships as an Inno Setup installer and a portable zip, with
the MSVC runtime bundled. Sign-in uses the Edge (WebView2) engine.

4. **Intel Mac builds and macOS DMGs from CI**
The macOS build is no longer hardcoded to arm64: `gen_xcode_proj.sh` takes `ARCH`, and
GitHub Actions builds native DMGs for Apple Silicon (`macos-15`) and Intel
(`macos-15-intel`). Builds compile with Xcode 16 (Boost 1.78 enum-conversion error
silenced). CI checks that each DMG has the right architecture, has no stray Homebrew
library paths and passes `codesign --verify`, and that the bundled app starts.

5. **Live status for EU accounts fixed**
The app picked the message broker by checking whether the account's `ab_code` was
"EU", but that field holds the account's country (e.g. `PL`). EU accounts were therefore
connected to the US broker and never received temperatures, progress or print state.
Printing still worked, which hid the problem. The broker now follows the API server the
account is assigned at login. This affected every platform, including 0.1.0 on macOS.
The EU broker is confirmed to present the same pinned certificate as the US one.

6. **Update checker removed, "View source code" added**
Both the PrusaSlicer and Anker update checkers are gone: the startup and 24-hour
background checks, the "new version" dialogs, notifications and red dot, and the related
options in Preferences and the first-run wizard. Settings → "Check For Update" is replaced
by **View source code**, which opens this repository. Release Notes now point at this
fork's releases. Printer profile updates are unaffected.

7. **Build system modernised**
The dependency superbuild works with current toolchains (GCC 16, CMake 4, current MSVC):
fixes for C23 defaults, LTO archives, case-sensitive includes, link order, and download
hosts that block CI runners. Build guides for every platform live in `AnkerStudio/doc/`,
and `AGENTS.md` documents the quirks for contributors.

8. **Automated builds and releases**
Every push builds all platforms on GitHub Actions. Tagging a `v*` release attaches the
Flatpaks, the Windows installer and zip, and the macOS DMGs to the GitHub release.

### Known not to work / untested:

1. **Windows has only been tested in CI.** Packaging, DLL loading, the network plugin and
CLI slicing are checked automatically, but the GUI, sign-in, live monitoring, printing and
the camera have not yet been tried on a real Windows desktop.

2. **The Linux arm64 Flatpak has only been built, not run on hardware.**

3. **All builds are unsigned.** Windows SmartScreen will warn. The macOS DMGs are only
ad-hoc signed and not notarized: right-click → Open (or
`xattr -dr com.apple.quarantine "/Applications/M5 FDM Studio.app"`) the first time. The
CI-built DMGs have not yet been launched on a real Mac.

4. **macOS 15 is the minimum for the CI DMGs.** The bundled Homebrew libraries (OpenSSL,
jansson, xz, zstd) come from the macOS 15 runners.

5. **Linux needs X11 or XWayland.** The 3D view uses GLX, so the app runs with
`GDK_BACKEND=x11` (the Flatpak sets this).

6. **Direct LAN connections may fall back to remote.** In testing the app sometimes failed
to reach the printer directly on the LAN and connected over the internet instead. The
camera and uploads still worked, but went through the remote path.

7. Items 2–7, 9–11 and 13 from the 0.1.0 list below still apply: camera light and quality
controls, firmware updates, multi-colour printers, switching cameras between printers,
raw Message Center error codes, disabled analytics, the recovered delete endpoint,
unverified extrude/retract, no relay fallback, and the unchanged slicing engine.

---

> ## ⬆ Above: this fork (artur9010). ⬇ Below: the project it is forked from.
>
> **M5 FDM Studio 0.2.0 is a fork of [bobbyb's M5 FDM Studio 0.1.0](https://github.com/bobbyb/eufyMake-PrusaSlicer-Release-ARM/releases/tag/v0.1.0)**
> (forked at commit `a602b4d`, shortly after the `v0.1.0` tag). Everything below is that
> project's original 0.1.0 release notes, unchanged.

---

### M5 FDM Studio V0.1.0

First release of this fork. A native Apple Silicon (arm64) build of eufyMake Studio for
AnkerMake M5 / M5C printers, based on upstream eufyMake Studio v1.5.26.

The stock release ships a closed-source, x86_64-only network plugin (`libAnkerNet.dylib`)
that handles login, device discovery, telemetry, and printing. Because the app cannot
load it natively on Apple Silicon, everything runs under Rosetta. This fork replaces that
plugin with a from-scratch native implementation, so the app runs natively on arm64.

Presets, configuration, and profiles carry over unchanged — the profile directory is
deliberately unchanged from eufyMake Studio, so an existing setup keeps working.

This is an unofficial community fork. It is not affiliated with, endorsed by, or
supported by Anker or eufyMake.

### What's new:

1. **Native Apple Silicon build**
No Rosetta. The proprietary network plugin is replaced by a native arm64 implementation
built from scratch, covering the account, device, telemetry, and printing paths.

2. **Account sign-in with a persistent session**
Sign-in runs through the normal eufyMake web login. The session is cached locally, so
relaunching keeps you signed in instead of asking every time.

3. **Printer list and live monitoring**
Your printers load automatically. Nozzle and bed temperatures, print state, progress,
elapsed and remaining time, filament usage, and layer counts all update live over MQTT.

4. **Printer controls**
Nozzle and bed temperature targets, Z-offset, and filament extrude/retract.

5. **Send to printer — local network and remote**
Sliced jobs upload over the printer's own P2P protocol. Remote sending works away from
home over the internet, not just on the same LAN.

6. **Live camera — local network and remote**
720p H.264 camera streaming in the Device tab, hardware-decoded through VideoToolbox,
working both on the LAN and remotely.

7. **Stop, pause, and resume a running print**
Full print control, with a confirmation dialog on cancel and a completion dialog carrying
the real filename, duration, filament usage, and layer count.

8. **Local print history**
Every job is appended to `print_history.csv` in the app's profile directory: printer,
filename, start and end time, duration, filament used, layers, and outcome. Local only —
nothing is uploaded.

9. **Message Center**
The notification bell works, with correctly formatted dates, and deleting a message now
removes it from your account rather than just hiding it locally.

10. **Broker certificate pinning**
The connection to the printer message broker now verifies the server's certificate
instead of accepting any certificate presented. Previously the channel was encrypted but
unauthenticated.

11. **Printing workflow fixes**
Print now works from the slicer window, not just the Device tab. Choosing a printer sends
the job without a second confirmation step, and the app switches to the Device tab
immediately so you can watch the transfer.

12. **Interface fixes**
Redesigned Device tab layout, the window remembers its position, dialogs open centred on
the app rather than the screen, slicing tips are legible again, and the application menu
uses the correct app name.

13. **macOS packaging**
`make_app.sh` builds, bundles, signs, notarizes, and produces a DMG.

### Known not to work:

These are genuine gaps. Some are deliberate, some are unfinished.

1. **macOS on Apple Silicon only.** No Intel, Windows, or Linux builds come out of this
fork. The upstream project remains the place to get those.

2. **Camera light toggle and video quality selection do nothing.** The controls appear but
are not implemented — the necessary commands have not been worked out yet.

3. **Firmware updates (OTA) are not implemented.** The app will not check for or install
printer firmware. Use the official app or the printer itself.

4. **Multi-colour printers are untested.** This fork targets the M5 and M5C. The filament
slot-mapping step for multi-material machines is inherited from upstream and has not been
exercised.

5. **Switching the camera between two printers can drop the active stream.** Viewing one
camera at a time works reliably.

6. **Message Center error codes show as raw codes.** The lookup that turns an error code
into descriptive text is not fetched, so entries display the code itself.

7. **Usage analytics, feedback submission, and review prompts are disabled on purpose.**
This fork does not send usage data. The feedback form and app-rating prompts are inert.

8. **Only the US message broker's certificate is verified.** The EU broker uses the same
wildcard certificate name, but this has not been confirmed against the live EU host. If it
differs, monitoring will fail to connect with an explicit certificate error rather than
falling back to an unverified connection.

9. **Deleting a Message Center entry uses an endpoint recovered from the stock plugin.** It
behaves correctly in testing, but it is not from published documentation and could change.

10. **Extrude and retract are not hardware-verified.** They are implemented over the
printer's g-code channel and are believed correct, but have not been confirmed on a
machine.

11. **Remote connections rely on direct NAT traversal.** There is no relay fallback. On
networks where a direct path cannot be established, remote camera and remote sending will
not connect; monitoring and print control still work, since those go through the message
broker.

12. **Builds are unsigned unless you supply your own Developer ID.** An unsigned build runs
locally but will be refused by Gatekeeper if copied to another machine.

13. **The slicing engine is untouched.** No slicer behaviour, profile, or output has been
modified from upstream v1.5.26.
