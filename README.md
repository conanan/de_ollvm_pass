# llvm-pass-ollvm

A completely  LLVM pass to deollvm.

Build:

    $ cd llvm-pass-deollvm
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ cd ..

Run:

    $ clang -Xclang -load -Xclang build/skeleton/libSkeletonPass.* something.c