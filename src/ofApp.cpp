#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	//some path, may be absolute or relative to bin/data	
	ofDirectory dir(".");
	//populate the directory object
	dir.listDir();
	
	videoPlayers[0] = *new ofVideoPlayer();
	videoPlayers[1] = *new ofVideoPlayer();	

	//go through and save the files paths
	for (int i = 0; i < dir.size(); i++) {
		filesPaths.push_back(dir.getPath(i));
	}

	//load the first video	
	loadVideo(filesPaths[0], &videoPlayers[0], &loaders[0]);	
	loadVideo(filesPaths[1], &videoPlayers[1], &loaders[1]);
	
	//init timer
	fileTimer = ofGetElapsedTimeMillis();
		
	
	current = &loaders[0].videoPlayer;
	next = &loaders[1].videoPlayer;
	
	fileIndex = 0;
	
	Sleep(5000);
	//start playing current video
	current->play();
	current->setLoopState(OF_LOOP_NONE);
	int x = 3;
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
		/*
		for (int i = 0; i < filesPaths.size(); i++) {
			cout << "filesPaths[" << i << "] : " << filesPaths[i] << endl; 
		}		
		*/
		//increment index in files list
		fileIndex++;
		if (fileIndex == filesPaths.size())  fileIndex = 0;

		cout << "file index : " << fileIndex << endl;

		//switch current & next		
		loaders[fileIndex % 2].lock();
		loaders[(fileIndex + 1) % 2].lock();
		current = &loaders[fileIndex % 2].videoPlayer;
		cout << "current : " << current->getMoviePath() << endl;
		next = &loaders[(fileIndex + 1) % 2].videoPlayer;			
		loaders[fileIndex % 2].unlock();
		loaders[(fileIndex + 1) % 2].unlock();

		//load next video in a separate thread
		loadVideo(filesPaths[fileIndex], next, &loaders[(fileIndex + 1) % 2]);
		
		cout << "loading next : " << filesPaths[fileIndex] << endl;
		cout << endl;
		//start playing next video
		current->play();			
		current->setLoopState(OF_LOOP_NONE);
	}

	//update currently playing video
	current->update();

	//display framerate as window title
	std::stringstream strm;
	strm << "fps: " << ofGetFrameRate();
	ofSetWindowTitle(strm.str());
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
				
				//put file next in file list
				filesPaths.insert((filesPaths.begin() + (fileIndex + 1)%(filesPaths.size()-1)), dir.getPath(i)); 
			}
		}
	}
	fileTimer = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
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