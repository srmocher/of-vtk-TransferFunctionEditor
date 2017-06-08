//
// Created by srmocher on 4/14/17.
//

#define LOAD_DATASET "Load Dataset.."
#define SELECT_COLORMAP "Select Colormap"
#include "tfeditor.h"
#include "utils.h"
#include <algorithm>


TFEditor::TFEditor(volumeRenderThread *vrThread) {

    this->vrThread = vrThread;
    this->enableGradientOpacity = true;
    this->shadingOn = true;
    this->currentColorMapFile = "colorsDiverging.tsv";
    ofRegisterMouseEvents(this);
    colorPointDraggable = false;
    opacityPointDraggable = false;
    dataSetLoaded = false;
    gradientPointDraggable = false;
    enableGradientOpacity = false;
    boundingBox = false;
    opacityPointSelected = gradientPointSelected =colorPointSelected = false;
}

TFEditor::~TFEditor() {
    ofUnregisterMouseEvents(this);
}

void TFEditor::loadColors(const char* path) {
    string filePath = ofToDataPath(path);
    ifstream file(filePath);
    colors.clear();
    string str;
    while(!file.eof())
    {
        file >> str;
        int r = stoi(str);
        file >> str;
        int g = stoi(str);
        file >> str;
        int b = stoi(str);
        //cout<<r<<","<<g<<","<<b<<endl;
        ofColor color(r,g,b);
        colors.push_back(color);
    }
}

void TFEditor::draw() {
    const char* colorMaps[] ={"Diverging (Blue To Red)", "Linear (Black-Yellow-Red-White)","Diverging-Linear","Rainbow","Isoluminant"};

    ImGui::SetWindowSize(ImVec2(ofGetWindowWidth()-150,150));
    if(ImGui::Button(LOAD_DATASET))
    {
        ofFileDialogResult result = ofSystemLoadDialog("Load .vti dataset");
        if(result.bSuccess)
        {
            vrThread->setup(result.filePath.c_str());
            this->scalars = vrThread->getScalars();
          //  this->gradientMagnitudes = vrThread->getGradients();
            this->colorControlPoints.clear();
            this->opacityControlPoints.clear();
            this->gradientControlPoints.clear();

            if(!vrThread->started){
                vrThread->setupRenderer();
                vrThread->startThread();
                vrThread->started = true;
              //  cout<<vrThread->volumeProperty->GetShade()<<endl;
            }
            else{
                vrThread->volumeMapper->SetInputData(vrThread->image);
            }
            setup();
            addControlPointsToVTK();
            this->dataSetLoaded=true;
        }
    }

    if(dataSetLoaded &&ImGui::Combo(SELECT_COLORMAP,&currentColorMap,colorMaps,5))
    {
        switch(currentColorMap)
        {
            case 0:loadColors("colorsDiverging.tsv");

                    break;
            case 1:loadColors("colorsLinear.tsv");
                   break;
            case 2:loadColors("colorsDL.tsv");
                   break;
            case 3:loadColors("colorsRainbow.tsv");
                   break;
            case 4:loadColors("colorsIso.tsv");
                   break;
        }
        colorControlPoints.clear();
        opacityControlPoints.clear();
        gradientControlPoints.clear();
        addDefaultControlPoints();
        addControlPointsToVTK();
    }

    if(dataSetLoaded && ImGui::Checkbox("Bounding Box",&boundingBox))
    {
        if(boundingBox)
        {
            vtkPropCollection* props = vrThread->ren1->GetViewProps();
            props->InitTraversal();
            props->GetNextProp();
            props->GetNextProp()->VisibilityOn();
            vrThread->update();
        }
        else {
            vtkPropCollection* props = vrThread->ren1->GetViewProps();
            props->InitTraversal();
            props->GetNextProp();
            props->GetNextProp()->VisibilityOff();

            vrThread->update();
        }
    }

    if(dataSetLoaded && ImGui::Checkbox("Shading",&shadingOn))
    {
        if(shadingOn){
            vrThread->volumeProperty->ShadeOn();

        }
        else{
            vrThread->volumeProperty->ShadeOff();
        }
        vrThread->update();
    }
    ImGui::Text("Instructions/Notes: 1. Histograms are log scaled on the counts for both \nscalars and gradients. 2. Left click inside the panels to create control points.\n 3. Right click on an existing point to delete.\n");
    if(dataSetLoaded) {
        drawColorMap();
        drawOpacityPanel();
        drawHistogram();

            drawGradientOpacityPanel();

            drawGradientHistogram();

        drawControlPoints();
        drawOpacityControlLines();


        //if(vrThread->started)
        // addControlPointsToVTK();
    }
}

