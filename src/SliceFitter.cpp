//
//  SliceFitter.cpp
//  ViZa
//
//  Created by martin hermant on 16/12/14.
//
//

#include "SliceFitter.h"


#include "Physics.h"
#include "Container.h"

#include <Accelerate/Accelerate.h>

SliceFitter* SliceFitter::inst;

vector<string> SliceFitter::DistanceType::types;
const int dumb = (0,
    SliceFitter::DistanceType::types.push_back("Euclidian"),
    SliceFitter::DistanceType::types.push_back("Angular"),
    SliceFitter::DistanceType::types.push_back("Binary"),
    0);



SliceFitter::SliceFitter(){
    
    ofAddListener(ofEvents().update , this, &SliceFitter::update);
    //get a list of all algorithms we could use for the test
    
    type.idx = 0;
    
}
SliceFitter::~SliceFitter(){
    if(fitThread.fitter)fitThread.fitter->forceStop();
    fitThread.waitForThread(true);
}



void SliceFitter::fitFor(float s){
    //   fitting function
    
    int elemSize = samplePct* Physics::vs.size();
    int dimSize = Container::attributeNames.size();
    
    // choose random pairs of point to build dataset
    // the data is the vector of differences of each pair
    
    if(!keepResult|| fitThread.model==NULL){
        
        if(fitThread.model!=NULL ){
            delete fitThread.model;
        }
        fitThread.model = new FitThread::Model();
    }
    fitThread.model->type = type.idx;
    
    fitThread.model->size = dimSize;
    int paramSize = fitThread.model->getParameterCount();
    
    
    
    fitThread.dataset.resize(elemSize);
    for(int i = 0 ; i < elemSize ; i ++){
        int i2 ;
        if(Container::classeMap.count("Cluster")>0){
            vector<string> v;
            map<string,vector<unsigned int> > * m = &Container::classeMap["Cluster"];
            int id = (int)ofRandom(m->size()-1);
            for(map<string,vector<unsigned int> >::iterator it = m->begin(); it != m->end(); ++it) {
                if((id--)<0){
                    i2 = it->second[ofRandom(it->second.size()-1)];
                    break;
                }
                
                
            }
            
        }
        else {
         i2 = (int)ofRandom(Physics::vs.size()-1);   
        }
        ofVec3f d = Physics::vs[i%Physics::vs.size()]- Physics::vs[i2];
        ofxNonLinearFit::Models::Data dc(d);
        fitThread.dataset[i].angle =   fitThread.model->getAngle(dc);
        fitThread.dataset[i].descriptorsDiff.resize(dimSize);
        for(int k = 0 ; k< dimSize ; k++){
            fitThread.dataset[i].descriptorsDiff[k] = Container::normalizedAttributes[i2*Container::attrSize + k] - Container::normalizedAttributes[i%Physics::vs.size()*Container::attrSize+k];
        }
//        vDSP_vsub(&Container::normalizedAttributes[i2*Container::attrSize],1,&Container::normalizedAttributes[i%Physics::vs.size()*Container::attrSize],1,&fitThread.dataset[i].descriptorsDiff[0],1,dimSize);
        for(int k = 0 ; k< Container::fixAttributes.size() ; k++){
            fitThread.dataset[i].descriptorsDiff[Container::fixAttributes[k]]= 10;
        }
    }
    
    
    

    
    // init search space
    if(!keepResult){
        vector<double> randParam(paramSize);
        
        for(int i = 0 ; i <paramSize ; i ++){
            randParam[i] = ofRandom(.01);
            
        }
        
        fitThread.model->setParameters(&randParam[0]);
    }
    fitThread.init();
    fitThread.fitter->upperBound = 1;
    fitThread.fitter->lowerBound = -1;
    
    fitThread.fitter->maxTime = s;
    fitThread.fitter->stopval = pow(.1 ,2);
    cout << "stopval : " << fitThread.fitter->stopval << endl;
    if(fitThread.model->size>0){
        fitThread.startThread();
    }
    else{
        cout << "no parameters to fit" << endl;
    }
    
}


