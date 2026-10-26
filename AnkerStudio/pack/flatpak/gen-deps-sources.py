#!/usr/bin/env python3
"""Write deps-sources.json: the AnkerStudio/deps superbuild as flatpak-builder file sources.

flatpak-builder never caches a module whose source is a `dir` (it cannot checksum a
directory), which would rebuild every dependency on each build. File sources are
checksummed by content, so the deps module is only rebuilt when deps/ changes.
Run from anywhere; CI regenerates the file before every build.
"""
import json
import os
import subprocess

here = os.path.dirname(os.path.abspath(__file__))
deps = os.path.normpath(os.path.join(here, "..", "..", "deps"))

# Prebuilt Windows/macOS binaries are not used on Linux.
skip_prefixes = ("bin/", "GMP/gmp/", "MPFR/mpfr/")

files = subprocess.run(["git", "ls-files"], cwd=deps, check=True,
                       capture_output=True, text=True).stdout.split()
sources = []
for rel in sorted(files):
    if rel.startswith(skip_prefixes):
        continue
    source = {"type": "file", "path": os.path.relpath(os.path.join(deps, rel), here)}
    dest = os.path.dirname(rel)
    if dest:
        source["dest"] = dest
    sources.append(source)

with open(os.path.join(here, "deps-sources.json"), "w") as f:
    json.dump(sources, f, indent=2)
    f.write("\n")
print(f"wrote {len(sources)} sources")
