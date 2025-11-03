Hello, this is a fairly bare-bones real-time interactive fluid simulation program based on Jos Stam's Stable Fluids algorithm. It is written in C++ with OpenGL for rendering graphics. 

How to build and run:

Windows:

1. git clone https://github.com/jackbirkett/stablefluids.git
2. cd FluidSimulation
3. mkdir build
4. cd build
5. cmake .. -G "Visual Studio 17 2022"
6. cmake --build . --config Release
7. cd bin/Release
8. FluidSimulator.exe
