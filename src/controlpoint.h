#pragma once
//
// Created by srmocher on 4/15/17.
//

#include "ofMain.h"
#ifndef A06P01_CONTROLPOINT_H
#define A06P01_CONTROLPOINT_H

#endif A06P01_CONTROLPOINT_H



class ControlPoint
{
    protected:
        ofVec2f location;
        float scalarValue;

public:
    ControlPoint(){}
    ControlPoint(ofVec2f,float);
    float getValue(){return scalarValue;}
    
};

class ColorControlPoint:ControlPoint
{
private:
    ofColor color;
public:
   
    ColorControlPoint(ofVec2f loc,float val,ofColor color);
    ofVec2f getLocation(){return location;}
    ofColor getColor(){return color;}
    float getValue(){return scalarValue;}
    void setLocation(ofVec2f loc){this->location=loc;}
    void setValue(float val){this->scalarValue = val;}
    void setColor(ofColor color){this->color = color;}
     static bool compare(const ColorControlPoint &first,const ColorControlPoint &other)
	{
        return first.scalarValue < other.scalarValue;
	}


};

class OpacityControlPoint:ControlPoint
{
private:
    float alpha;
public:
    OpacityControlPoint(ofVec2f,float,float);
    ofVec2f getLocation(){return location;}
    float getAlpha(){return alpha;}
    float getValue(){return scalarValue;}
    void setLocation(ofVec2f loc){this->location=loc;}
    void setValue(float val){this->scalarValue = val;}
    void setAlpha(float alpha){this->alpha=alpha;}
    static bool compare(const OpacityControlPoint &first,const OpacityControlPoint &other)
    {
        return first.scalarValue < other.scalarValue;
    }
    
};

class GradientOpacityPoint:ControlPoint
{
private:
    float alpha;
    float gradientMagnitude;
public:
    GradientOpacityPoint(ofVec2f,float,float);
    ofVec2f getLocation(){return location;}
    float getGrad(){return gradientMagnitude;}
    float getAlpha(){return alpha;}
    float getGradient(){return this->gradientMagnitude;}
    void setLocation(ofVec2f loc){this->location=loc;}
    void setGradient(float val){this->gradientMagnitude = val;}
    void setAlpha(float alpha){this->alpha=alpha;}
    static bool compare(const GradientOpacityPoint &first,const GradientOpacityPoint &other)
    {
        return first.gradientMagnitude < other.gradientMagnitude;
    }
    
};
