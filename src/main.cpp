//
//  main.cpp
//  A06 TestVR Example
//
//  Created by Joshua Levine on 04/02/17
//  email: josh@email.arizona.edu
//

/*
 ***********************************************************************
 
 Copyright (C) 2017, Joshua A. Levine
 University of Arizona
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 ***********************************************************************
 */

#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
	ofGLWindowSettings settings;
  //cout << settings.glVersionMajor << " " << settings.glVersionMinor << endl;
	//settings.setGLVersion(3,2);
	settings.width = 800;
	settings.height = 800;
	settings.setPosition(ofVec2f(300,0));
	//settings.resizable = true;
	ofCreateWindow(settings);

  //We manually create the window above to have some control
  //over the openGL version, if needed.
	//ofSetupOpenGL(400,300,OF_WINDOW);			// <-------- setup the GL context

  shared_ptr<ofApp> ofa(new ofApp);

	// this kicks off the running of my app
	ofRunApp(ofa);
}
