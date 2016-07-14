#pragma once


#include "ofThread.h"

class ThreadedVideoLoader : public ofThread {

		// the thread function
		void ThreadedVideoLoader::threadedFunction() {

		// start
			lock();
			isLoaded = false;
			videoPlayer.setLoopState(OF_LOOP_NONE);
			videoPlayer.setAnchorPercent(0.5, 0.5);
			videoPlayer.load(path);
			isLoaded = true;
			unlock();
		// done
		}

		public :		
		bool isLoaded;
		ofVideoPlayer videoPlayer;
		string path;
};