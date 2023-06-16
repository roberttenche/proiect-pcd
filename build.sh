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
mv -f ./build/client/gui ./bin/gui.app
mv -f ./build/server/server ./bin/server.app
cp -f ./client/python_client/python_user.py ./bin/python_user.py
cp -f ./client/python_client/python_gui.py ./bin/python_gui.py
