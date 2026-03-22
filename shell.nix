{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    llvmPackages_19.llvm
    llvmPackages_19.clang
    cmake
    ninja
    zlib
    pkgs.pkgsCross.mipsel-linux-gnu.buildPackages.gcc
    pkgs.pkgsCross.mipsel-linux-gnu.buildPackages.binutils
  ];

  shellHook = ''
    export LLVM_DIR=${pkgs.llvmPackages_19.llvm.dev}/lib/cmake/llvm
  '';
}
