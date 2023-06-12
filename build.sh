cmake -S . -B build/ -G "Unix Makefiles" \
-D CMAKE_C_COMPILER=gcc \
-D CMAKE_CXX_COMPILER=g++ \
-D CMAKE_BUILD_TYPE=debug

cd build/

make

cd ..

# used more as a quiet dir create
mkdir -p bin

mv -f ./build/client/user ./bin/user.app
mv -f ./build/client/admin ./bin/admin.app
mv -f ./build/server/server ./bin/server.app
