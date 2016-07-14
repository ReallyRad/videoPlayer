#pragma once


#include "ofThread.h"

class ThreadedVideoLoader : public ofThread {

		// the thread function
		void ThreadedVideoLoader::threadedFunction() {

		// start
			lock();
			videoPlayer.setLoopState(OF_LOOP_NONE);
			videoPlayer.setAnchorPercent(0.5, 0.5);
			videoPlayer.load(path);
			unlock();
		// done
		}

		public :		
		ofVideoPlayer videoPlayer;
		string path;
};