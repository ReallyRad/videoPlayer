#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {	
	listDirs();
	listFiles();
	
	started = false;
	if (filesPaths.size() > 1) started = true;

	//initialize videoPlayers
	videoPlayers[0] = *new ofVideoPlayer();
	videoPlayers[1] = *new ofVideoPlayer();

	//load the first video	
	if (started) {
		loadVideo(filesPaths[0], &videoPlayers[0], &loaders[0]);
		loadVideo(filesPaths[1], &videoPlayers[1], &loaders[1]);
	}	
	
	Sleep(5000);

	//init timer
	fileTimer = ofGetElapsedTimeMillis();
	
	current = &loaders[0].videoPlayer;
	next = &loaders[1].videoPlayer;
	
	fileIndex = 0;
	
	//start playing current video
	if (started) {
		current->play();
		current->setLoopState(OF_LOOP_NONE);
	}

	newVideoDetected = false;
}

//--------------------------------------------------------------
void ofApp::update() {	
	//if we had some error loading a video, rebuild file index
	if (loaders[0].error || loaders[1].error) {
		listFiles();
		loaders[0].error = false; loaders[1].error = false;
//		fileIndex 
	}

	//every second, check for new file and put it next on playback queue
	if (ofGetElapsedTimeMillis() - fileTimer > 1000) checkNewVideos();

	//every 3 seconds, check for new file and put it next on playback queue
	if (ofGetElapsedTimeMillis() - dirTimer > 3000) checkNewDirs();
		
	//if video finished, rewind, stop, set file index to next video and start playing next video
	if (current->getIsMovieDone()) {		
		//reset video		
		current->setFrame(0);				

		//switch current & next		
		loaders[0].lock();
		loaders[1].lock();
		//will play next file now
		current = &loaders[(fileIndex + 1) % 2].videoPlayer;
		//previous buffer will be for loading next video
		next = &loaders[fileIndex % 2].videoPlayer;
		loaders[0].unlock();
		loaders[1].unlock();
		
		//load next video in a separate thread
		if (!newVideoDetected){
			loadVideo(filesPaths[(fileIndex + 2)%filesPaths.size()], next, &loaders[(fileIndex) % 2]);						
		}
		else {			
			loadVideo(filesPaths[filesPaths.size()-1], next, &loaders[(fileIndex) % 2]);
			newVideoDetected = false;
		}		

		//increment index in files list
		fileIndex++;
		if (fileIndex >= filesPaths.size())  fileIndex = 0;

		//start playing new video
		current->play();
		current->setLoopState(OF_LOOP_NONE);
	}	

	//update currently playing video
	if (started) {				
		current->update();
		if (filesPaths.size() < 2) started = false();
	}
	else if (filesPaths.size() > 1) {
		started = true;
		loadVideo(filesPaths[0], &videoPlayers[0], &loaders[0]);
		Sleep(2000);
		loadVideo(filesPaths[1], &videoPlayers[1], &loaders[1]);		
		Sleep(2000);
		current = &loaders[0].videoPlayer;
		next = &loaders[1].videoPlayer;
		current->play();
		current->setLoopState(OF_LOOP_NONE);
	}


}

//--------------------------------------------------------------
void ofApp::draw() {
	//display framerate as window title
	std::stringstream strm;
	strm << "fps: " << ofGetFrameRate();
	ofSetWindowTitle(strm.str());

	//draw current video at center of screen
	if (started) {
		current->draw(ofGetWidth() / 2, ofGetHeight() / 2);
	}	
	else {
		stringstream ss;
		ss << "Not enough videos loaded in " << dirPaths.back() << ", waiting for more videos to load" << endl;
		ofDrawBitmapString(ss.str(), ofGetWidth() / 2-315, ofGetHeight() / 2);
	}

	if (debugMode) {
		stringstream ss;
		ss << "current : " << current->getMoviePath();
		ofDrawBitmapString(ss.str(), 10, 20);
		ss.clear();
		ss << "next : " << next->getMoviePath();
		ofDrawBitmapString(ss.str(), 10, 30);
		ss.clear();
		ss << "fileIndex : " << fileIndex;
		ofDrawBitmapString(ss.str(), 10, 40);
		ss.clear();				
	}
}

//--------------------------------------------------------------
void ofApp::checkNewVideos() {		
	//get path for last directory
	int s = dirPaths.size();
	
	if (!started) listFiles();

	else {
		string path = dirPaths.at(s - 1);
		ofDirectory last(path);
		last.listDir();

		//look for new files in folder
		if (last.size() > filesPaths.size()) {			

			for (int i = 0; i < last.size(); i++) { //for every element in last folder
				for (int j = 0; j < filesPaths.size(); j++) { //for every element in filesPaths list
					//check if video not already in files list
					if (std::find(filesPaths.begin(), filesPaths.end(), last.getPath(i)) == filesPaths.end()) {

						//put file next in file list so it can be loaded immediatly
						//filesPaths.insert((filesPaths.begin() + (fileIndex + 1) % (filesPaths.size())), last.getPath(i));

						filesPaths.push_back(last.getPath(i));						
						newVideoDetected = true;
					}
				}
			}
		}
	}	

	fileTimer = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
//writes list of files within last subdirectory in filesPaths
void ofApp::listFiles() {
	
	//the last folder within bin/data in alphabetical order
	if (dirPaths.size() > 0) {
		ofDirectory last(dirPaths.back());

		//populate the directory object
		last.listDir();

		//clear filesPaths
		filesPaths.clear();

		//list the files within the last dir and write that into filesPaths
		for (int i = 0; i < last.size(); i++) {
			filesPaths.push_back(last.getPath(i));
		}
	}
	
}

//--------------------------------------------------------------
//puts list of subdirectories into dirPaths
void ofApp::listDirs() {
	
	//the bin/data folder	
	ofDirectory dir(".");

	//populate the directory object
	dir.listDir();

	//list all the folders
	for (int i = 0; i < dir.size(); i++) {
		ofDirectory dar(dir.getPath(i));
		if (dar.isDirectory()) {
			dirPaths.push_back(dir.getPath(i));
		}
	}

}

//--------------------------------------------------------------
//checks if new dir
void ofApp::checkNewDirs() {	
	//set directory
	ofDirectory bindata(".");
	bindata.listDir();

	if (bindata.size() > dirPaths.size()) {		
		//reset the folder list
		dirPaths.clear();
		//populate it
		listDirs();
		//reset the files list
		filesPaths.clear();
		//populate it
		listFiles();		
		//if file list has less than 2 files, stop playback
		if (filesPaths.size() < 2) started = false;
	}
			
	dirTimer = ofGetElapsedTimeMillis();
}

//loads next video
//TODO make threaded
void ofApp::loadVideo(string path, ofVideoPlayer* v, ThreadedVideoLoader* t) {	
	//v->load(path);
	t->path = path;
	t->videoPlayer = *v;	
	t->startThread();	
}

void ofApp::exit() {
	// stop the thread
	loaders[0].stopThread();
	loaders[1].stopThread();	
}

void ofApp::keyPressed(int key) {
	if (key == OF_KEY_UP) {
		debugMode = !debugMode;
	}
}