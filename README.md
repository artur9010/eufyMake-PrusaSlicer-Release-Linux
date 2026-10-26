# M5 FDM Studio
![image](AnkerStudio/resources/icons/AnkerStudio.png)

## Overview
M5 FDM Studio is a native Apple Silicon build of **eufyMake Studio**, for AnkerMake M5 and
M5C printers. It offers remote printing control and real-time streaming, giving you full
control wherever you are.

It is a fork of [eufyMake Studio](https://github.com/eufymake/eufyMake-PrusaSlicer-Release)
and tracks upstream v1.5.26. The slicing engine, profiles, and output are unchanged; the
work in this fork is in the networking, device, and printing layers.

### Why this fork exists

macOS 27 drops support for Intel applications. The upstream release ships a
closed-source, x86_64-only network plugin, which forces the whole application to run under
Rosetta on Apple Silicon — so upstream stops working on macOS 27 altogether.

This fork replaces that plugin with a native implementation built from source. Nothing in
the app is x86_64, nothing depends on Rosetta, and it keeps working on macOS 27.

This is an unofficial community fork. It is not affiliated with, endorsed by, or supported
by Anker or eufyMake.

M5 FDM Studio is based on [eufyMake Studio](https://github.com/eufymake/eufyMake-PrusaSlicer-Release),
which is based on [PrusaSlicer](https://github.com/prusa3d/PrusaSlicer) by Prusa Research,
which is from [Slic3r](https://github.com/Slic3r/Slic3r) by Alessandro Ranellucci and the
RepRap community.

## What are M5 FDM Studio's main features?

Key features are:

- **Runs on macOS 27**, which drops Intel app support — fully native arm64, no Rosetta;
- **Runs on Linux** (x86_64 and arm64, GTK 3), packaged as a Flatpak;
- **Runs on Windows** (x64), as an installer or a portable zip;
- Basic slicing features & GCode viewer;
- Remote control & monitoring;
- Live camera streaming, on the local network and remotely;
- No proprietary network plugin, and no plugin download prompt;

See [RELEASE_NOTES.md](RELEASE_NOTES.md) for what works, and for the list of things that
are known not to work.

## Download

Prebuilt, signed and notarized builds are on the
[releases page](https://github.com/bobbyb/eufyMake-PrusaSlicer-Release-ARM/releases).

### Linux

Flatpak bundles (`M5FDMStudio-x86_64.flatpak` and `M5FDMStudio-aarch64.flatpak`) are attached to each release of the
[Linux fork](https://github.com/artur9010/eufyMake-PrusaSlicer-Release-Linux/releases),
and every push builds them as GitHub Actions artifacts. Install the one for your CPU:

```
flatpak install --user M5FDMStudio-x86_64.flatpak
```

### Windows

`M5FDMStudio-windows-x64-setup.exe` (installer) and `M5FDMStudio-windows-x64.zip`
(portable: unpack anywhere, run `M5FDMStudio.exe`) are attached to each release of the
[Linux fork](https://github.com/artur9010/eufyMake-PrusaSlicer-Release-Linux/releases), and
every push builds them as GitHub Actions artifacts. x64 only. They are not code-signed, so
SmartScreen asks for confirmation on first launch ("More info" -> "Run anyway").

## How to compile
- macOS on Apple Silicon, [Compile Guide](AnkerStudio/doc/MacOs_build.md)
- Linux (x86_64 and arm64, GTK 3), [Compile Guide](AnkerStudio/doc/Linux_build.md)
- Windows (x64, MSVC), [Compile Guide](AnkerStudio/doc/Windows_build.md)

# Report issue
You can add an issue to the [github tracker](https://github.com/bobbyb/eufyMake-PrusaSlicer-Release-ARM/issues) if **it isn't already present.**

## License
	
M5 FDM Studio is licensed under the GNU Affero General Public License, version 3. M5 FDM
Studio is based on eufyMake Studio, which is based on PrusaSlicer by PrusaResearch.

PrusaSlicer is licensed under the GNU Affero General Public License, version 3. PrusaSlicer is owned by Prusa Research. PrusaSlicer is originally based on Slic3r by Alessandro Ranellucci.

Slic3r is licensed under the GNU Affero General Public License, version 3. Slic3r was created by Alessandro Ranellucci with the help of many other contributors.

The GNU Affero General Public License, version 3 ensures that if you use any part of this software in any way (even behind a web server), your software must be released under the same license.
