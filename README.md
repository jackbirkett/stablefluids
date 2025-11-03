Hello, this is a fairly bare-bones real-time interactive fluid simulation program based on Jos Stam's [Stable Fluids](https://pages.cs.wisc.edu/~chaol/data/cs777/stam-stable_fluids.pdf) algorithm. It is written in C++ with OpenGL for rendering graphics. 

## How to build and run:

Windows:
```
1. git clone https://github.com/jackbirkett/stablefluids.git
2. cd FluidSimulation
3. mkdir build
4. cd build
5. cmake .. -G "Visual Studio 17 2022"
6. cmake --build . --config Release
7. cd bin/Release
8. FluidSimulator.exe
```
Linux (haven't personally tested):
```
1. git clone https://github.com/jackbirkett/stablefluids.git
2. cd FluidSimulation
3. mkdir build
4. cd build
5. cmake -DCMAKE_BUILD_TYPE=Release ..
6. make
7. ./bin/FluidSimulator
```
Libraries Used: GLFW, GLAD, Dear ImGui

Resources Used:
 - https://pages.cs.wisc.edu/~chaol/data/cs777/stam-stable_fluids.pdf
 - https://graphics.cs.cmu.edu/nsp/course/15-464/Fall09/papers/StamFluidforGames.pdf
