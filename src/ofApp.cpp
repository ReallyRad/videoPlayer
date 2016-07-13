#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	//some path, may be absolute or relative to bin/data
	string path = ".";

	ofDirectory dir(path);
	//only show mov files
	dir.allowExt("mov");
	//populate the directory object
	dir.listDir();
	
	//go through and load all the videos
	for (int i = 0; i < dir.size(); i++) {
		addNewVideo(dir.getPath(i), true);
	}

	current = 0;
	videoPlayers[0].play();

	fileTimer = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofApp::update() {	
	//every second, check for new file and put it next on playback queue
	if (ofGetElapsedTimeMillis() - fileTimer > 1000) {
		checkNewVideos();
		fileTimer = ofGetElapsedTimeMillis();
	}
	
	//if video finished, rewind, stop, set index to next video and load another video
	if (videoPlayers[current].getIsMovieDone()) {
		//reset video
		videoPlayers[current].stop();
		videoPlayers[current].setFrame(0);

		//increment index
		if (current < videoPlayers.size() - 1) current++;
		else current = 0;
		videoPlayers[current].play();
		
	}

	//update currently playing video
	videoPlayers[current].update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	//draw current video at center of screen
	videoPlayers[current].draw(ofGetWidth() / 2, ofGetHeight() / 2);
}

//--------------------------------------------------------------
void ofApp::checkNewVideos() {	
	//set directory
	ofDirectory dir(".");
	dir.listDir();

	//look for new files in folder
	for (int i = 0; i < dir.size(); i++) {
		for (int j = 0; j < filesPaths.size(); j++) {
			//if video not already in files list, add it
			if (std::find(filesPaths.begin(), filesPaths.end(), dir.getPath(i)) == filesPaths.end()) {
				cout << "new video found : " << dir.getPath(i) << endl;
				//add new found videos after currently playing
				addNewVideo(dir.getPath(i), false);
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::addNewVideo(string path, bool end) {
	//add new file at end of files list
	filesPaths.push_back(path);
	ofVideoPlayer * v = new ofVideoPlayer();
	v->load(path);
	v->setLoopState(OF_LOOP_NONE);
	v->setAnchorPercent(0.5, 0.5);
	//put at end of queue
	if (end) videoPlayers.push_back(*v);
	//put after currently playing video
	else videoPlayers.insert(videoPlayers.begin() + current + 1, *v); //use threaded video loader here

	//if more than max videos loaded, unload one video
	if (videoPlayers.size>PLAYBACK_QUEUE_MAX_SIZE) videoPlayers.erase(videoPlayers.begin());
}