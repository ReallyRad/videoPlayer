#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void checkNewVideos();
	void loadVideo(string path, ofVideoPlayer* videoPlayer);

	ofVideoPlayer* current;
	ofVideoPlayer* next;
	int fileIndex;
	int nr_videos;
	std::vector<string> filesPaths;
	double fileTimer;
	ofDirectory dir;		
};
