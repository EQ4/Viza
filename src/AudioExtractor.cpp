//
//  AudioExtractor.cpp
//  ViZa
//
//  Created by martin hermant on 09/10/14.
//
//

#include "AudioExtractor.h"
#define NUM_BEST 30



vector<string> AudioExtractor::statsToCompute;
bool AudioExtractor::bEssentiaInited = false;

AudioExtractor::AudioExtractor(const std::string& name,bool isCaching):
BaseFileLoader(name,isCaching),
extr(nullptr){
    {
        
        ofScopedLock(staticMutex);
        if(!bEssentiaInited){
            bEssentiaInited =true;
            
        }
    }
    essentia::infoLevelActive = false;
    essentia::warningLevelActive = false;
    essentia::errorLevelActive = false;
    maxAnalysingThread = 4 ;
    maxImportingThread = 10 ;
    extensions = vector<string>();
    extensions.push_back(".wav");
    extensions.push_back(".mp3");
    //    essentia::setDebugLevel(essentia::DebuggingModule::EExecution);
    
    
    mapIt.name = "LowLevelSpectralExtractor";
    mapIt.inputName = "signal";
    //    mapIt.parameters.add("frameSize", 1024);
    //    mapIt.parameters.add("hopSize",512);
    //    mapIt.outputs.push_back("barkbands");
    mapIt.outputs.push_back("barkbands_kurtosis");
    mapIt.outputs.push_back("barkbands_skewness");
    mapIt.outputs.push_back("barkbands_spread");
    mapIt.outputs.push_back("hfc");
    //    mapIt.outputs.push_back("mfcc");
    mapIt.outputs.push_back("pitch");
    mapIt.outputs.push_back("pitch_instantaneous_confidence");
    mapIt.outputs.push_back("pitch_salience");
    //    mapIt.outputs.push_back("silence_rate_20dB");
    //    mapIt.outputs.push_back( "silence_rate_30dB");
    //    mapIt.outputs.push_back( "silence_rate_60dB");
    mapIt.outputs.push_back( "spectral_complexity");
    mapIt.outputs.push_back("spectral_crest");
    mapIt.outputs.push_back("spectral_decrease");
    mapIt.outputs.push_back("spectral_energy");
    mapIt.outputs.push_back("spectral_energyband_low");
    mapIt.outputs.push_back("spectral_energyband_middle_low");
    mapIt.outputs.push_back("spectral_energyband_middle_high");
    mapIt.outputs.push_back("spectral_energyband_high");
    mapIt.outputs.push_back("spectral_flatness_db");
    mapIt.outputs.push_back("spectral_flux");
    mapIt.outputs.push_back("spectral_rms");
    mapIt.outputs.push_back("spectral_rolloff");
    mapIt.outputs.push_back("spectral_strongpeak");
    mapIt.outputs.push_back("zerocrossingrate");
    mapIt.outputs.push_back("inharmonicity");
    //    mapIt.outputs.push_back("tristimulus");
    //mapIt.outputs.push_back("oddtoevenharmonicenergyratio");
    
    infos.push_back(mapIt);
    
    statsToCompute = vector<string>(1,"mean");
    
    if(isCaching){createExtractor();}
}

void AudioExtractor::createExtractor(){
    extr = new SimpleEssentiaExtractor(infos);
    extr->statsToCompute = statsToCompute;
    extr->initFile();
    extr->buildMe();
}

bool AudioExtractor::fillContainerBlock(const string  annotationPath){
    
    
    
    
    extr->setInput(containerBlock->parsedFile,"");
    extr->threadedFunction();
    
    vector<Real> onsets = extr->aggregatedPool.value<vector<Real>>("onsets");
    containerBlock->numElements = onsets.size();
    
    
    // Save to json to avoid to much heap memory alloc before loadFiles
    

   //getAbsolutePath();

    string destinationFile = getParsedFileCache(containerBlock->parsedFile);
    ofFile(destinationFile).create();
    ofLogWarning("FileLoader") << "saving JSON at : " << destinationFile;
    extr->saveIt(destinationFile);
    containerBlock->parsedFile = destinationFile;
    
    
    
    extr->aggregatedPool.clear();
    
    //    delete extr;
}

string AudioExtractor::getParsedFileCache(const string & file){
    ofFile pFile(file);
    string destinationFile = ofFilePath::join(pFile.getEnclosingDirectory(),"Viza");
    destinationFile = ofFilePath::join(destinationFile, pFile.getBaseName() + ".json");
    return destinationFile;
}

int AudioExtractor::loadFile(){
    
    ofxJSONElement json;
    json.open(containerBlock->parsedFile);
    
    vector<std::pair<float,float>> onsets;
    for(int i = 0; i <  json["slice"]["time"].size() ; i++){
        onsets.push_back(std::pair<float,float>(json["slice"]["time"][i][0].asFloat(),json["slice"]["time"][i][1].asFloat()));
    };
    
    
    
    int containerNum = containerBlock->containerIdx;
    
    
    float begin = 0;
    float end = 0;
    float songLength = onsets.back().second;
    for(int i = 0 ; i < containerBlock->numElements ; i++){
        
        
        begin = onsets[i].first;
        end = onsets[i].second;
        Container::containers[containerNum] = new Container(begin,end,containerNum,0);
        
        
        for(auto & v:mapIt.outputs){
            string name = "values."+v+".mean";
            ofxJSONElement value = json["values"][v]["mean"].get(i, 0);
            if(!value.isNumeric()){
                ofLogError("AudioLoader")<<" No feature found " << name;
                for (auto d:value){
                    ofLogError("AudioLoader") << "\t" << d.type();
                }
            }
            else{
                
                Container::containers[containerNum]->setAttribute(v,value.asFloat());
            }
        }
        
//        Container::containers[containerNum]->setAttribute("length",end-begin);
//        Container::containers[containerNum]->setAttribute("startTime",begin);
//        Container::containers[containerNum]->setAttribute("relativeStartTime",songLength!=0?begin/(songLength):0);
        
        containerNum++;
    }
    
    
    //    for(map<string,vector<string> >::iterator itc = classMap.begin() ; itc !=classMap.end() ; ++itc){
    //        Container::containers[containerNum]->setClass(itc->first, itc->second[sliceNum]);
    //    }
    //    delete res;
    return 1;
    
    
    
    
    
};





vector<string> AudioExtractor::getAttributeNames(const string & path){
    return mapIt.outputs;
    
    
}




