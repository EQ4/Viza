//
//  GUICluster.cpp
//  ViZa
//
//  Created by martin hermant on 24/04/15.
//
//

#include "GUICluster.h"


GUICluster::GUICluster(string name): GUICanvasBase(name){
    
    
    ////////////////CLUSTER </////////////////////

    setName("Cluster");
    findClusters = new ofxUIButton("findClusters",false,10,10);
    clusterEps = new ofxUISlider("Epsilon",.01,.3,.05,100,10);
    clusterMinK = new ofxUISlider("MinK",2,100,10,100,10);
    minClusterSize = new ofxUISlider("minClusterSize",1,500,10,100,10);
    
    tSNE2D = new ofxUIToggle("tSNE2d",true,10,10);
    tSNEPerp = new ofxUISlider("tSNEPerplexity",.5,65,30,100,10);
    tSNEtheta = new ofxUISlider("tSNEtheta",.0,.49,0.,100,10);
    findtSNE = new ofxUIButton("tSNE",false,10,10);
    applyTsne = new ofxUIButton("apply",false,10,10);
    
    
    addWidgetDown(findClusters);
    addWidgetDown(clusterMinK);
    addWidgetDown(clusterEps);
    addWidgetDown(minClusterSize);
    addSpacer();
    addLabel("t-SNE");
    addSpacer();
    addWidgetDown(tSNEtheta);
    addWidgetDown(tSNEPerp);
    addWidgetDown(tSNE2D);
    addWidgetDown(findtSNE);
    addWidgetDown(applyTsne);
    
    setDimensions(getRect()->width, getRect()->height*1.3);
    
    
    
}


void GUICluster::init(){

}



void GUICluster::guiEvent(ofxUIEventArgs & e){
    if(e.widget == findClusters && !findClusters->getValue()){
        vector<int> classes(Physics::vs.size(),0);
        Physics::kNN.dbscan(&classes,  clusterMinK->getValue(), clusterEps->getValue(),minClusterSize->getValue());
        //            Physics::kNN.medoids(&classes,  clusterMinK->getValue(), clusterEps->getValue(),minClusterSize->getValue());
        Container::classeMap["Cluster"].clear();
        for(int i = 0 ; i < classes.size();i++){
            if(abs(classes[i])>0){
                Physics::updateOneColor(i, ofColor(Container::getColorForId(abs(classes[i])-1),255*Container::stateColor[0].a));
            }
            else{
                Physics::updateOneColor(i, ofColor(ofColor::white,5));
            }
            Container::containers[i]->setClass("Cluster", ofToString(abs(classes[i])));
        }
        Physics::updateVScreen();
    }
    
    if(e.widget == findtSNE && !findtSNE->getValue()){
        
        int dim = tSNE2D->getValue()?2:3;
       
        if(ofxTSNE::i()->isThreadRunning()){
            ofxTSNE::i()->stopThread();
        }
        else{
            ofxTSNE::i()->init(Container::normalizedAttributes, Container::attrSize, Container::containers.size(), tSNEtheta->getValue(), tSNEPerp->getValue(),dim);
            ofxTSNE::i()->startThread();
        }
    }
    else if (e.widget == applyTsne && !applyTsne->getValue()){
        if(Physics::applyFit()){
            int maxAxe = tSNE2D->getValue()?2:3;
            for(int i = 0 ; i < maxAxe ;i++){
                ofMessage msg("Axes:setLabels,"+ofToString(i)+",tSNE");
                //ofNotifyEvent(*sharedGUIEvent,msg,this);
            }
        }
    }
}


