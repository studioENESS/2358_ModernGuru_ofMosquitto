#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "imgui_internal.h"
#include "ofxPixileComms.h"
#include "ofxPixelEyes.h"
#include "Eyeball.h"

#ifdef __arm__
#include "ofxGPIO.h"
#define MICROWAVE_INSTALLED
#endif

class ofApp : public ofBaseApp {

	public:
		ofJson saveConfig();
		void loadConfig();

		ofJson getSettings();
		void setSettings(ofJson settings);

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
		
		struct eyeMovementState {
			std::string name;
			union {
				struct {
					uint32_t moveIntervalMin;
					uint32_t moveIntervalMax;
					uint32_t targetIntervalMin;
					uint32_t targetIntervalMax;
					uint32_t stateIntervalMin;
					uint32_t stateIntervalMax;
				};
				uint32_t values[6];
			};
		};

		void setEyeMovementState(eyeMovementState s);
		void addCurrentState(std::string name ="Untitled");
		void saveStates();
		void loadStates();

		void playSound(std::string cmd);
		void playQuote(int quoteID);
		void setEyeballColor(ofColor c);
#ifdef __arm__
		GPIO* gpioMicrowaveSensor;
#endif
		std::string stateMicrowaveSensor;
		
		enum eState {
			es_Eyes = 0,
			es_Numbers,
			es_Count
		};
		
		void setState(eState state);
	
		eState getState(){
			return currentState;
		};

		const int numberMap[2][36] = {
		  { // 0
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
		
	protected:
		static void PixileMessageHandler(SPixileMessage* pMessage, void* pUserData);
		bool mapUpsideDown = false;

	private:
#ifdef __arm__
		LED apa;
#endif
		ofxImGui::Gui gui;
		std::vector<eyeMovementState> vEyeMovementStates;
		int iSelectedStateIndex=0;
		void drawEyeSettingsWindow();
		void drawStateCollectionWindow();
		void drawAppSettingsWindow();

		SoundPlayer soundPlayer1;
		SoundPlayer soundPlayer2;

		int myNetworkID;
		int drawMargin;
		int drawScale;
		eState currentState;
		
		uint64_t currentMillis;
		
		uint64_t lastStateMillis;
		uint32_t stateInterval;
		void freshStateInterval();
		
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

		std::vector<ofColor> pixelData;
		std::vector<ofColor> pixelDataOFF;
		ofxPixelEyes PixelEyes;
		
		ofxPixileComms pixile;
		bool soundsOn;
		bool lightsOn;

		ofFbo outputTexture;
};
