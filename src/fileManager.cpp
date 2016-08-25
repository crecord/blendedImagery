//
//  fileManager.cpp
//  FunWithFiles
//
//  Created by Caroline Record on 8/24/16.
//
//


#include "fileManager.h"

void fileManager::setup(int numOfImgToLoad,int crpTop, int crpBot){
    
    crpT = crpTop;
    crpB = crpBot;
    
    startPath = "sampleImages/sample";
    
    setUpDays();
    //test.setup("005_2016-08-19_R/016_00-20-01.png");
    
    ofImage testSz;
    testSz.load(nextFileToLoad);
    imgWidth = testSz.getWidth()/15;
    imgHeight = testSz.getHeight()/15;
    for(int i =0; i < numOfImgToLoad; i++){
        testQ.push_back(move(unique_ptr<oneImage>(new oneImage)));
        string temp = nextFileToLoad;
        testQ.back()->setup(temp, crpT, crpB);
        nextMoment();
    }
    posX = 0;
}

void fileManager::update(){
    checkNewDay();
    checkNewMoment();
    //posX-=5;
}


bool fileManager::check(int xPos, int thresh){
    if (xPos < thresh){
        return true;
    }
    else {
        // take out the one that just disapeared from the left
        testQ.pop_front();
        // insert the one from the rights
        testQ.push_back(move(unique_ptr<oneImage>(new oneImage)));
        testQ.back()->setup(nextFileToLoad, crpT, crpB);
        nextMoment();
        return false;
    }
}

void fileManager::draw(int index, int x, int y){
    //test.draw(0,0,1000,1000);
    testQ.at(index)->draw(x,y);
}


void fileManager::setUpDays(){
    // day selection
    lastWeather = "";
    dayCount = 0;
    day.listDir(startPath);
    numOfDays = day.size();
    vector<string> parts;
    parts = ofSplitString(day.getPath(day.size()-1), "_");
    currentDayWeather = parts.at(parts.size()-1);
    makeUnusedDaysMap();
    //moment.allowExt(".tif");
    moment.listDir(day.getPath(day.size()-1));
    numOfMoments = moment.size();
    indexDay =selectDay();
    moment.listDir(day.getPath(indexDay));
    indexMoment = 0;
    nextFileToLoad = moment.getPath(indexMoment);
    ofLog()<< "how big" << moment.size();
}

int fileManager::selectDay(){
    dayCount ++;
    if(dayCount%3==0){
        lastWeather = currentDayWeather;
        return day.size()-1;
    }
    else{
        if (mapOfDays.size() <= 0){
            // fill back up with days
            ofLog() << "went through all of them";
            makeUnusedDaysMap();
        }
        map<int, string>::iterator it = mapOfDays.begin();
        while(it != mapOfDays.end()){
            if (lastWeather != it->second){
                lastWeather= it->second;
                int toReturn = it->first;
                mapOfDays.erase(it);
                return toReturn;
            }
            it++;
        }
        // there was not one available with different weather
        // so just go with a random one
        int randomDay = int(ofRandom(mapOfDays.size()-1));
        it = mapOfDays.begin();
        advance(it, randomDay);
        int toReturnTwo = it->first;
        lastWeather = it->second;
        mapOfDays.erase(it);
        return toReturnTwo;
    }
}

void fileManager::makeUnusedDaysMap(){
    vector<string> theParts;
    
    // make that list of days
    // minus one bc I do not want to intclude the current day in this list.
    for(int i=0; i <day.size()-1; i++){
        theParts.clear();
        theParts = ofSplitString(day.getPath(i), "_");
        string weatherCode = theParts.at(theParts.size()-1);
        if (weatherCode != "E"){
            mapOfDays[i] = theParts.at(theParts.size()-1);
        }
    }
}

void fileManager::nextMoment(){
    // there are more moments in this day
    if(indexMoment <= (moment.size()-2)){
        indexMoment ++;
        ofLog()<< indexMoment;
        
    }
    // no more moments in this day
    // go to the next day
    else {
        indexMoment =0;
        indexDay = selectDay();
        moment.listDir(day.getPath( indexDay ));
    }
    // check if the file has a flag on it
    string nextPotentialFile = moment.getPath(indexMoment);
    vector <string> temp = ofSplitString( nextPotentialFile, "_");
    string endId = temp.at(temp.size()-1);
    endId = endId[0];
    ofLog() << endId;
    if (endId != "E"){
        nextFileToLoad = nextPotentialFile;
    }
    else {
        ofLog()<< "do not load this file";
    }
}

void fileManager::checkNewDay(){
    dirToCheck.listDir(startPath);
    if(numOfDays < dirToCheck.size()){
        ofLog() << "new day";
        setUpDays();
    }
}

void fileManager::checkNewMoment(){
    dirToCheck.listDir(day.getPath(day.size()-1));
    if (numOfMoments < dirToCheck.size() ){
        ofLog()<< "new moment";
        numOfMoments = dirToCheck.size();
        // new Image - so draw it.
        string currentMomentPath =dirToCheck.getPath(dirToCheck.size()-1);
        curMoment.setup(currentMomentPath, crpT, crpB);
        insertMomentCheck(currentMomentPath);
    }
}

void fileManager::insertMomentCheck(string momentPath){
    
    // a new moment was just added
    // if the current day is the one being processed then re-initialize the list so it will include that new image
    if(indexDay == day.size()-1){
        moment.listDir(day.getPath(indexDay));
    }
    // need to check if it should be inserted into the deque or whether it will just happen the next the current day is in active circulation
    // loop through all the days currently in the deque
    
    // what is folder ID of the current day?
    ofLog()<< momentPath;
    // 005_2016-08-19_R/013_00-20-01.png
    string dayIdCur = ofSplitString(momentPath,"_").at(0);
    vector<string> temp = ofSplitString(momentPath, "/");
    string nameOfMomentCur = temp.at(temp.size()-1);
    int momentIdCur = ofToInt(ofSplitString(nameOfMomentCur, "_").at(0));
    
    for(int i=0; i < testQ.size(); i++){
        string fileName = testQ.at(i)->filePath;
        string dayId = ofSplitString(fileName, "_").at(0);
        if (dayId == dayIdCur){
            vector<string> partz = ofSplitString(fileName, "/");
            string nameOfMoment = partz.at(partz.size()-1);
            int momentId = ofToInt(ofSplitString(nameOfMoment, "_").at(0));
            
            if (momentId == momentIdCur-1){
                // put it after I in the deque
                testQ.insert(testQ.begin()+i+1,move(unique_ptr<oneImage>(new oneImage)) );
                testQ.at(i+1)->setup(momentPath, crpT, crpB);
                break;
            }
            ofLog()<< momentId;
        }
    }
    /*
     deque<unique_ptr<oneImage>>::iterator it = testQ.begin();
     while(it != testQ.end()){
     // are any of them from the current day
     string = it->
     it++;
     }*/
    
    
    // is the last day in there - if it is insert most recent image into it
}
    