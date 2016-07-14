#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	
	listDirs();
	listFiles();

	//initialize videoPlayers
	videoPlayers[0] = *new ofVideoPlayer();
	videoPlayers[1] = *new ofVideoPlayer();

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

	//every 3 seconds, check for new file and put it next on playback queue
	if (ofGetElapsedTimeMillis() - dirTimer > 3000) checkNewDirs();
	
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
	//get path for last directory
	int s = dirPaths.size();
	string path = dirPaths.at(s - 1);	
	ofDirectory last(path);	
	last.listDir();

	//look for new files in folder
	if (last.size() > filesPaths.size()) {
		cout << "new file detected" << endl;
		
		for (int i = 0; i < last.size(); i++) { //for every element in last folder
			for (int j = 0; j < filesPaths.size(); j++) { //for every element in filesPaths list
				//check if video not already in files list
				if (std::find(filesPaths.begin(), filesPaths.end(), last.getPath(i)) == filesPaths.end()) {
					cout << "new video found : " << last.getPath(i) << endl;

					//put file next in file list so it can be loaded immediatly
					filesPaths.insert((filesPaths.begin() + (fileIndex + 1) % (filesPaths.size() - 1)), last.getPath(i));
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
	ofDirectory last(dirPaths.back());

	//populate the directory object
	last.listDir();

	//list the files within the last dir and write that into filesPaths
	for (int i = 0; i < last.size(); i++) {
		filesPaths.push_back(last.getPath(i));
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
		dirPaths.push_back(dir.getPath(i));
	}

}

//--------------------------------------------------------------
//checks if new dir
void ofApp::checkNewDirs() {	
	//set directory
	ofDirectory bindata(".");
	bindata.listDir();

	if (bindata.size() > dirPaths.size()) {
		cout << "new folder detected" << endl;

		//reset the folder list
		dirPaths.clear();
		listDirs();
		filesPaths.clear();
		listFiles();		
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