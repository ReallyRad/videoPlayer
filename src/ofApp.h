#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void checkNewVideos();
	void addNewVideo(string path, bool end);

	std::vector<ofVideoPlayer> videoPlayers;
	int current;
	int nr_videos;
	std::vector<string> filesPaths;
	double fileTimer;
};
