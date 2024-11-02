
#### 6. `build.sh` (Convenience Build Script)
```bash
#!/bin/bash
mkdir -p build
cd build
cmake ..
make
cd ..
echo "Build complete"