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

	nr_videos = dir.size();

	stringstream files;

	//go through and load all the videos
	for (int i = 0; i < dir.size(); i++) {
		addNewVideo(dir.getPath(i), true);
	}

	ofLogNotice(filesPaths);

	current = 0;
	videoPlayers[0].play();

	fileTimer = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofApp::update() {

	if (ofGetElapsedTimeMillis() - fileTimer > 1000) {
		checkNewVideos();
		fileTimer = ofGetElapsedTimeMillis();
	}

	
	//if video finished, rewind, pause and go to next video
	if (videoPlayers[current].getIsMovieDone()) {
		videoPlayers[current].stop();
		videoPlayers[current].setFrame(0);
		if (current < videoPlayers.size() - 1) current++;
		else current = 0;
		videoPlayers[current].play();
	}

	//update current playing video
	videoPlayers[current].update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	//draw current video
	videoPlayers[current].draw(ofGetWidth() / 2, ofGetHeight() / 2);
}

void ofApp::checkNewVideos() {

	ofDirectory dir(".");
	dir.listDir();

	if (dir.size() > nr_videos) nr_videos = dir.size();

	//look for new files in folder
	for (int i = 0; i < dir.size(); i++) {
		for (int j = 0; j < filesPaths.size(); j++) {
			//if video not already in files list, add it
			if (std::find(filesPaths.begin(), filesPaths.end(), dir.getPath(i)) == filesPaths.end()) {
				cout << "new video found : " << dir.getPath(i) << endl;
				addNewVideo(dir.getPath(i), false);
			}
		}
	}

}

void ofApp::addNewVideo(string path, bool end) {
	filesPaths.push_back(path);
	ofVideoPlayer * v = new ofVideoPlayer();
	v->load(path);
	v->setLoopState(OF_LOOP_NONE);
	v->setAnchorPercent(0.5, 0.5);
	//put at end of queue
	if (end) videoPlayers.push_back(*v);
	//put after currently playing video
	else videoPlayers.insert(videoPlayers.begin() + current + 1, *v);	
}