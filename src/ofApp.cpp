

#include "ofApp.h"





//--------------------------------------------------------------
void ofApp::setup(){

  gui.setup();
  ofSetBackgroundColor(114,144,154);
  ofSetWindowTitle("VR Transfer Function Editor");

  vr = new volumeRenderThread();
 // ImGui::GetIO().MouseDrawCursor = false;
  //vr->setup(ofToDataPath("tooth.vti").c_str());
  tfEditor = new TFEditor(vr);

  startedVTK = false;

  //now that we've read the data, set up the gui
  //we'll make the slider have a callback to re-run MC


}



//--------------------------------------------------------------
void ofApp::update(){
}




int foo = 0;

//--------------------------------------------------------------
void ofApp::draw() {
    gui.begin();
    ImGui::Begin("Colormap editor");
	ImGui::Text("The TF editor has 3 widgets - scalar opacity widget, scalar color widget and gradient\n opacity widget. A colormap can be selected from any of the 5 different choices");
    if (!startedVTK && tfEditor->isDataSetLoaded()) {
        //vr->setupRenderer();
       // vr->startThread();
        startedVTK = true;
        //vr->started = true;
    }
    tfEditor->draw();

  //initialize the vtk render window, if it's not yet started

    ImGui::End();
    gui.end();
    //gui.end();
  //cout << foo << endl;
  //foo++;
}

void ofApp::exit() {
  vr->stopThread();
  //::exit(0);


  vr->waitForThread();
}






void ofApp::toggleShading(bool& shadeOn) {
  if (startedVTK && tfEditor->isDataSetLoaded()) {
    //if(vr.lock()) {

      if (shadeOn) {
        vr->volumeProperty->ShadeOn();
      } else {
        vr->volumeProperty->ShadeOff();
      }

      vr->update();
      //vr.updated = true;
      //vr.unlock();
    //}
  }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
