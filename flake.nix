{
  description = "hdoc";

  outputs = { self, nixpkgs }: {
    pkgs = import nixpkgs { system = "x86_64-linux"; };
    buildTimeDeps = with import nixpkgs { system = "x86_64-linux"; }; [
      xxd
      meson
      cmake
      ninja
      pkg-config
    ];
    gitrev = if (self ? rev) then builtins.substring 0 7 self.rev else "dirty";

    packages.x86_64-linux.hdoc = with import nixpkgs { system = "x86_64-linux"; };
      stdenv.mkDerivation {
        name = "hdoc";
        src = self;

        nativeBuildInputs = self.buildTimeDeps;
        buildInputs = [
          openssl
          llvmPackages_12.clang-unwrapped
          llvmPackages_12.llvm
          llvmPackages_12.libclang
        ];

        CPPFLAGS = ''-DHDOC_GIT_REV=\"${self.gitrev}\"'';
        installPhase = ''
          mkdir -p $out/bin;
          install -t $out/bin hdoc;
          install -t $out/bin hdoc-client;
        '';
      };

    defaultPackage.x86_64-linux = self.packages.x86_64-linux.hdoc;
    devShell.x86_64-linux = with import nixpkgs { system = "x86_64-linux"; };
      mkShell {
        buildInputs = self.buildTimeDeps ++ [
          zola
          openssl
          llvmPackages_12.clang-unwrapped
          llvmPackages_12.llvm
          llvmPackages_12.libclang
        ];
      };
  };
}
