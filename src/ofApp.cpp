#include "ofApp.h"
//#include <omp.h>
#include <stdio.h>
ofEasyCam ofApp::cam;
ofVec3f ofApp::scrS;



//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(70);
    ofEnableAlphaBlending();
    //    ofDisableSmoothing();
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofDisableDepthTest();
    
    
    setcamOrtho(true);
    cam.reset();
    cam.setTranslationKey('a');
    cam.setZoomKey('s');
    
    Casttime=ofGetElapsedTimeMillis();
    
    
    loadFiles();
    
    
    ofBackground(0);
    
    ofShowCursor();
    
    
    
    
    
    
    GUI::instance()->isModifiying.addListener(this, &ofApp::isGUIing);
    AudioPlayer::instance();
    windowResized(ofGetWindowWidth(), ofGetWindowHeight());
    lastCamPos =cam.getPosition();
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    Midi::update();
    
    if((cam.getPosition()-lastCamPos).length()>0 ){
        isCamSteady = false;
        if(ofGetElapsedTimef()-lastCamUpdate>.3){
            Physics::updateVScreen();
            lastCamUpdate = ofGetElapsedTimef();
        }
    }
    else if (!isCamSteady ){
        Physics::updateVScreen();
        isCamSteady = true;
        //        cout << "steadyCam" << endl;
    }
    lastCamPos = cam.getPosition();
    
    
    
}

