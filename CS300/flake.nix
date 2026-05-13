{
  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      devShells.${system}.default = pkgs.mkShell {
        packages = with pkgs; [
          clang-tools
          clang
          gcc
          xmake
					gdb
          wayland
          wayland-protocols
          wayland-scanner
          libxkbcommon
          libX11
          libXcursor
          libXrandr
          libXi
          libXext
          libXfixes
          libXinerama
          libXrender
          libxscrnsaver
          libxxf86vm
          libxtst
          libunwind
          libusb1
          libdrm
          mesa
          dbus
          ibus
          udev
          libdecor
          libGL
          xorg.libxcb
          xorg.xcbutilwm
          xorg.xcbutilimage
          xorg.xcbutilkeysyms
          xorg.xcbutilrenderutil
        ];
        shellHook = ''
          export LD_LIBRARY_PATH="${pkgs.lib.makeLibraryPath [ 
            pkgs.xorg.libxcb 
            pkgs.libGL 
            pkgs.libxkbcommon 
          ]}:$LD_LIBRARY_PATH"
        '';
      };
    };
}
