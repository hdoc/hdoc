{
  description = "hdoc";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = nixpkgs.legacyPackages.${system};
      in rec {
        buildTimeDeps = with pkgs; [ xxd meson cmake ninja pkg-config ];
        gitrev =
          if (self ? rev) then builtins.substring 0 7 self.rev else "dirty";

        packages.hdoc = with pkgs;
          stdenv.mkDerivation {
            name = "hdoc";
            src = self;

            nativeBuildInputs = buildTimeDeps;
            buildInputs = [
              openssl
              llvmPackages_14.clang-unwrapped
              llvmPackages_14.llvm
              llvmPackages_14.libclang
            ];

            CPPFLAGS = ''-DHDOC_GIT_REV=\"${gitrev}\"'';
            installPhase = ''
              mkdir -p $out/bin;
              install -t $out/bin hdoc;
              install -t $out/bin hdoc-online;
            '';
          };

        defaultPackage = packages.hdoc;
        devShell = with pkgs;
          mkShell {
            buildInputs = buildTimeDeps ++ [
              zola
              openssl
              llvmPackages_14.clang-unwrapped
              llvmPackages_14.llvm
              llvmPackages_14.libclang
            ];
          };
      });
}