void TFEditor::drawColorMap() {
    ofTrueTypeFont font;
    font.load("verdana.ttf", 9);
    ofPushMatrix();
    int x_pos = colorPanelPosX;
    int y_pos = colorPanelPosY;
    ofTranslate(x_pos, y_pos);

    ofPixels pixels;
  //  colorPanelPosX = x_pos;
   // colorPanelPosY = y_pos;
    colorMapWidth = ofGetWindowWidth() - 50;
    colorMapHeight = 30;
    pixels.allocate(colorMapWidth,colorMapHeight,OF_IMAGE_COLOR);
    for(int i=0;i<pixels.getWidth();i++)
    {
        int index = ofMap(i,0,colorMapWidth,0,colors.size()-1);
        ofColor color = colors[index];
        for(int j=0;j<colorMapHeight;j++)
            pixels.setColor(i,j,color);
    }
    colorMap.setFromPixels(pixels);
    colorMap.draw(0,0);
    ofPopMatrix();
}

void TFEditor::loadCounts() {
    for(int i=0;i<scalars.size();i++){
        if(counts.find(scalars[i])!=counts.end()){
            counts[scalars[i]]++;
        }
        else{
            counts[scalars[i]]=1;
        }
    }
    maxCount = -1;
    secondHighestCount = -1;
    minCount = INFINITY;

    for(map<float,int>::iterator it=counts.begin();it!=counts.end();it++){

        if(maxCount < it->second)
            maxCount = it->second;
        if(minCount > it->second)
            minCount = it->second;
    }
    maxCount = log(maxCount);
    minCount = log(minCount);
}

/**
 * Draws a log scaled histogram for the whole scalar range
 */

void TFEditor::drawHistogram() {
    ofPushMatrix();
    ofTranslate(opacityPanelPosX,opacityPanelPosY);
    int x_pos,y_pos;
    int totalVals = counts.size();
    float barWidth = (float)opacityPanelWidth/(float)totalVals;
    float xPos = 0;
    for(map<float,int>::iterator it=counts.begin();it!=counts.end();it++){
        float count = log(it->second);
        float height = ofMap(count,minCount,maxCount,0,opacityPanelHeight);
        float yPos = opacityPanelHeight - height;
        ofPushMatrix();
        ofTranslate(xPos,yPos);
        ofNoFill();
        ofSetLineWidth(1.0f);
        ofSetColor(ofColor::grey,50);
        ofDrawRectangle(0,0,barWidth,height);
        ofSetColor(ofColor::white);
        ofFill();
        ofPopMatrix();
        xPos+=barWidth;
    }

    ofPopMatrix();
}

/**
 * draws the opacity canvas housing the histogram and opacity transfer function
 */

void TFEditor::drawOpacityPanel() {

    ofPushMatrix();
    ofTranslate(opacityPanelPosX,opacityPanelPosY);
    ofSetColor(ofColor::white);
    //opacityPanelHeight = ofGetWindowHeight()-150;
    //opacityPanelWidth = ofGetWindowWidth() - 50;
   // opacityPanelPosX = 30;
   // opacityPanelPosY = 70;
    ofDrawRectangle(0,0,opacityPanelWidth,opacityPanelHeight);
    //ofSetColor(ofColor::black);
   // ofSetLineWidth(2.0f);
    //ofDrawLine(0,ofGetWindowHeight()-150,ofGetWindowWidth()-50,0);
    ofPopMatrix();
    ofSetColor(ofColor::white);
}

void TFEditor::mouseReleased(ofMouseEventArgs &args) {
    colorPointDraggable=false;
    opacityPointDraggable = false;
    gradientPointDraggable = false;
}

