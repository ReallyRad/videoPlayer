#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	//some path, may be absolute or relative to bin/data	
	ofDirectory dir(".");
	//populate the directory object
	dir.listDir();
	
	videoPlayers[0] = *new ofVideoPlayer();
	videoPlayers[1] = *new ofVideoPlayer();
	videoPlayers[2] = *new ofVideoPlayer();

	//go through and save the files paths
	for (int i = 0; i < dir.size(); i++) {
		filesPaths.push_back(dir.getPath(i));
	}

	//load the first three videos
	/*
	if (filesPaths.size() > 2) {
		loadVideo(filesPaths[0], &videoPlayers[0]);
		loadVideo(filesPaths[1], &videoPlayers[1]);
		loadVideo(filesPaths[2], &videoPlayers[2]);
	}	
	else { cout << "not enough videos!!" << endl; }		
	*/
	for (int i = 0; i < 3; i++) {
		videoPlayers[i].load(filesPaths[i]);
		videoPlayers[i].setLoopState(OF_LOOP_NONE);
		videoPlayers[i].setAnchorPercent(0.5, 0.5);
	}

	//init timer
	fileTimer = ofGetElapsedTimeMillis();

	//start playing current video
	videoPlayers[0].play();
	
	current = &videoPlayers[0];
	next = &videoPlayers[1];

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
		fileIndex++;
		if (fileIndex == filesPaths.size())  fileIndex = 0;

		//switch current & next
		loader.lock();
		current = &videoPlayers[fileIndex % 3];
		next = &videoPlayers[(fileIndex + 1) % 3];
		loader.unlock();

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
				
				//put file 2 elements away from current
				try { filesPaths.insert((filesPaths.begin() + fileIndex + 2), dir.getPath(i)); }
				catch (exception e) { 
					filesPaths.insert((filesPaths.begin()), dir.getPath(i));					
				}
			}
		}
	}
	fileTimer = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
//loads next video
//TODO make threaded
void ofApp::loadVideo(string path, ofVideoPlayer* v) {	
	//v->load(path);
	loader.path = path;
	loader.videoPlayer = *v;
	loader.startThread();	
}

void ofApp::exit() {
	// stop the thread
	loader.stopThread();
}