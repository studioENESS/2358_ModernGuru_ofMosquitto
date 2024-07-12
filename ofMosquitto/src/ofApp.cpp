#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	brightness = 1;
	numLed     = 64;
	initPixelData();
	apa.setupAPA102();

	PixelEyes.setup(2);
	PixelEyes.setDrawMargin(4);

}

//--------------------------------------------------------------
void ofApp::update(){
	PixelEyes.update();
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
	apa.setAPA102(numLed,pixelData,brightness);
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
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::initPixelData(){ 
	for(int i = 0; i < numLed; i++) {
		pixelData.push_back(ofColor(0,0,0));
	}
}

//--------------------------------------------------------------
void ofApp::exit(){ 
	apa.clearAPA102(numLed+5);
	ofExit(0);
}