void TFEditor::mouseMoved(ofMouseEventArgs &args) {


    int x = ofGetMouseX();
    int y = ofGetMouseY();
    int selected;
    bool exists = findColorPoint(x,y,selected);
    if(exists)
    {
        ofTrueTypeFont font;
        ofSetColor(ofColor::black);
        font.load(ofToDataPath("verdana.ttf"),9);
        //cout<<font.isLoaded()<<endl;
        ColorControlPoint point = colorControlPoints[selected];
        ostringstream oss;
        oss.precision(2);
        oss<<fixed;
        oss << point.getValue();
        string output  = oss.str();
        //cout<<output<<endl;
        //ImGui::SetTooltip(output.c_str());
        font.drawString(output,x+3,y+4);

    }

    exists = findOpacityPoint(x,y,selected);
    if(exists)
    {
        OpacityControlPoint ocp = opacityControlPoints[selected];
        ofTrueTypeFont font;
        font.load(ofToDataPath("verdana.ttf"),9);
        ofSetColor(ofColor::black);
       // OpacityControlPoint ocp = opacityControlPoints[selected];
        ostringstream oss;
        oss.precision(2);
        oss<<fixed;
        oss << ocp.getValue();
        string output  = oss.str();
        //cout<<output<<endl;
        //ImGui::SetTooltip(output.c_str());
        font.drawString(output,x+3,y+5);
        ofSetColor(ofColor::white);
    }
    //ofSetColor(ofColor::white);
}

void TFEditor::mouseExited(ofMouseEventArgs &args) {

}

void TFEditor::mouseEntered(ofMouseEventArgs &args) {

}

void TFEditor::mouseDragged(ofMouseEventArgs &args) {
    if(colorPointDraggable){
      //  ImGui::SetMouseCursor(ImGuiMouseCursor_Move);
        int x = ofGetMouseX();
        int y = ofGetMouseY();
        int currentY = colorControlPoints[colorPointSelectedIndex].getLocation().y;
        int nextPointX = colorControlPoints[colorPointSelectedIndex+1].getLocation().x;
        float val = colorControlPoints[colorPointSelectedIndex].getValue();
        //cout<<"Size is "<<vrThread->color->GetSize()<<endl;
        if(x<colorPanelPosX || x>nextPointX)
            return;
        colorControlPoints[colorPointSelectedIndex].setLocation(ofVec2f(x,currentY));
        float newVal = ofMap(x-colorPanelPosX,0,colorMapWidth,minVal,maxVal);
        colorControlPoints[colorPointSelectedIndex].setValue(newVal);

        //addColorPointsToVTK();
       int res = vrThread->color->RemovePoint(val);
       // cout<<res<<endl;
        ofColor color = colors[ofMap(newVal,minVal,maxVal,0,colors.size()-1)];
        colorControlPoints[colorPointSelectedIndex].setColor(color);
        ofFloatColor normColor = Utils::getNormalizedColor(color);
        vrThread->color->AddRGBPoint(newVal,normColor.r,normColor.g,normColor.b);
        vrThread->update();
    }

    if(opacityPointDraggable)
    {
        int x = ofGetMouseX();
        int y = ofGetMouseY();
        if(x < opacityPanelPosX || x > opacityPanelPosX + opacityPanelWidth || y < opacityPanelPosY +3 || y > opacityPanelPosY + opacityPanelHeight + 3)
            return;
        float oldVal = opacityControlPoints[opacityPointSelectedIndex].getValue();
        int newX;
        float newVal;
        if(oldVal == maxVal || oldVal == minVal)
        {
            newX = opacityControlPoints[opacityPointSelectedIndex].getLocation().x;
            newVal = oldVal;

        }

        else
        {
            int currentX = x - opacityPanelPosX;
            newVal = ofMap(currentX,0,opacityPanelWidth,minVal,maxVal);
            newX = x;
            if(x > opacityControlPoints[opacityPointSelectedIndex+1].getLocation().x || x < opacityControlPoints[opacityPointSelectedIndex-1].getLocation().x)
                return;
        }


        opacityControlPoints[opacityPointSelectedIndex].setLocation(ofVec2f(newX,y));
        opacityControlPoints[opacityPointSelectedIndex].setValue(newVal);
        float yVal = opacityPanelHeight - y + opacityPanelPosY;
        float alpha = ofMap(yVal,0,opacityPanelHeight,0.0f,1.0f);
        opacityControlPoints[opacityPointSelectedIndex].setAlpha(alpha);
        vrThread->compositeOpacity->RemovePoint(oldVal);

        vrThread->compositeOpacity->AddPoint(newVal,alpha);
        vrThread->update();
    }

    if(gradientPointDraggable){
        int x = ofGetMouseX();
        int y = ofGetMouseY();
        if(x < gradientPanelX || x > gradientPanelX + gradientPanelWidth || y < gradientPanelY +3 || y > gradientPanelY + gradientPanelHeight + 3)
            return;
        float oldVal = gradientControlPoints[gradientPointSelectedIndex].getGrad();
        int newX;
        float newVal;
        if(oldVal == maxGradVal || oldVal == minGradVal)
        {
            newX = gradientControlPoints[gradientPointSelectedIndex].getLocation().x;
            newVal = oldVal;

        }

        else
        {
            int currentX = x - gradientPanelX;
            newVal = ofMap(currentX,0,gradientPanelWidth,minGradVal,maxGradVal);
            newX = x;
            if(x > gradientControlPoints[gradientPointSelectedIndex+1].getLocation().x || x < gradientControlPoints[gradientPointSelectedIndex-1].getLocation().x)
                return;
        }


        gradientControlPoints[gradientPointSelectedIndex].setLocation(ofVec2f(newX,y));
        gradientControlPoints[gradientPointSelectedIndex].setGradient(newVal);
        float yVal = gradientPanelHeight - y + gradientPanelY;
        float alpha = ofMap(yVal,0,gradientPanelHeight,0.0f,1.0f);
        gradientControlPoints[gradientPointSelectedIndex].setAlpha(alpha);
        vrThread->gradientOpacity->RemovePoint(oldVal);

        vrThread->gradientOpacity->AddPoint(newVal,alpha);
        vrThread->update();

    }
}


