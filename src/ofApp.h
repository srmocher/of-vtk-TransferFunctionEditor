

#pragma once

#include "ofMain.h"
#include "volumeRenderThread.h"
#include "ofxImGui.h"
#include "tfeditor.h"





class ofApp : public ofBaseApp {
  public:
    void setup();
    void update();
    void draw();
    void exit();
    void updateIsovalue(float& isovalue);
    void toggleShading(bool& shadeOn);

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void addControlPointsToVTK();

    ofxImGui::Gui gui;

    bool startedVTK;
    volumeRenderThread *vr;

    TFEditor *tfEditor;

};
