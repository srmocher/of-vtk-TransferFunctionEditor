#include "volumeRenderThread.h"



void volumeRenderThread::setup(const char* path) {
  reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
  reader->SetFileName(path);
  reader->Update();

  image = reader->GetOutput();
  image->GetPointData()->GetScalars()->GetRange(range);
  image->GetDimensions(dims);

  scalars.clear();
  for(int i=0;i<dims[0];i++){
    for(int j=0;j<dims[1];j++){
      for(int k=0;k<dims[2];k++){
        float val = image->GetScalarComponentAsFloat(i,j,k,0);
        scalars.push_back(val);
      }
    }
  }

  setupGradientMagnitudes();
  //std::cout << "Image dimensions: " << dims[0] << "x" << dims[1] << "x" << dims[2] << std::endl;

  //std::cout << "Scalar value range: [" << range[0] << "," << range[1] << "]" << std::endl;

  updated = false;
}


void volumeRenderThread::setupRenderer() {
  renWin = vtkSmartPointer<vtkRenderWindow>::New();
  oglRenWin = vtkOpenGLRenderWindow::SafeDownCast(renWin);
  oglRenWin->PushContext();
  ren1 = vtkSmartPointer<vtkRenderer>::New();
  renWin->AddRenderer(ren1);
  //renWin->InitializeFromCurrentContext();

  ren1->SetBackground(0.0,0.0,0.0);
  renWin->SetSize(300,300);

  iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);

  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); 
  iren->SetInteractorStyle( style );

  volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
#ifndef __APPLE__
  volumeMapper->SetRequestedRenderModeToRayCast();
#endif
  volumeMapper->SetBlendModeToComposite(); 
  volumeMapper->SetInputData(image);

  volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
  volumeProperty->ShadeOn();
  volumeProperty->SetInterpolationTypeToLinear();

    vtkSmartPointer<vtkOutlineFilter> outlineData =
            vtkSmartPointer<vtkOutlineFilter>::New();
    outlineData->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkPolyDataMapper> mapOutline =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapOutline->SetInputConnection(outlineData->GetOutputPort());

    outline =
            vtkSmartPointer<vtkActor>::New();
    outline->SetMapper(mapOutline);
    outline->GetProperty()->SetColor(255,255,255);
  compositeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
  volumeProperty->SetScalarOpacity(compositeOpacity); 


  color = vtkSmartPointer<vtkColorTransferFunction>::New();
  volumeProperty->SetColor(color);


  gradientOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
  volumeProperty->SetGradientOpacity(gradientOpacity);


  volume = vtkSmartPointer<vtkVolume>::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  ren1->AddVolume(volume);
    ren1->AddActor(outline);

    //hide bounding box by default
    vtkPropCollection* props = ren1->GetViewProps();
    props->InitTraversal();
    props->GetNextProp();
    props->GetNextProp()->VisibilityOff();
  ren1->ResetCamera();

  oglRenWin->PopContext();

}


void volumeRenderThread::threadedFunction(){
#ifndef __APPLE__
  renWin->Render();
    if(!this->initial) {
        ofThread::sleep(400);
        initial = true;
    }
  iren->Start();
#endif

  //In theory, the following loop is the same as what iren->Start()
  //does -- just call Render() over-and-over.  But it's not necessary to
  //use and doesn't seem to work right on Linux
  /*
  while(isThreadRunning()) {
    if(lock()) {
      if (updated) {
        oglRenWin->PushContext();
        renWin->Render();
        oglRenWin->PopContext();
        updated = false;
      }
      unlock();
    }
  }
  */
}

void volumeRenderThread::update() {
  //force the rendered to update again

  oglRenWin->PushContext();
  renWin->Render();

  oglRenWin->PopContext();
}

vector<float> volumeRenderThread::getScalars() {
  return this->scalars;
}

vector<float> volumeRenderThread::setupGradientMagnitudes() {
    gradient = vtkSmartPointer<vtkImageGradientMagnitude>::New();
    gradient->SetInputData(image);
    gradient->SetDimensionality(3);
    gradient->Update();

    vtkSmartPointer<vtkImageData> result = gradient->GetOutput();
    double range[2];
    result->GetScalarRange(range);
    //vector<float> gradients;
    for(int i=0;i<dims[0];i++){
        for(int j=0;j<dims[1];j++){
            for(int k=0;k<dims[2];k++){
                float val = result->GetScalarComponentAsFloat(i,j,k,0);
                gradients.push_back(val);
            }
        }
    }
   // cout<<scalars.size()<<","<<gradients.size()<<endl;
    return gradients;
}

vector<float> volumeRenderThread::getGradients(){
    return gradients;
}