void TFEditor::mousePressed(ofMouseEventArgs &args) {
    int xPos = ofGetMouseX();
    int yPos = ofGetMouseY();
    bool leftClicked = ofGetMousePressed(OF_MOUSE_BUTTON_LEFT);

    if(leftClicked)
    {
        int selected;
        bool exists = findColorPoint(xPos,yPos,selected);
        if(exists && selected!=0 && colorControlPoints[selected].getValue()!=maxVal)
        {
            colorPointDraggable = true;
            colorPointSelectedIndex = selected;
            colorPointSelected = true;
            return;
        }

        exists = findOpacityPoint(xPos,yPos,selected);
        if(exists)
        {
            opacityPointDraggable = true;
            opacityPointSelectedIndex = selected;
            opacityPointSelected = true;
            return;
        }

        exists = findGradientPoint(xPos,yPos,selected);
        if(exists)
        {
            gradientPointDraggable = true;
            gradientPointSelectedIndex = selected;
            gradientPointSelected = true;
            return;
        }
        opacityPointSelected = gradientPointSelected =colorPointSelected = false;

    }

    bool rightClicked = ofGetMousePressed(OF_MOUSE_BUTTON_RIGHT);
    if(rightClicked)
    {
        int selected;
        bool exists = findColorPoint(xPos,yPos,selected);
        if(exists && selected!=0 && colorControlPoints[selected].getValue()!=maxVal)
        {
            vrThread->color->RemovePoint(colorControlPoints[selected].getValue());
            colorControlPoints.erase(colorControlPoints.begin()+selected);
            vrThread->update();
            return;
        }

        exists = findOpacityPoint(xPos,yPos,selected);
        if(exists && selected!=0 &&opacityControlPoints[selected].getValue()!=maxVal)
        {
            vrThread->compositeOpacity->RemovePoint(opacityControlPoints[selected].getValue());
            opacityControlPoints.erase(opacityControlPoints.begin()+selected);
            vrThread->update();
            return;
        }

        exists = findGradientPoint(xPos,yPos,selected);
        if(exists && selected!=0 && gradientControlPoints[selected].getGrad()!=maxGradVal)
        {
            vrThread->gradientOpacity->RemovePoint(gradientControlPoints[selected].getGrad());
            gradientControlPoints.erase(gradientControlPoints.begin()+selected);
            vrThread->update();
            return;
        }
    }
    if( leftClicked && xPos >= colorPanelPosX && yPos >= colorPanelPosY && xPos <= colorPanelPosX + colorMapWidth && yPos <= colorPanelPosY + colorMapHeight)
    {
        // clicked on the colormap
        float scalar = ofMap(xPos,colorPanelPosX,colorPanelPosX+colorMapWidth,minVal,maxVal);
        int colorIndex = ofMap(scalar,minVal,maxVal,0,colors.size()-1);
        ofColor color = colors[colorIndex];
        ColorControlPoint point(ofVec2f(xPos,colorPanelPosY+colorMapHeight/2),scalar,color);
        colorControlPoints.push_back(point);
        addControlPointsToVTK();
    }
    if(leftClicked && xPos >= opacityPanelPosX && xPos <= opacityPanelPosX + opacityPanelWidth && yPos >= opacityPanelPosY && yPos <= opacityPanelPosY + opacityPanelHeight)
    {
        //clicked on the opacity panel
        float scalar = ofMap(xPos,opacityPanelPosX,opacityPanelPosX + opacityPanelWidth,minVal,maxVal);
        int yPanelPos = opacityPanelHeight - yPos + opacityPanelPosY;

        float alpha = ofMap(yPanelPos,0,opacityPanelHeight,0.0f,1.0f);
        cout<<alpha<<endl;
        OpacityControlPoint point(ofVec2f(xPos,yPos),scalar,alpha);
        opacityControlPoints.push_back(point);
        addControlPointsToVTK();
    }

    if(leftClicked && xPos >= gradientPanelX && xPos <= gradientPanelX + gradientPanelWidth && yPos >= gradientPanelY && yPos <=gradientPanelY + gradientPanelHeight)
    {
        float gradVal = ofMap(xPos,gradientPanelX,gradientPanelX + gradientPanelWidth,minGradVal,maxGradVal);
        int gradPanelYPos = gradientPanelHeight - yPos + gradientPanelY;

        float alpha = ofMap(gradPanelYPos,0,gradientPanelHeight,0.0,1.0f);
        GradientOpacityPoint point(ofVec2f(xPos,yPos),gradVal,alpha);
        gradientControlPoints.push_back(point);
        addControlPointsToVTK();
    }

}

