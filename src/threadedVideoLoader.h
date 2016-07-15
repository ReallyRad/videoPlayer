#pragma once


#include "ofThread.h"

class ThreadedVideoLoader : public ofThread {

		// the thread function
		void ThreadedVideoLoader::threadedFunction() {

		// start
			lock();
			error = false;
			videoPlayer.setLoopState(OF_LOOP_NONE);
			videoPlayer.setAnchorPercent(0.5, 0.5);
			if (!videoPlayer.load(path)) {
				error = true;
			}			
			unlock();
		// done
		}

		public :		
		bool error;
		ofVideoPlayer videoPlayer;
		string path;
};