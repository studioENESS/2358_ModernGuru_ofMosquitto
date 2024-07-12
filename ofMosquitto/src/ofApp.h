#pragma once

#include "ofMain.h"
#include "ofxGPIO.h"
#include "ofxPixileComms.h"
#include "ofxPixelEyes.h"

enum eState {
	es_Eyes = 0,
	es_Numbers,
	es_Count
};

const int numberMap[][36] = {
  {
	-1, 0, 1, 1, 0,-1,
	 0, 1, 0, 0, 1, 0,
	 0, 1, 0, 0, 1, 0,
	 0, 1, 0, 0, 1, 0,
	 0, 1, 0, 0, 1, 0,
	-1, 0, 1, 1, 0,-1
  },
  { // 1
	-1, 0, 1, 1, 0,-1,
	 0, 0, 0, 1, 0, 0,
	 0, 0, 0, 1, 0, 0,
	 0, 0, 0, 1, 0, 0,
	 0, 0, 0, 1, 0, 0,
	-1, 0, 0, 1, 0,-1
  }	
};

class ofApp : public ofBaseApp, public ofThread {

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

	protected:
		static void PixileMessageHandler(SPixileMessage* pMessage, void* pUserData);

	private:
		eState currentState;
		uint64_t currentMillis;
		
		uint64_t lastStateNumberMillis;
		uint16_t stateNumberInterval;
		uint16_t stateNumberIntervalMin;
		uint16_t stateNumberIntervalMax;
		
		uint64_t stateNumberStartMillis;
		uint16_t stateNumberDuration;
		uint16_t stateNumberDurationMin;
		uint16_t stateNumberDurationMax;
		void freshStateNumberDuration();
		
		uint64_t lastNewNumberMillis;
		uint16_t newNumberInterval;

		void doStateEyes();
		void doStateNumbers();
		
		void newRandomNumbers();
		void freshStateNumberInterval();
		std::vector<int> randomNumbers;

		void initPixelData(); 
		int numLed;
		int numPCBs;
		int brightness;
		LED apa;
		std::vector<ofColor> pixelData;
		std::vector<ofColor> pixelDataOFF;
		ofxPixelEyes PixelEyes;
		
		ofxPixileComms pixile;
		bool soundsOn;
};
