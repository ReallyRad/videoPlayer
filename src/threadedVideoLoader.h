#pragma once


#include "ofThread.h"

class ThreadedVideoLoader : public ofThread {

		// the thread function
		void ThreadedVideoLoader::threadedFunction() {

		// start
			lock();

			videoPlayer.load(path);
			unlock();
		// done
		}

		public :		
		ofVideoPlayer videoPlayer;
		string path;
};