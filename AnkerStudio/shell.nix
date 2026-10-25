# Build environment for compiling M5 FDM Studio on NixOS / with Nix.
# Usage: nix-shell AnkerStudio/shell.nix   (then follow doc/Linux_build.md)
{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  hardeningDisable = [ "all" ];
  nativeBuildInputs = with pkgs; [
    cmake ninja pkg-config git gettext autoconf automake libtool m4 perl python3 which file
  ];
  buildInputs = with pkgs; [
    gtk3 webkitgtk_4_1 libsoup_3 glib-networking
    ffmpeg jansson openssl curl zlib xz zstd bzip2 dbus
    libGL libGLU mesa libx11 libxext libxrandr libxi libxtst libxkbcommon wayland
    fontconfig freetype libsecret systemd
  ];
  # WebKitGTK needs glib-networking's GIO module for TLS when run from this shell.
  shellHook = ''
    export GIO_EXTRA_MODULES=${pkgs.glib-networking}/lib/gio/modules
  '';
}
