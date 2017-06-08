/**
 * tfeditor.h - TFEditor is the main class encapsulating the GUI and functionality to edit the color and alpha transfer
 * function. ofxImGui is used as the GUI widget addon
 */
#ifndef A06P01_TFEDITOR_H
#define A06P01_TFEDITOR_H

#include "ofMain.h"
#include "volumeRenderThread.h"
#include <vector>
#include "ofxImGui.h"
#include "controlpoint.h"
#endif //A06P01_TFEDITOR_H

//1D Transfer function editor
class TFEditor
{
private:
    ofImage colorMap; //image drawing the selected color map
    std::vector<float> scalars,gradientMagnitudes;
    map<float,int> counts;
    map<float,int> gradientCounts;
    std::vector<OpacityControlPoint> opacityControlPoints;
    std::vector<ColorControlPoint> colorControlPoints;
    std::vector<GradientOpacityPoint> gradientControlPoints;
    float minVal;
    float maxVal;
    float minGradVal,maxGradVal;
    void drawColorMap();
    void drawHistogram();
    void loadColors(const char *path);
    void drawOpacityPanel();
    ofImage opacityColorRegion;
    int colorMapWidth,colorMapHeight;
    std::vector<ofColor> colors;
    volumeRenderThread *vrThread;
    int currentColorMap;
    void loadCounts();
    void loadGradientCounts();
    void drawControlPoints();
    int colorPanelPosX,colorPanelPosY,opacityPanelPosX,opacityPanelPosY,gradientPanelX,gradientPanelY;
    float maxCount,minCount,secondHighestCount,maxGradCount,minGradCount;
    int opacityPanelHeight,opacityPanelWidth,gradientPanelWidth,gradientPanelHeight;
    void addDefaultControlPoints();
    void setup();
    bool dataSetLoaded,initialSetup=true;
    string currentColorMapFile;
    bool colorPointDraggable,opacityPointDraggable,gradientPointDraggable,colorPointSelected,opacityPointSelected,gradientPointSelected;
    int colorPointSelectedIndex,opacityPointSelectedIndex,gradientPointSelectedIndex;
    void addColorPointsToVTK();
    void addOpacityPointsToVTK();
    void addGradientPointsToVTK();
    void colorOpacityPanel();
    void drawOpacityControlLines();
    void drawGradientHistogram();
    bool shadingOn,enableGradientOpacity;
    void drawGradientOpacityPanel();
    void setupGradients();
    void addDefaultGradientControlPoints();
    bool boundingBox;
public:
    TFEditor(volumeRenderThread *vrThread);
    ~TFEditor();
    void draw();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(ofMouseEventArgs & args);
    void mouseDragged(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    void mouseScrolled(ofMouseEventArgs & args);
    void mouseEntered(ofMouseEventArgs & args);
    void mouseExited(ofMouseEventArgs & args);
    void addControlPointsToVTK();
    bool findColorPoint(int x, int y,int& selectedIndex);
    bool findOpacityPoint(int x, int y,int& selectedIndex);
    bool findGradientPoint(int x,int y, int& selectedIndex);
    bool isDataSetLoaded(){return dataSetLoaded;}

};