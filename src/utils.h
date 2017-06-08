//
// Created by srmocher on 4/16/17.
//

#include "ofMain.h"

namespace Utils
{
    ofFloatColor getNormalizedColor(ofColor color)
    {
     //   cout<<(int)color.r<<endl;
        float r = ofMap(color.r,0,255,0.0f,1.0f);
        float g = ofMap(color.g,0,255,0.0f,1.0f);
        float b = ofMap(color.b,0,255,0.0f,1.0f);
      //  cout<<r<<","<<g<<","<<b<<endl;
        return ofFloatColor(r,g,b);
    }
}