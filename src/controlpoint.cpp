//
// Created by srmocher on 4/15/17.
//

#include "controlpoint.h"

ControlPoint::ControlPoint(ofVec2f point, float val) {
    this->location = point;
    this->scalarValue = val;
}

ColorControlPoint::ColorControlPoint(ofVec2f loc, float val, ofColor color) {
    ControlPoint(loc,val);
    this->location = loc;
    this->scalarValue = val;
    this->color = color;
}

OpacityControlPoint::OpacityControlPoint(ofVec2f loc, float val, float alpha) {
    ControlPoint(loc,val);
    this->location = loc;
    this->scalarValue = val;
    this->alpha = alpha;
}

GradientOpacityPoint::GradientOpacityPoint(ofVec2f loc, float grad, float alpha) {
    this->location = loc;
    this->gradientMagnitude = grad;
    this->alpha = alpha;
}
