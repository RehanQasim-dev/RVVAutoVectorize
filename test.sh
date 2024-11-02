clang -S -emit-llvm tests/matmul.c -o matmul.ll
opt -load-pass-plugin=./build/RVVVectorize.so -passes=rvv-vectorize matmul.ll -o matmul_opt.ll
clang --target=riscv64-linux-gnu -march=rv64gcv matmul_opt.ll -o matmul_vec -static
qemu-riscv64 ./matmul_vec