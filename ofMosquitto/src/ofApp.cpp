#include "ofApp.h"
#include "Eyeball.h"
#include <unistd.h>
//--------------------------------------------------------------
void ofApp::setup() {
	gpioMicrowaveSensor = new GPIO("23");
	gpioMicrowaveSensor->export_gpio();
	gpioMicrowaveSensor->setdir_gpio("in");
	stateMicrowaveSensor = "1"; //pull-down logic
	
	drawMargin = 4;
	brightness = 1;
	numPCBs    = 2;
	numLed     = 32*numPCBs;
	initPixelData();
	apa.setupAPA102();
	
	for(int i=0; i<numPCBs; i++){
		randomNumbers.push_back(ofRandom(0,2));
	}

	currentState = es_Eyes;
	
	soundsOn = false;
	
	PixelEyes.setup(numPCBs);
	PixelEyes.setDrawMargin(drawMargin);
	PixelEyes.Eyeballs.setSoundOn(soundsOn);

#ifdef MICROWAVE_INSTALLED
	PixelEyes.Eyeballs.canSleep(true);
#else
	PixelEyes.Eyeballs.canSleep(false);
#endif

	currentMillis = ofGetElapsedTimeMillis();
	lastStateNumberMillis = currentMillis;
	lastStateNumberMillis = currentMillis;
	lastNewNumberMillis = currentMillis;
	stateNumberStartMillis = currentMillis;

	// How long before the numbers appear
	stateNumberIntervalMin = 20000;
	stateNumberIntervalMax = 50000;
	stateNumberInterval = stateNumberIntervalMin;
	
	// How long the numbers appear for
	stateNumberDurationMin =  3000;
	stateNumberDurationMax =  9000;
	stateNumberDuration = stateNumberDurationMin;
	
	// The speed of number change
	newNumberInterval = 250;

	// Pixile Communicator
	// Can we have a nicer setup?
	getMyNetworkID();
	pixile.Master(false);
	pixile.Computer_id(myNetworkID);
	pixile.Server_port(3637);
	pixile.SetupSockets();
	pixile.SetMessageHandler(&PixileMessageHandler, this);
}

//--------------------------------------------------------------
void ofApp::update(){
	currentMillis = ofGetElapsedTimeMillis();
	pixile.update();
	
	if (pixile.SoundsOn() != soundsOn) {
		soundsOn = pixile.SoundsOn();
		PixelEyes.Eyeballs.setSoundOn(soundsOn);
	}

	switch (currentState) {
		case es_Eyes:
			PixelEyes.update();
			doStateEyes();
			break;
		case es_Numbers:
			doStateNumbers();
			break;
	};

	if(!pixile.LightsOn()){
		// Leds OFF
		apa.setAPA102(numLed,pixelDataOFF,0);
	} else {
		// Update LEDs
		apa.setAPA102(numLed,pixelData,brightness);
	};
	
#ifdef MICROWAVE_INSTALLED
	gpioMicrowaveSensor->getval_gpio(stateMicrowaveSensor);
	//ofLog() << stateMicrowaveSensor << std::endl;
	if(stateMicrowaveSensor == "0") PixelEyes.Eyeballs.sleep(false);
#endif

}

//--------------------------------------------------------------
void ofApp::setState(eState state){
	currentState = state;
}

//--------------------------------------------------------------
void ofApp::draw(){
	// Drawing is just for debugging :)
	if(soundsOn) {
		ofDrawBitmapString("SOUND ON", 20, 20);
	} else {
		ofDrawBitmapString("SOUND OFF", 20, 20);
	}
	
	if(pixile.LightsOn()){
		ofDrawBitmapString("LIGHT ON", 20, 40);
	} else {
		ofDrawBitmapString("LIGHT OFF", 20, 40);
	}
	
	if(stateMicrowaveSensor == "0"){
		ofDrawBitmapString("TRIGGER", 20, 60);
	}
	
	switch (currentState) {
		case es_Eyes:
			PixelEyes.draw(180, 100, 25);
			break;
		case es_Numbers:
			drawNumbers(180, 100, 25);
			break;
	};
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	ofLog() << "Key " << key << " pressed." << std::endl;
	if(key == 115) { // s for Sleep
		switch(PixelEyes.Eyeballs.getState()){
			case eye_Normal:
				PixelEyes.Eyeballs.sleep(true);
				break;
			case eye_Sleeping:
				PixelEyes.Eyeballs.sleep(false);
				break;
		}
	} else if (key == 113) { // q for Quote
		playQuote(ofRandom(0,199));
	} else if (key == 98) { // b for Blink
		PixelEyes.Eyeballs.blink();
	} else if (key == 110) { // n for Numbers
		switch(currentState){
			case es_Eyes:
				setState(es_Numbers);
				stateNumberStartMillis = currentMillis;
				freshStateNumberDuration();
				ofLog() << "Setting State Numbers" << currentState << std::endl;
				break;
			case es_Numbers:
				setState(es_Eyes);
				ofLog() << "Setting State Eyes" << currentState << std::endl;
				break;
			default:
				ofLog() << "Key " << key << " pressed." << std::endl;
				break;
		}
	}
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

void ofApp::drawNumbers(int _x, int _y, int _scale) {
	ofPushStyle();
	ofFill();
	
	for(int i=0; i<numPCBs; i++){
		float startX = _x + (i*(6*_scale)) + (i*(drawMargin*_scale));
		float startY = _y;
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
						c = PixelEyes.Eyeballs.m_ColourEyeball;
						break;
				}
				
				ofSetColor(c);
				ofDrawRectangle(startX+(x*_scale), startY+(y*_scale), 1*_scale, 1*_scale);
				
			}
		}
	}

	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::sysCMD(std::string cmd) {
	if(!soundsOn) return;

	pid_t pid;
	pid = fork();
	if (pid == 0) {
		ofSystem(cmd.c_str());
		
		pid_t mypid = getpid();
		printf("Child id %d\n" ,mypid);
		kill(mypid,SIGKILL);

	} else {
		kill(pid, SIGTERM);
	}
}