void SliceFitter::update(ofEventArgs &a){
    
    
    if(fitThread.isThreadRunning() &&fitThread.model!=NULL && fitThread.model->getParameters()!=NULL ){
        int totalNum = Physics::vs.size();
        if(totalNum!= outPoints.size()){
            outPoints.resize(totalNum);
            
            cout << "resize to" << totalNum << endl;
        }
        curParams.resize(fitThread.model->getParameterCount());
        for(int  i = 0 ; i < fitThread.model->getParameterCount();i++){
            curParams[i] = fitThread.model->getParameters()[i];
        }
        vDSP_mmul(&Container::normalizedAttributes[0],1,&curParams[0],1,&outPoints[0].x,1,totalNum,3,Container::attrSize);
        
        outPointsReshape();
        
        Physics::setFits(outPoints);
        
        
        //    }
        
        
        
        
        
        //print out report
        
        
        fitEquation.clear();
        fitEquation.paramNames = Container::attributeNames;
        
        // iterate over parameters :  saving equation
        for(int i = 0 ; i < fitThread.model->size ; i++){
            for(int j = 0 ; j< 3 ; j++){
                float curParam = fitThread.model->getParameters()[i*3+j];
                if(abs(curParam)>0){
                    fitEquation.equation[j].push_back(FitEquation::eqElem(i,curParam));
                }
            }
            
            
            
        }
        GUI::instance()->LogIt(ofToString(fitThread.fitter->lowerResidual/(samplePct*Container::attrSize))+"\n"+fitEquation.toString(3));
        if(!fitThread.isThreadRunning() && fitThread.ended == true){
            cout << ofToString(fitThread.fitter->lowerResidual/(samplePct*Container::attrSize))+"\n"+fitEquation.toString(3) << endl;
            fitThread.clear();}
    }
}







void SliceFitter::outPointsReshape(){
    
    
    
    ofVec3f scale(0,0,0);
    int numElements = 400;
    if(type.idx>0){
    int validEl=0;
    int maxWatch = 0;
    while(validEl < numElements && maxWatch < Physics::vs.size()){
        int curIdx = ofRandom(Physics::vs.size()-1);
        int curIdx2 = (int)(curIdx + ofRandom(Physics::vs.size()-1))%(Physics::vs.size()-1);
        
        
        ofVec3f scaleNum = (Physics::vs[curIdx] - Physics::vs[curIdx2]);
        ofVec3f scaleDen = (outPoints[curIdx]-outPoints[curIdx2]);
        if(
           (scaleNum.x*scaleDen.x)!=0 &&
           (scaleNum.y*scaleDen.y)!=0 &&
           (scaleNum.z*scaleDen.z)!=0
           
           
           ){
            scale.x+=abs(scaleNum.x/scaleDen.x);
            scale.y+=abs(scaleNum.y/scaleDen.y);
            scale.z+=abs(scaleNum.z/scaleDen.z);
            validEl++;
        }
        maxWatch++;
    }
    
    scale/= validEl;
    }
    else{
    
    scale.set(1,1,1);
    }
    
    ofVec3f translation(0,0,0);
    for(int i = 0 ; i < numElements ;i++){
        int curIdx = ofRandom(Physics::vs.size()-1);
        translation+=Physics::vs[curIdx] - outPoints[curIdx]*scale;
    }
    
    translation/= numElements;
    
    //cout << "scale" <<  scale << endl;
    vDSP_vsmsa(&outPoints[0].x, 3, &scale.x, &translation.x, &outPoints[0].x, 3, outPoints.size());
    vDSP_vsmsa(&outPoints[0].y, 3, &scale.y, &translation.y, &outPoints[0].y, 3, outPoints.size());
    vDSP_vsmsa(&outPoints[0].z, 3, &scale.z, &translation.z, &outPoints[0].z, 3, outPoints.size());
    
    
    
    
}




