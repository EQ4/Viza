//
//  BaseFileLoader.h
//  ViZa
//
//  Created by martin hermant on 07/04/15.
//
//

#ifndef __ViZa__BaseFileLoader__
#define __ViZa__BaseFileLoader__

#include "ofMain.h"
#include "Container.h"
#include "Poco/Task.h"
#include "Poco/Notification.h"
#include "Poco/Random.h"
#include <regex>




class BaseFileLoader : public Poco::Task{
    
public:
    
    typedef map<string,BaseFileLoader*(*)(const string & s)> loaders_map_type;
    
    static BaseFileLoader* getLoader(string const & extension,string const & name);
    static loaders_map_type * getMap();
    
    BaseFileLoader(const std::string& name);
    virtual ~BaseFileLoader();
    vector<string> extensions;

    
    //filled only first time as it's a coherent database
    typedef struct {
        
        vector<string> attributeNames;
        
        // contain Viza-added Attribute names : length, start idx, relativeStartidx
        bool hasVizaMeta = false;
        unsigned int totalContainers;
        

        
    }GlobalInfo;
    
    typedef struct {
        unsigned int containerIdx;
        unsigned int songIdx;
        string parsedFile;
        int numElements;
        Container::SongMeta song;

        
    }ContainerBlockInfo;
    
    
    virtual bool getCachedInfo(const string & annotationdir)=0;
    virtual bool fillContainerBlock(const string & annotationpath) = 0;
    virtual bool hasCachedInfo(const string & annotationpath) = 0;
    virtual bool cacheInfo(const string & annotationpath) = 0;
    
    
    ContainerBlockInfo containerBlock;
    GlobalInfo globalInfo;
    
    static void linkLoaders();
    
    void runTask();
    
    
    static vector<string> attrSubset;
    static string audioFolderPath;
    static bool init;
    
    string searchAudiofromAnal(const string & s,const string & audioFolder);

protected:
    
    // return actual number of containerCreated
    virtual int loadFile() = 0;
    
    static  loaders_map_type * loadersMap;
    
    void setSongInfo();
    
};

template<typename T> BaseFileLoader * createT(const string & s) { return new T(s); }




#endif /* defined(__ViZa__BaseFileLoader__) */
