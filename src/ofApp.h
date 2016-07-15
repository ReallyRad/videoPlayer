#pragma once

#include "ofMain.h"
#include "threadedVideoLoader.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void exit();
	void checkNewVideos();
	void loadVideo(string path, ofVideoPlayer* videoPlayer, ThreadedVideoLoader* loader);
	void checkNewDirs();
	void listFiles();
	void listDirs();

	ofVideoPlayer videoPlayers[2];
	ofVideoPlayer * current;
	ofVideoPlayer * next;

	int fileIndex;	
	std::vector<string> filesPaths; //list of files paths within the current folder
	std::vector<string> dirPaths; //list of directories within the bin/data folder
	double fileTimer;
	double dirTimer;	

	ThreadedVideoLoader loaders[2];

	bool started; //whether we've started playing back the videos
	bool newVideoDetected;
};