void TFEditor::mouseScrolled(ofMouseEventArgs &args) {

}

/**
 * Draw circles representing both types of control points - scalar opacity and color
 */
void TFEditor::drawControlPoints() {



    // draw opacity control points;
    for(int i=0;i<opacityControlPoints.size();i++){
       ofVec2f point = opacityControlPoints[i].getLocation();
        ofSetColor(ofColor::black);
        ofFill();
        ofDrawCircle(point.x,point.y,5.0f);
    }

    for(int i=0;i<colorControlPoints.size();i++){
        ofVec2f point = colorControlPoints[i].getLocation();
        ofNoFill();
        ofSetColor(ofColor::black);
        ofDrawCircle(point.x,point.y,5.0f);
        ofFill();
        ofSetColor(ofColor::white);
        ofDrawCircle(point.x,point.y,4.5f);
    }

    for(int i=0;i<gradientControlPoints.size();i++){
        ofVec2f point = gradientControlPoints[i].getLocation();
        ofSetColor(ofColor::black);
        ofFill();
        ofDrawCircle(point.x,point.y,4.0f);
    }
    ofSetColor(ofColor::white);
   // ofPopMatrix();
}

/**
 * Add default control values for min, max and middle value
 */
void TFEditor::addDefaultControlPoints(){

    colorControlPoints.clear();
    int colorIndex = ofMap(minVal,minVal,maxVal,0,colors.size()-1);
    ColorControlPoint minPoint(ofVec2f(colorPanelPosX,colorPanelPosY + colorMapHeight/2),minVal,colors[colorIndex]);
    colorIndex = ofMap(maxVal,minVal,maxVal,0,colors.size()-1);
    ColorControlPoint maxPoint(ofVec2f(colorPanelPosX+colorMapWidth,colorPanelPosY+colorMapHeight/2),maxVal,colors[colorIndex]);
    colorIndex = ofMap((minVal+maxVal)/2,minVal,maxVal,0,colors.size()-1);
    ColorControlPoint midPoint(ofVec2f(colorPanelPosX + colorMapWidth/2,colorPanelPosY+colorMapHeight/2),(minVal+maxVal)/2,colors[colorIndex]);
    colorControlPoints.push_back(minPoint);
    colorControlPoints.push_back(midPoint);
    colorControlPoints.push_back(maxPoint);

    opacityControlPoints.clear();
    OpacityControlPoint min_point(ofVec2f(opacityPanelPosX,opacityPanelPosY+opacityPanelHeight),minVal,0);
    OpacityControlPoint max_point(ofVec2f(opacityPanelPosX+opacityPanelWidth,opacityPanelPosY),maxVal,1);
    opacityControlPoints.push_back(min_point);
    opacityControlPoints.push_back(max_point);


        addDefaultGradientControlPoints();

}

