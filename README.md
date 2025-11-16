# gltut-lab
practical exercises related to gltut

```
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cmake --build . --config Release
```
in root for clangd support
```
ln -s build/compile_commands.json .
```

some issue with glsdk when cloning, copy it from original version


line 121 in freeglut_std.h makes clangd act up, comment it out if needed



there is some issue with how the project is built where shader names will interfere with eachother, if you can't see objects or meshes rendering try renaming the shaders in the specific data folder to something else and linking to those shaders in the .cpp file that initalizes them