//--------------------------------------------------------------
void ofApp::playQuote(int quoteID) {
	if(!soundsOn) return;
 	if(currentState != es_Eyes) setState(es_Eyes);

	// Make sure eyes are not sleeping on speech
	PixelEyes.Eyeballs.sleep(false);
	std::string cmd = "aplay data/audio/MOUNTAINS_QUOTE_" + std::to_string(quoteID) + ".wav & exit;";
	sysCMD(cmd);
}

//--------------------------------------------------------------
void ofApp::setEyeballColor(ofColor c){
	PixelEyes.Eyeballs.m_ColourEyeball = c;
}

//--------------------------------------------------------------
void ofApp::exit(){ 
	apa.clearAPA102(numLed+5);
	ofExit(0);
}

//--------------------------------------------------------------
void ofApp::doStateEyes(){
	ofPixels eyepix = PixelEyes.Eyeballs.getEyeballPixels();
	int led = 0;
	for(int i=0; i<PixelEyes.getEyeCount(); i++){
		for(int y = 0; y < PixelEyes.Eyeballs.getSize().y; y++){
			for(int x = 0; x < PixelEyes.Eyeballs.getSize().x; x++){
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
						c = PixelEyes.Eyeballs.m_ColourEyeball;
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
int ofApp::getMyNetworkID() {
	myNetworkID = -1;

	ofDirectory d(""); // data
	d.allowExt("id");
	d.listDir();
	
	for(int i = 0; i < d.size(); i++) {
		ofLog() << d[i].getFileName() << std::endl;
		myNetworkID = stoi(d[i].getFileName().substr(0,d[i].getFileName().find_first_of(".")));
		ofLog() << "Setting network ID: " << myNetworkID <<  std::endl;
	}
	
	return myNetworkID;
}

//--------------------------------------------------------------
void ofApp::newRandomNumbers(){
	bool playNewNumberSound = false;
	for(int &i : randomNumbers){
		int old = i;
		ofSeedRandom();
		i = ofRandom(0,2);
		if(i != old) playNewNumberSound = true;
	}
	lastNewNumberMillis = currentMillis;
	if(playNewNumberSound){
		std::string cmd = "aplay data/audio/GURU_BLINK_" + std::to_string((int)ofRandom(1,5)) + ".wav & exit";
		sysCMD(cmd);
	}
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

void ofApp::PixileMessageHandler(SPixileMessage* pMessage, void* pUserData)
{
	ofApp* pMe = static_cast<ofApp*>(pUserData);
	switch(pMessage->_id)
	{
		case 1:
		{
			if(pMessage->param[1] == 0 || pMessage->param[1] == pMe->myNetworkID) {
				pMe->playQuote(pMessage->param[0]);
			}
			break;
		}
		case 2:
		{
			pMe->PixelEyes.Eyeballs.m_ColourEyeball.r = pMessage->param[0];
			pMe->PixelEyes.Eyeballs.m_ColourEyeball.g = pMessage->param[1];
			pMe->PixelEyes.Eyeballs.m_ColourEyeball.b = pMessage->param[2];
			break;
		}
		case 3:
		{
			pMe->PixelEyes.Eyeballs.sleep(pMessage->param[0]);
		}
		default:
		{	
			ofLog() << "Unhandled Message Type: " << pMessage->_id << std::endl;
			break;
		}
	};
}
