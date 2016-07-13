#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	//some path, may be absolute or relative to bin/data	
	ofDirectory dir(".");
	//populate the directory object
	dir.listDir();
	
	current = new ofVideoPlayer();
	next = new ofVideoPlayer();

	//go through and save the files paths
	for (int i = 0; i < dir.size(); i++) {
		filesPaths.push_back(dir.getPath(i));
	}

	//load the first two videos
	if (filesPaths.size() > 1) {
		loadVideo(filesPaths[0], current);
		loadVideo(filesPaths[1], next);
	}
	else {
		cout << "not enough videos loaded" << endl;
	}
	
	//init timer
	fileTimer = ofGetElapsedTimeMillis();

	//start playing current video
	current->play();

	fileIndex = 0;
	
}

//--------------------------------------------------------------
void ofApp::update() {	
	//every second, check for new file and put it next on playback queue
	if (ofGetElapsedTimeMillis() - fileTimer > 1000) checkNewVideos();
	
	//if video finished, rewind, stop, set file index to next video and start playing next video
	if (current->getIsMovieDone()) {
		//reset video
		current->stop();
		current->setFrame(0);		

		//increment index in files list
		if (fileIndex < filesPaths.size() - 1) fileIndex++;
		else fileIndex = 0;

		//set current to next
		current = next;

		//load next video in a separate thread
		loadVideo(filesPaths[fileIndex], next);				

		//start playing next video
		current->play();			

	}

	//update currently playing video
	current->update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	//draw current video at center of screen
	current->draw(ofGetWidth() / 2, ofGetHeight() / 2);
}

//--------------------------------------------------------------
void ofApp::checkNewVideos() {	
	//set directory
	ofDirectory dir(".");
	dir.listDir();

	//look for new files in folder
	for (int i = 0; i < dir.size(); i++) {
		for (int j = 0; j < filesPaths.size(); j++) {
			//if video not already in files list
			if (std::find(filesPaths.begin(), filesPaths.end(), dir.getPath(i)) == filesPaths.end()) {
				cout << "new video found : " << dir.getPath(i) << endl;
				//play new video next
				//loadVideo(dir.getPath(i), next);				
				//push video at end of file list
				//filesPaths.push_back(dir.getPath(i));				
				filesPaths.insert(filesPaths.begin() + fileIndex, dir.getPath(i));
			}
		}
	}
	fileTimer = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
//loads next video
//TODO make threaded
void ofApp::loadVideo(string path, ofVideoPlayer* v) {	
	v->load(path);
	v->setLoopState(OF_LOOP_NONE);
	v->setAnchorPercent(0.5, 0.5);	
}