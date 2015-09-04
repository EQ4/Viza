//
//  FileUtils.h
//  ViZa
//
//  Created by martin hermant on 03/09/15.
//
//

#ifndef __ViZa__FileUtils__
#define __ViZa__FileUtils__


#include "ofMain.h"
#include <regex>

class FileUtils{
    public :
    // faster than OF  listDir() method because no ofFile instance
    static vector<std::filesystem::path> getFilePaths(const string & d, bool allowDir = false){
        vector<std::filesystem::path> res;
        
        
        std::filesystem::path myDir ( d);
        
        std::filesystem::directory_iterator end_iter;
        if ( std::filesystem::exists(myDir) && std::filesystem::is_directory(myDir)){
            for( std::filesystem::directory_iterator dir_iter(myDir) ; dir_iter != end_iter ; ++dir_iter){
                res.emplace_back(dir_iter->path());
            }
        }
        else{
            ofLogError("FileUtils","no file to list");
            return res;
        }
        
        
        return res;
        
    }
    
    static vector<std::filesystem::path> getFilePathsWithExt(const string & d,const string ext , bool allowDir = false){
        vector<std::filesystem::path> res;
        
        
        std::filesystem::path myDir ( d);
        
        std::filesystem::directory_iterator end_iter;
        if ( std::filesystem::exists(myDir) && std::filesystem::is_directory(myDir)){
            for( std::filesystem::directory_iterator dir_iter(myDir) ; dir_iter != end_iter ; ++dir_iter){
                if(dir_iter->path().extension() == ext){
                res.push_back(dir_iter->path());
                }
            }
        }
        else{
            ofLogError("FileUtils","no file to list");
            return res;
        }
        
        
        return res;
        
    }
    
    static vector<std::filesystem::path> getFilePathsWithExt(const string & d,const vector<string> & ext , bool allowDir = false){
        vector<std::filesystem::path> res;
        
        
        filesystem::path myDir ( d);
        
       filesystem::directory_iterator end_iter;
        if ( filesystem::exists(myDir) && std::filesystem::is_directory(myDir)){
            for( filesystem::directory_iterator dir_iter(myDir) ; dir_iter != end_iter ; ++dir_iter){
                if(std::find(ext.begin(), ext.end(), dir_iter->path().extension().string() )!=ext.end()){
                    res.push_back(dir_iter->path());
                }
            }
        }
        else{
            ofLogError("FileUtils","no file to list");
            return res;
        }
        
        
        return res;
        
    }
    
    static vector<std::filesystem::path> getFolderPaths(const string & d){
        vector<std::filesystem::path> res;
        
        
        std::filesystem::path myDir ( d);
        
        std::filesystem::directory_iterator end_iter;
        if ( std::filesystem::exists(myDir) && std::filesystem::is_directory(myDir)){
            for( std::filesystem::directory_iterator dir_iter(myDir) ; dir_iter != end_iter ; ++dir_iter){
                if(std::filesystem::is_directory(dir_iter->path())){
                    res.emplace_back(dir_iter->path());
                }
            }
        }
        else{
            ofLogError("FileUtils","no folder to list");
            return res;
        }
        
        
        return res;
        
    }
    
};

#endif /* defined(__ViZa__FileUtils__) */