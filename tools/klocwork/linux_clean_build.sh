rm -rf build
mkdir build
cd build
cmake -G "Unix Makefiles" -DETCPAL_BUILD_TESTS=ON -DETCPAL_BUILD_EXAMPLES=ON ..
cmake --build .
