#pragma once

#include "ofMain.h"

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolumeProperty.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkXMLImageDataReader.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkImageGradientMagnitude.h>
#include <vtkOutlineFilter.h>



class volumeRenderThread : public ofThread {
private:
    vector<float> scalars;

  public:
    volumeRenderThread(){this->started=false;this->initial=false;}
    void setup(const char* path);
    void setupRenderer();
    void update();
    void threadedFunction();
    vector<float> getScalars();
    vector<float> setupGradientMagnitudes();
    vector<float> getGradients();
    vtkSmartPointer<vtkXMLImageDataReader> reader;
    vtkSmartPointer<vtkImageData> image;
    vtkSmartPointer<vtkRenderWindowInteractor> iren;
    vtkSmartPointer<vtkRenderWindow> renWin;
    vtkSmartPointer<vtkOpenGLRenderWindow> oglRenWin;
    vtkSmartPointer<vtkVolumeProperty> volumeProperty;
    vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper;
    vtkSmartPointer<vtkRenderer> ren1;
    vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity;
    vtkSmartPointer<vtkColorTransferFunction> color;
    vtkSmartPointer<vtkVolume> volume;
    vtkSmartPointer<vtkImageGradientMagnitude> gradient;
    vtkSmartPointer<vtkPiecewiseFunction> gradientOpacity;
    vtkSmartPointer<vtkActor> outline;
    double range[2];
    int dims[3];
    bool updated;
    bool started;
    bool initial;
    vector<float> gradients;
};


