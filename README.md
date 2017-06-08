# of-vtk-TransferFunctionEditor
An interactive 2D transfer function editor using openframeworks and VTK

## Features
* Log-scaled histograms for scalar values and gradient magnitudes to vary opacity
* 5 different colormaps to choose from
* Add/remove/drag control points on all widgets
* .vti files supported


## Build steps
Navigate to project root
````
mkdir build
cd build
cmake ..
make
````

This requires **ofxImGui** addon as part of compilation along with **VTK**.