void TFEditor::addDefaultGradientControlPoints() {
    gradientControlPoints.clear();
    GradientOpacityPoint min_grad_point(ofVec2f(gradientPanelX, gradientPanelY + gradientPanelHeight), minGradVal,
                                        0);
    GradientOpacityPoint max_grad_point(ofVec2f(gradientPanelX + gradientPanelWidth, gradientPanelY), maxGradVal,
                                        1.0);
    gradientControlPoints.push_back(min_grad_point);
    gradientControlPoints.push_back(max_grad_point);
}
void TFEditor::setup() {

        this->scalars = this->vrThread->getScalars();
        std::sort(this->scalars.begin(),this->scalars.end());
        vector<float>::iterator it = std::min_element(this->scalars.begin(),this->scalars.end());
        this->minVal = *it;
        it = std::max_element(this->scalars.begin(),this->scalars.end());
        this->maxVal = *it;
        this->currentColorMap=0;
        this->loadCounts();
        opacityPanelHeight = 150;
        opacityPanelWidth = ofGetWindowWidth()-50;
        opacityPanelPosX = 30;
        opacityPanelPosY = 175;
        this->dataSetLoaded = false;
        colorPanelPosX = 30;
        colorPanelPosY = 350;
        colorMapWidth = ofGetWindowWidth() - 50;
        colorMapHeight = 30;
        this->loadColors(this->currentColorMapFile.c_str());
        gradientPanelX = 30;
        gradientPanelY = colorPanelPosY+colorMapHeight+30;
        gradientPanelWidth = ofGetWindowWidth()-50;
        gradientPanelHeight = 120;


           setupGradients();



        addDefaultControlPoints();
}

/**
 * Add the user generated control points to VTK volume mapper
 */
void TFEditor::addControlPointsToVTK() {
   addColorPointsToVTK();
    addOpacityPointsToVTK();

        addGradientPointsToVTK();
}

void TFEditor::addColorPointsToVTK() {
    vrThread->color->RemoveAllPoints();
    for(int i=0;i<colorControlPoints.size();i++){
        ColorControlPoint ccp = colorControlPoints[i];
        ofFloatColor clr = Utils::getNormalizedColor(ccp.getColor());
        //cout<<(int)clr.r<<","<<(int)clr.g<<","<<(int)clr.b<<endl;
        int res = vrThread->color->AddRGBPoint(ccp.getValue(),clr.r,clr.g,clr.b);
        //cout<<res<<endl;
    }
    vrThread->update();
}

void TFEditor::addOpacityPointsToVTK() {
    vrThread->compositeOpacity->RemoveAllPoints();
    for(int i=0;i<opacityControlPoints.size();i++){
        OpacityControlPoint ocp = opacityControlPoints[i];
        vrThread->compositeOpacity->AddPoint(ocp.getValue(),ocp.getAlpha());
    }
    vrThread->update();
}

/**
 *
 * @param x - x coordinate of user click
 * @param y - y coordinate of user click
 * @param selectedIndex - variable to set if user selected an existing color control point
 * @return - true or false
 */
bool TFEditor::findColorPoint(int x, int y, int& selectedIndex) {

    int midPointY = colorPanelPosY + colorMapHeight/2;
    for(int i=0;i<colorControlPoints.size();i++){
        ofVec2f loc = colorControlPoints[i].getLocation();
        if(abs(loc.x - x)<=3 && abs(loc.y-y)<=3) // tolerance of 3 pixels
        {
            selectedIndex = i;
            return true;
        }
    }
    return false;
}

void TFEditor::drawOpacityControlLines() {
    std::sort(opacityControlPoints.begin(),opacityControlPoints.end(),OpacityControlPoint::compare);
    ofSetColor(ofColor::black);
    for(int i=0;i<opacityControlPoints.size()-1;i++){
        ofVec2f p1 = opacityControlPoints[i].getLocation();
        ofVec2f p2 = opacityControlPoints[i+1].getLocation();

        ofDrawLine(p1.x,p1.y,p2.x,p2.y);
    }


        std::sort(gradientControlPoints.begin(), gradientControlPoints.end(),GradientOpacityPoint::compare);
        for (int i = 0; i < gradientControlPoints.size() - 1; i++) {
            ofVec2f p1 = gradientControlPoints[i].getLocation();
            ofVec2f p2 = gradientControlPoints[i + 1].getLocation();

            ofDrawLine(p1.x, p1.y, p2.x, p2.y);
        }


    ofSetColor(ofColor::white);
}

