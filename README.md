# LLVM Auto-Vectorization Pass with Dependence Analysis for RISC-V RVV

This project implements an LLVM LoopPass that automatically vectorizes loops in C code for the RISC-V Vector Extension (RVV), using dependence analysis to ensure safety.

## Features

- Analyzes loop dependencies with LLVM's `DependenceInfo`.
- Vectorizes safe loops with RVV intrinsics.
- Targets nested loops like matrix multiplication.

## Prerequisites

- Ubuntu 24.04
- LLVM/Clang 18+ (`sudo apt install llvm clang cmake`)
- RISC-V toolchain (`sudo apt install gcc-riscv64-linux-gnu`)
- QEMU-RISCV (`sudo apt install qemu-system-riscv64`)

## Build Instructions

1. Clone the repo: `git clone https://github.com/yourusername/RVVAutoVectorize.git`
2. Build the pass:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
