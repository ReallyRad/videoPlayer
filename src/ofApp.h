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
	void loadVideo(string path, ofVideoPlayer* videoPlayer);

	ofVideoPlayer videoPlayers[3];
	ofVideoPlayer * current;
	ofVideoPlayer * next;

	int fileIndex;
	int nr_videos;
	std::vector<string> filesPaths;
	double fileTimer;
	ofDirectory dir;		

	ThreadedVideoLoader loader;
};