bool TFEditor::findOpacityPoint(int x, int y, int &selectedIndex) {
    if(x>=opacityPanelPosX - 3 && x<=opacityPanelPosX+opacityPanelWidth+3 && y>=opacityPanelPosY-3 && y<=opacityPanelPosY+opacityPanelHeight+3){
        for(int i=0;i<opacityControlPoints.size();i++){
            ofVec2f loc = opacityControlPoints[i].getLocation();
            if(abs(x-loc.x)<=3 && abs(y-loc.y)<=3)
            {
                selectedIndex = i;
                return true;
            }
        }
    }
    return false;
}

void TFEditor::loadGradientCounts() {
    for(int i=0;i<gradientMagnitudes.size();i++){
        float val = gradientMagnitudes[i];
        if(gradientCounts.find(val)!=gradientCounts.end()){
            gradientCounts[val]++;
        }
        else{
            gradientCounts[val] = 1;
        }
    }

    minGradCount = INFINITY;
    maxGradCount = -1;
    for(map<float,int>::iterator it= gradientCounts.begin();it!=gradientCounts.end();it++){
        int count = it->second;
        if(count > maxGradCount)
            maxGradCount = count;
        if(count < minGradCount)
            minGradCount = count;
    }
    maxGradCount = log(maxGradCount);
    minGradCount = log(minGradCount);

}

void TFEditor::drawGradientOpacityPanel() {
    ofPushMatrix();
    ofTranslate(gradientPanelX,gradientPanelY);
    ofSetColor(ofColor::white);
    ofFill();
    ofDrawRectangle(0,0,gradientPanelWidth,gradientPanelHeight);
    ofPopMatrix();
}

void TFEditor::drawGradientHistogram() {
    ofPushMatrix();
    ofTranslate(gradientPanelX,gradientPanelY);
    int x_pos,y_pos;
    int totalVals = gradientCounts.size();
    float barWidth = (float)gradientPanelWidth/(float)totalVals;
    float xPos = 0;
    for(map<float,int>::iterator it=gradientCounts.begin();it!=gradientCounts.end();it++){
        float count = log(it->second);
        float height = ofMap(count,minGradCount,maxGradCount,0,gradientPanelHeight);
        float yPos = gradientPanelHeight - height;
        ofPushMatrix();
        ofTranslate(xPos,yPos);
        ofNoFill();
        ofSetLineWidth(1.0f);
        ofSetColor(ofColor::grey,50);
        ofDrawRectangle(0,0,barWidth,height);
        ofSetColor(ofColor::white);
        ofFill();
        ofPopMatrix();
        xPos+=barWidth;
    }

    ofPopMatrix();
}

void TFEditor::addGradientPointsToVTK() {
    vrThread->gradientOpacity->RemoveAllPoints();

    for(int i=0;i<gradientControlPoints.size();i++){
        GradientOpacityPoint point = gradientControlPoints[i];
        vrThread->gradientOpacity->AddPoint(point.getGradient(),point.getAlpha());
    }
    vrThread->update();
}

bool TFEditor::findGradientPoint(int x, int y, int &selectedIndex) {
    if(x >= gradientPanelX-3 && x <=gradientPanelX+3 + gradientPanelWidth && y >= gradientPanelY-3 && y <= gradientPanelY + gradientPanelHeight+3){
        for(int i=0;i<gradientControlPoints.size();i++){
            GradientOpacityPoint point = gradientControlPoints[i];
            if(abs(x - point.getLocation().x)<=4 && abs(y - point.getLocation().y)<=4)
            {
                selectedIndex = i;
                return true;
            }
        }
    }
    return false;
}

void TFEditor::setupGradients() {
    this->gradientMagnitudes = vrThread->getGradients();
    this->gradientCounts.clear();
    vector<float>::iterator it = std::max_element(gradientMagnitudes.begin(), gradientMagnitudes.end());
    maxGradVal = *it;
    it = std::min_element(gradientMagnitudes.begin(), gradientMagnitudes.end());
    minGradVal = *it;
    loadGradientCounts();
}
