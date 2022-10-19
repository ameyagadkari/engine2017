# GAME ENGINE 
* Its a multithreaded engine which supports DirectX 11 and OpenGL apis for rendering
* x64 uses DirectX 11 to render and x86 uses OpenGL (An arbitrary choice)

# BUILD STEPS
1. Clone or download the repo
2. Open .sln file using Visual Studio 2022
3. Open the property page for the Example Game project (set the project as start up if not automatically set)
4. For all configs and platforms, go to debugging tab 
> * Change **Command** to **$(GameInstallDir)** **$(TargetFileName)**
> * Change **Working Directory** to **$(GameInstallDir)**
5. Build **BuildExampleGameAssets**
6. Set **ExampleGame** as startup project and run

# CONTROLS
1. WASDQE to move camera and arrow keys to move the rotating globe
2. L to take a screenshot