void ofApp::loadFiles(string audiopath,string segpath){
    
    
    AudioPlayer::UnloadAll();
    Container::clearAll();
    jsonLoader::instance()->loadSegments(audiopath,segpath);
    Physics::resizeVBO();
    GUI::instance()->registerListener();
    GUI::instance()->setup();
    Container::registerListener();
    for(map<string,vector<Container* > >::iterator it = Container::songs.begin() ; it != Container::songs.end() ; ++it ){
        for(int i = 0 ; i <POLYPHONY ; i++){
            //           AudioPlayer::Load(*it->second[i], true);
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    cam.begin();
    
    Physics::draw();
    Midi::draw();
    drawMire();
    
    cam.end();
    
    if(abs(selectRect.width)>0){
        ofSetColor(0,0,255,80);
        ofRect(selectRect);
    }
    drawCam();
    
}

void ofApp::drawCam(){
    
    ofPushMatrix();
    ofPushView();
    ofEnableDepthTest();
    ofMatrix4x4 t ;
    ofMatrix4x4 ortho ;
    float angle;
    ofVec3f v;
    
	ortho.makeOrthoMatrix(-scrS.x/2,    scrS.x/2   , -scrS.y/2, scrS.y/2, .1, 2000);
    t.makeTranslationMatrix(0,0,-1000);
    
    ofSetMatrixMode(OF_MATRIX_PROJECTION);
	ofLoadMatrix( ortho);
	ofSetMatrixMode(OF_MATRIX_MODELVIEW);
	ofLoadMatrix( t);
    
    
    cam.getOrientationQuat().getRotate(angle, v);
    
    
    
    ofTranslate(scrS.x/2 - 90,-scrS.y/2 + 90,0);
    
    ofRotate(angle,v.x,-v.y,v.z);
    
    
    ofDrawAxis(50);
    
    
    
    ofPopMatrix();
    ofPopView();
}


void ofApp::setcamOrtho(bool t){
    
    
    
    
    
    
    if(t){
        cam.enableOrtho();
        cam.setNearClip(.000001f);
        
        
        cam.setScale(1.0/ofGetWindowHeight());
        cam.setDistance(1);//ofGetScreenHeight());
        scrS.z = cam.getDistance()*2*ofGetWindowHeight();
        cam.setFarClip(scrS.z);
        cam.setNearClip(.000001f);
        cam.setLensOffset(ofVec2f(-1,-1));
        cam.orbit(0,0, cam.getDistance());
        cam.setAutoDistance(false);
        cam.enableMouseInput();
        cam.disableMouseMiddleButton();
        Container::radius = 300;
        glPointSize(Container::radius);
        GLfloat attPoints[] = {0,Physics::distanceVanish(cam),0};//*,0};
        
        glPointParameterfv(	GL_POINT_DISTANCE_ATTENUATION,&attPoints[0]);
        
    }
    else{
        cam.disableOrtho();
        
        cam.setScale(1);
        cam.setNearClip(.000001f);
        cam.setDistance(1);
        scrS.z = cam.getDistance()*3;
        cam.setFarClip(scrS.z);
        cam.setFov(2*ofRadToDeg(atan(.5/cam.getDistance())));
        cam.orbit(0,0, cam.getDistance());
        cam.setAutoDistance(false);
        cam.setLensOffset(ofVec2f(0,0));
        cam.enableMouseInput();
        cam.disableMouseMiddleButton();
        Container::radius = 20;
        glPointSize(Container::radius);
        GLfloat attPoints[] = {0,Physics::distanceVanish(cam),0};//*,0};
        
        glPointParameterfv(	GL_POINT_DISTANCE_ATTENUATION,&attPoints[0]);
        
        
    }
    
}

void ofApp::drawMire(){
    ofPushMatrix();
    ofPushStyle();
    
    //    if(ofApp::cam.getOrtho())ofScale(1.0/ofApp::cam.getDistance(),1.0/ofApp::cam.getDistance(),1.0/ofApp::cam.getDistance());
    
    ofNoFill();
    ofSetLineWidth(1);
    ofSetCircleResolution(60);
    
    
    
    
    ofSetColor(0,0,255);
    ofCircle(ofVec3f(0),.5);
    
    
    ofRotateX(90);
    ofSetColor(0,255,0);
    ofCircle(ofVec3f(0),.5);
    
    
    ofRotateY(90);
    ofSetColor(255,0,0);
    ofCircle(ofVec3f(0),.5);
    
    
    
    ofPopStyle();
    ofPopMatrix();
    
    ofPushMatrix();
    for (int i = 0 ; i < 3 ; i++){
        ofVec3f mask(i==0?255:0,i==1?255:0,i==2?255:0);
        ofSetColor (mask.x,mask.y,mask.z);
        ofDrawBitmapString(GUI::instance()->attr[i]->getSelected()[0]->getName(), .45/255.*mask);
    }
    ofPopMatrix();
}



void ofApp::isGUIing(bool & t){
    //    if(t){
    //        cam.disableMouseInput();
    //
    //    }
    //    else{
    //        cam.enableMouseInput();
    //
    //    }
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key) {
        case 'x':
            cam.orbit(-90,0,cam.getDistance());
            break;
        case 'y':
            cam.orbit(0,90,cam.getDistance());
            break;
        case 'z':
            cam.orbit(0,0,cam.getDistance());
            break;
            
        case 'l':{
            ofFileDialogResult f = ofSystemLoadDialog("analysisFiles",true);
            loadFiles("",f.filePath);
            break;
        }
        case ' ':
            ofFmodSoundStopAll();
            break;
            
        case 'f':
            if(fit.fitThread.isThreadRunning()){
                fit.fitThread.fitter->forceStop();
            }
            else
                fit.fitFor();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    if(ofGetElapsedTimeMillis()-Casttime>10){
        if(isSelecting){
            
        }
        else{
            Container * cc = Physics::hoveredOnScreen( ofVec3f(x,y,0));
            bool change = Container::hoverContainer(cc == NULL?-1:cc->index);
            Casttime = ofGetElapsedTimeMillis();
            if (change)GUI::LogIt(cc == NULL?"":cc->filename +" : "+ ofToString((cc->getPos()*(Physics::maxs.get()-Physics::mins)+Physics::mins)));
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    // select instances
    if(isSelecting){
        
        selectRect.width =  x -selectRect.x;
        selectRect.height=  y -selectRect.y;
//        selectRect.standardize();
        
    }
    
    // drag instances
    else if(button==1){
        if(Physics::updateDrag(ofVec2f(x,y))){
            
        }
        
    }
    
    // play instances
    else if (button ==2 && GUI::instance()->continuousPB->getValue()){
        if(ofGetElapsedTimeMillis()-Casttime>70){
            Container * cc = Physics::nearestOnScreen( ofVec3f(x,y,0));
            int oldIdx = Container::hoverIdx;
            bool change = Container::hoverContainer(cc == NULL?-1:cc->index);
            Casttime = ofGetElapsedTimeMillis();
            if (change){
                GUI::LogIt(cc == NULL?"":cc->filename +" : "+ ofToString((cc->getPos()*(Physics::maxs.get()-Physics::mins)+Physics::mins)));
                cc->state =1;
                if(oldIdx>=0 && !GUI::instance()->holdPB->getValue())Container::containers[oldIdx].state=0;
            }
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    // start selecting multiple
    if(ofGetKeyPressed('d') && button == 1){
        Physics::dragged.clear();
        selectRect.set(x,y,0,0);
        cam.disableMouseInput();
        isSelecting = true;
        selectRect.x = x;
        selectRect.y = y;
        return;
    }
    else{
        isSelecting = false;
    }
    
    Container * cc = Container::hoverIdx!=-1? &Container::containers[Container::hoverIdx]:NULL;// if (cc == NULL) {cout<< "error : no Container hovered" << endl;return;}
    cout << Container::hoverIdx << endl;
    // play
    if(button == 2 && cc)cc->state =1;//cc->state==0?1:0;
    
    // drag
    if(button == 1){
        
        // will drag multiple
        if (selectRect.inside(x,y)){
            
            Physics::originDrag.clear();
            
            for(int i = 0 ; i < Physics::dragged.size() ; i++){
                int curI = Physics::dragged[i]->index;
                Physics::originDrag.push_back(cam.worldToScreen(Physics::vs[curI]) - ofVec2f(x,y));
            }
            selectRect.set(x,y,0,0);
            
        }
        
        
        
        
        
        else{
            selectRect.set(x,y,0,0);
            Physics::dragged.clear();
        // one selected
        if(cc){
        
                 Physics::originDrag.clear();
            Physics::dragged.push_back(cc);
            ofVec3f screenD = cam.worldToScreen(Physics::vs[cc->index])-ofVec3f(x,y);
            Physics::originDrag.push_back(screenD);
        }
        
            
        }
        
        
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if(isSelecting){
        selectRect.width =  x - selectRect.x;
        selectRect.height =  y -selectRect.y;
        selectRect.standardize();
        Physics::dragged = Physics::containedInRect(selectRect);
        
        
        isSelecting = false;
        cam.enableMouseInput();
    }
    
    Physics::updateVScreen();
    //    Physics::dragged.clear();
    //    cam.enableMouseMiddleButton();
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
    setcamOrtho(cam.getOrtho());
    scrS.x =w;
    scrS.y =h;
    scrS.z = cam.getFarClip()-cam.getNearClip();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}


float ofApp::toCamZ(float z){
    float nZ = (cam.getDistance()-z)/cam.getScale().z;
    return ofMap(nZ,cam.getNearClip(), cam.getFarClip(), -1, 1);
}


