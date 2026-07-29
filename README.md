# M5 FDM Studio
![image](AnkerStudio/resources/icons/AnkerStudio.png)

## Overview
M5 FDM Studio is a native Apple Silicon build of **eufyMake Studio**, for AnkerMake M5 and
M5C printers. It offers remote printing control and real-time streaming, giving you full
control wherever you are.

It is a fork of [eufyMake Studio](https://github.com/eufymake/eufyMake-PrusaSlicer-Release)
and tracks upstream v1.5.26. The slicing engine, profiles, and output are unchanged; the
work in this fork is in the networking, device, and printing layers.

The upstream release ships a closed-source, x86_64-only network plugin, which forces the
whole application to run under Rosetta on Apple Silicon. This fork replaces that plugin
with a native implementation built from source, so the app runs natively on arm64 — and
keeps running on macOS 27, where relying on Rosetta is no longer a safe assumption.

This is an unofficial community fork. It is not affiliated with, endorsed by, or supported
by Anker or eufyMake.

M5 FDM Studio is based on [eufyMake Studio](https://github.com/eufymake/eufyMake-PrusaSlicer-Release),
which is based on [PrusaSlicer](https://github.com/prusa3d/PrusaSlicer) by Prusa Research,
which is from [Slic3r](https://github.com/Slic3r/Slic3r) by Alessandro Ranellucci and the
RepRap community.

## What are M5 FDM Studio's main features?

Key features are:

- **Runs on macOS 27** — fully native arm64, with no dependency on Rosetta;
- Basic slicing features & GCode viewer;
- Remote control & monitoring;
- Live camera streaming, on the local network and remotely;
- No proprietary network plugin, and no plugin download prompt;

See [RELEASE_NOTES.md](RELEASE_NOTES.md) for what works, and for the list of things that
are known not to work.

## Download

Prebuilt, signed and notarized builds are on the
[releases page](https://github.com/bobbyb/eufyMake-PrusaSlicer-Release-ARM/releases).

## How to compile
- macOS on Apple Silicon, [Compile Guide](AnkerStudio/doc/MacOs_build.md)

This fork targets Apple Silicon only. For Windows or Intel builds, use the
[upstream project](https://github.com/eufymake/eufyMake-PrusaSlicer-Release).

# Report issue
You can add an issue to the [github tracker](https://github.com/bobbyb/eufyMake-PrusaSlicer-Release-ARM/issues) if **it isn't already present.**

## License
	
M5 FDM Studio is licensed under the GNU Affero General Public License, version 3. M5 FDM
Studio is based on eufyMake Studio, which is based on PrusaSlicer by PrusaResearch.

PrusaSlicer is licensed under the GNU Affero General Public License, version 3. PrusaSlicer is owned by Prusa Research. PrusaSlicer is originally based on Slic3r by Alessandro Ranellucci.

Slic3r is licensed under the GNU Affero General Public License, version 3. Slic3r was created by Alessandro Ranellucci with the help of many other contributors.

The GNU Affero General Public License, version 3 ensures that if you use any part of this software in any way (even behind a web server), your software must be released under the same license.
