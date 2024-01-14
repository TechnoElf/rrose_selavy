{
  inputs = {
    nixpkgs.url = github:nixos/nixpkgs/nixos-23.11;
    flake-utils.url = github:numtide/flake-utils;
  };

  outputs = { self, nixpkgs, flake-utils, ... }: flake-utils.lib.eachDefaultSystem (currentSystem:
    let
      pkgs = import nixpkgs {
        system = currentSystem;
      };
    in with pkgs; {
      devShell = mkShell.override {
        stdenv = pkgs.llvmPackages_16.libcxxStdenv;
      } rec {
        nativeBuildInputs = [
          cmake
          gnumake
          vulkan-headers
          vulkan-loader
          vulkan-validation-layers
          glslang
          shaderc
          xorg.libXrandr
          xorg.libXinerama
          xorg.libXcursor
          xorg.libXi
          pkg-config
          gtk3
        ];

        LD_LIBRARY_PATH = "${lib.makeLibraryPath nativeBuildInputs}";
        VULKAN_SDK = "${vulkan-validation-layers}/share/vulkan/explicit_layer.d";
      };

      packages.default = mkDerivation {};
    }
  );
}
