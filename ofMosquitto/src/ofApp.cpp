#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	brightness = 1;
	numPCBs    = 2;
	numLed     = 32*numPCBs;
	initPixelData();
	apa.setupAPA102();
	
	for(int i=0; i<numPCBs; i++){
		randomNumbers.push_back(ofRandom(0,2));
	}

	currentState = es_Eyes;

	PixelEyes.setup(numPCBs);
	PixelEyes.setDrawMargin(4);
	
	currentMillis = ofGetElapsedTimeMillis();
	lastStateNumberMillis = currentMillis;
	lastStateNumberMillis = currentMillis;
	lastNewNumberMillis = currentMillis;
	stateNumberStartMillis = currentMillis;
	
	// How long before the numbers appear
	stateNumberIntervalMin = 40000;
	stateNumberIntervalMax = 80000;
	stateNumberInterval = stateNumberIntervalMin;
	
	// How long the numbers appear for
	stateNumberDurationMin =  3000;
	stateNumberDurationMax =  9000;
	stateNumberDuration = stateNumberDurationMin;
	
	// The speed of number change
	newNumberInterval = 250;
	
	// Pixile Communicator
	// Can we have a nicer setup?
	pixile.Master(false);
	pixile.Computer_id(1); // Really?!
	pixile.Server_port(3637);
	pixile.SetupSockets();
	pixile.start();
	soundsOn = true;
	
	// we are running the systems commands
	// in a sperate thread so that it does
	// not block this application
	startThread();
}

//--------------------------------------------------------------
void ofApp::threadedFunction() {

	while (isThreadRunning()) {
		if (pixile.SoundsOn(){
			// call the system command say
			std::string cmd = "aplay data/audio/MOUNTAINS_QUOTE_" + std::to_string((int)ofRandom(299)) + ".wav";
			ofSystem(cmd.c_str());
			// slowdown
			ofSleepMillis(1000);
		}
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	currentMillis = ofGetElapsedTimeMillis();
	pixile.update();
	
	if (pixile.SoundsOn() != soundsOn) {
		soundsOn = pixile.SoundsOn();
	}

	PixelEyes.update();

	switch (currentState) {
		case es_Eyes:
			doStateEyes();
			break;
		case es_Numbers:
			doStateNumbers();
			break;
	}	
	
	if(!pixile.LightsOn()){
		// Leds OFF
		apa.setAPA102(numLed,pixelDataOFF,0);
	} else {
		// Update LEDs
		apa.setAPA102(numLed,pixelData,brightness);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	// Drawing is just for debugging :)
	// Comment out before deploying
	PixelEyes.draw(180, 100, 25);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::exit(){ 
	apa.clearAPA102(numLed+5);
	waitForThread(true);
	ofExit(0);
}

//--------------------------------------------------------------
void ofApp::doStateEyes(){
	ofPixels eyepix = PixelEyes.EyeBalls.getEyeBallPixels();
	int led = 0;
	for(int i=0; i<PixelEyes.getEyeCount(); i++){
		for(int y = 0; y < PixelEyes.EyeBalls.getSize().y; y++){
			for(int x = 0; x < PixelEyes.EyeBalls.getSize().x; x++){
				ofColor c = eyepix.getColor(x,y);
				if(c.a > 0){
					pixelData[led] = c; 
					led++;
				}
			}
		}
	}
	if(currentMillis - lastStateNumberMillis > stateNumberInterval){
		currentState = es_Numbers;
		stateNumberStartMillis = currentMillis;
		freshStateNumberDuration();
	}
}

//--------------------------------------------------------------
void ofApp::doStateNumbers(){
	if(currentMillis - lastNewNumberMillis > newNumberInterval) newRandomNumbers();
	int led = 0;
	for(int i=0; i<numPCBs; i++){
		for(int y = 0; y < 6; y++){
			for(int x = 0; x < 6; x++){
				ofColor c = ofColor(0,0,0); //OFF
				switch(numberMap[randomNumbers[i]][6*y+x]) {
					case -1:
						continue; // No Led at this position
						break;
					 case 0: // LED OFF
						break;
					case 1: // LED ON
						c = PixelEyes.EyeBalls.m_ColourEyeBall;
						break;
				}
				pixelData[led] = c; 
				led++;
			}
		}
	}
	// Get out of this state!
	if(currentMillis - stateNumberStartMillis > stateNumberDuration) {
		currentState = es_Eyes;
		lastStateNumberMillis = currentMillis;
	}
}

//--------------------------------------------------------------
void ofApp::newRandomNumbers(){
	for(int &i : randomNumbers){
		ofSeedRandom();
		i = ofRandom(0,2);
	}
	lastNewNumberMillis = currentMillis;
}

//--------------------------------------------------------------
void ofApp::freshStateNumberInterval(){
	lastStateNumberMillis = currentMillis;
	stateNumberInterval = ofRandom(stateNumberIntervalMin, stateNumberIntervalMax);
}

//--------------------------------------------------------------
void ofApp::freshStateNumberDuration(){
	stateNumberDuration = ofRandom(stateNumberDurationMin, stateNumberDurationMax);
}

//--------------------------------------------------------------
void ofApp::initPixelData(){ 
	for(int i = 0; i < numLed; i++) {
		pixelData.push_back(ofColor(0,0,0));
		pixelDataOFF.push_back(ofColor(0,0,0));
	}
}
