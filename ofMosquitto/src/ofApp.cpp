#include "ofApp.h"

static void to_json(ofJson& j, const ofApp::appState& state)
{
	j = ofJson {
		{ "name", state.name },
		{ "state", state.state },
		{ "stateIntervalMin", state.stateIntervalMin },
		{ "stateIntervalMax", state.stateIntervalMax }
	};
}
 
static void from_json(const ofJson& j, ofApp::appState& state)
{
	try {
		j.at("name").get_to(state.name);
		j.at("state").get_to(state.state);
		j.at("stateIntervalMin").get_to(state.stateIntervalMin);
		j.at("stateIntervalMax").get_to(state.stateIntervalMax);
	}
	catch (const ofJson::exception& e) {
			std::cout << e.what() << std::endl;
	}
}

static void to_json(ofJson& j, const ofApp::eyeMovementState& state)
{
	j = ofJson{
		{ "name", state.name },
		{ "moveIntervalMin", state.moveIntervalMin },
		{ "moveIntervalMax", state.moveIntervalMax },
		{ "targetIntervalMin", state.targetIntervalMin },
		{ "targetIntervalMax", state.targetIntervalMax },
		{ "stateIntervalMin", state.stateIntervalMin },
		{ "stateIntervalMax", state.stateIntervalMax }
	};
}

static void from_json(const ofJson& j, ofApp::eyeMovementState& state)
{
	try {
		j.at("name").get_to(state.name);
		j.at("moveIntervalMin").get_to(state.moveIntervalMin);
		j.at("moveIntervalMax").get_to(state.moveIntervalMax);
		j.at("targetIntervalMin").get_to(state.targetIntervalMin);
		j.at("targetIntervalMax").get_to(state.targetIntervalMax);
		j.at("stateIntervalMin").get_to(state.stateIntervalMin);
		j.at("stateIntervalMax").get_to(state.stateIntervalMax);
	}
	catch (const ofJson::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

//--------------------------------------------------------------
ofJson ofApp::saveConfig() {
	ofJson jconf;
	jconf["myNetworkID"]   = myNetworkID;
	jconf["mapUpsideDown"] = mapUpsideDown;
	jconf["numPCBs"]       = numPCBs;
	jconf["drawMargin"]    = drawMargin;
	jconf["drawScale"]     = drawScale;
	ofSaveJson("config.json", jconf);
	return jconf;
}

//--------------------------------------------------------------
void ofApp::loadConfig() {
	ofJson jconf;
	ofFile file("config.json");
	if (file.exists()) {
		file >> jconf;
	}
	else { // Init
		jconf = saveConfig();
	}
	
	myNetworkID   = jconf.value("myNetworkID", myNetworkID);
	mapUpsideDown = jconf.value("mapUpsideDown", false);
	numPCBs       = jconf.value("numPCBs", numPCBs);
	drawMargin    = jconf.value("drawMargin", drawMargin);
	drawScale     = jconf.value("drawScale", drawScale);

}

//--------------------------------------------------------------
ofJson ofApp::getSettings() {
	ofJson settings;

	settings["brightness"]  = brightness;
	
	// START TIMERS \\---------------------------------------------
	// TODO Save App State settings here
	// The speed of number change
	settings["newNumberInterval"]      = newNumberInterval;
	// END TIMERS -------------------------------------------------

	settings["pixelEyeSettings"] = PixelEyes.getSettings();

	return settings;
}

//--------------------------------------------------------------
void ofApp::setSettings(ofJson settings) {
	//myNetworkID = settings.value("myNetworkID", myNetworkID);
	//numPCBs = settings.value("numPCBs", numPCBs);
	//drawMargin  = settings.value("drawMargin" , drawMargin);
	//drawScale   = settings.value("drawScale"  , drawScale);
	brightness  = settings.value("brightness" , brightness);

	// START TIMERS \\---------------------------------------------
	// TODO Load App States
	// The speed of number change
	newNumberInterval      = settings.value("newNumberInterval", newNumberInterval);
	// END TIMERS -------------------------------------------------

	ofJson pixelEyeSettings;
	pixelEyeSettings = settings.value("pixelEyeSettings", pixelEyeSettings);
	PixelEyes.setSettings(pixelEyeSettings);

}

//--------------------------------------------------------------
void ofApp::setup() {
#ifdef __arm__
	gpioMicrowaveSensor = new GPIO("23");
	gpioMicrowaveSensor->export_gpio();
	gpioMicrowaveSensor->setdir_gpio("in");
	apa.setupAPA102();
#endif

	myNetworkID = 0;
	mapUpsideDown = false;
	numPCBs = 2;
	drawMargin = 4;
	drawScale = 25;
	loadConfig();
	
	gui.setup(nullptr, false, ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable, true);
	ImGui::StyleColorsLight();

	stateMicrowaveSensor = "1"; //pull-down logic
	
	brightness = 1;
	numLed     = 32*numPCBs;

	outputTexture.allocate(6 * numPCBs, 6, GL_RGBA);
	outputTexture.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

	initPixelData();
	
	for(int i=0; i<numPCBs; i++){
		randomNumbers.push_back(ofRandom(0,2));
	}

	currentStateApp = esa_Eyes;
	
	soundsOn = true;
	ofSeedRandom();

	PixelEyes.setup(numPCBs);
	PixelEyes.setDrawMargin(drawMargin);
	PixelEyes.Eyeballs.setSoundOn(soundsOn);

#ifdef MICROWAVE_INSTALLED
	PixelEyes.Eyeballs.canSleep(true);
#else
	PixelEyes.Eyeballs.canSleep(false);
#endif

	currentMillis = ofGetElapsedTimeMillis();
	lastStateAppMillis = currentMillis;
	lastStateEyeMovementMillis = currentMillis;
	lastNewNumberMillis = currentMillis;
	
	// The speed of number change
	newNumberInterval = 250;

	// Pixile Communicator
	// Can we have a nicer setup?
	pixile.Master(false);
	pixile.Computer_id(myNetworkID);
	pixile.Server_port(3637);
	pixile.SetupSockets();
	pixile.SetMessageHandler(&PixileMessageHandler, this);

	loadStates();

	// Set State Collection Timers 
	freshStateAppInterval();
	freshStateEyeMovementInterval();
}

//--------------------------------------------------------------
void ofApp::update(){
	currentMillis = ofGetElapsedTimeMillis();
	// Update App State
	if (currentMillis - lastStateAppMillis > stateAppInterval) {
		setState((eStateApp)(((int)currentStateApp + 1) % vStatesApp.size()));
	}

	// Update Eye Movement State
	if (currentMillis - lastStateEyeMovementMillis > stateEyeMovementInterval) {
		iSelectedStateEyeIndex = ofRandom(0, vEyeMovementStates.size());
		setEyeMovementState(vEyeMovementStates[iSelectedStateEyeIndex]);
	};

	pixile.update();
	
	lightsOn = pixile.LightsOn();
	if (pixile.SoundsOn() != soundsOn) {
		soundsOn = pixile.SoundsOn();
		PixelEyes.Eyeballs.setSoundOn(soundsOn);
	}

	switch (currentStateApp) {
		case esa_Eyes:
			PixelEyes.update();
			doStateEyes();
			break;
		case esa_Numbers:
			doStateNumbers();
			break;
	};

	// Update LEDS
	ofPixels pix;
	outputTexture.readToPixels(pix);

	int led = 0;
	for(int i = 0; i < numPCBs; i++) {
		for(int y = 0; y < 6; y++) {
			for(int x = 0; x < 6; x++) {
				int px;
				int py;
				int pi;
				px = mapUpsideDown ? 5-x: x;
				py = mapUpsideDown ? 5-y: y;
				pi = mapUpsideDown ? (numPCBs-1)-i: i;
				ofColor c = pix.getColor(px+(pi*6), py);
				if(c.a > 0){
					pixelData[led] = c;
					led++;
				}
			}
		}
	}

#ifdef __arm__
	if(!pixile.LightsOn()){
		// Leds OFF
		apa.setAPA102(numLed,pixelDataOFF,0);
	} else {
		// Update LEDs
		apa.setAPA102(numLed,pixelData,brightness);
	};
#endif

	
#ifdef MICROWAVE_INSTALLED
	gpioMicrowaveSensor->getval_gpio(stateMicrowaveSensor);
	//ofLog() << stateMicrowaveSensor << std::endl;
	if(stateMicrowaveSensor == "0") PixelEyes.Eyeballs.sleep(false);
#endif

}

//--------------------------------------------------------------
void ofApp::setState(eStateApp toState) {
	switch (toState) {
	case esa_Numbers:
		// Don't change state if eyes are sleeping
		if (!PixelEyes.Eyeballs.isSleeping()) {
			currentStateApp = toState;
		}
		break;
	case esa_Eyes:
		currentStateApp = toState;
		break;
	default:
		ofLog(OF_LOG_WARNING) << "Not a valid app state: " << toState << std::endl;
	}

	freshStateAppInterval();
}

//--------------------------------------------------------------
void ofApp::draw(){

	// Draw Preview ...
	static const int startX = 100;
	static const int startY = 50;
	for (int i = 0; i < numPCBs; i++) {
		float x = startX + (i * (6 * drawScale)) + (i * (drawMargin * drawScale));
		float y = startY;
		outputTexture.getTexture().drawSubsection(x, y, 0, (6 * drawScale), (6 * drawScale), i * 6, 0, 6, 6);
	}
	
	gui.begin();

	// Define the ofWindow as a docking space
	ImGuiID dockNodeID = ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_PassthruCentralNode);

	// Draw some windows
	drawAppSettingsWindow();	
	drawEyeSettingsWindow();
	drawEyeStateCollectionWindow();
	drawAppStateCollectionWindow();

	gui.end();
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	ofLog() << "Key " << key << " pressed." << std::endl;
	if(key == 's') { // s for Sleep
		switch(PixelEyes.Eyeballs.getState()){
			case eye_Normal:
				PixelEyes.Eyeballs.sleep(true);
				break;
			case eye_Sleeping:
				PixelEyes.Eyeballs.sleep(false);
				break;
		}
	} else if (key == 'u') { // map Upside-down
		mapUpsideDown = !mapUpsideDown;
		saveConfig();
	} else if (key == 'q') { // q for Quote
		playQuote(ofRandom(1,299));
	} else if (key == 'b') { // b for Blink
		PixelEyes.Eyeballs.blink();
	} else if (key == 'n') { // n for Numbers
		switch(currentStateApp){
			case esa_Eyes:
				if (soundPlayer1.ready() && soundPlayer2.ready()){
					setState(esa_Numbers);
					ofLog() << "Setting State Numbers" << currentStateApp << std::endl;
				} else {
					ofLog() << "Ignored State Number request as Guru was speaking, did not reset the timer." << currentStateApp << std::endl;
				}
				
				
				break;
			case esa_Numbers:
				setState(esa_Eyes);
				ofLog() << "Setting State Eyes" << currentStateApp << std::endl;
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

//--------------------------------------------------------------
void ofApp::setEyeMovementState(eyeMovementState s){
	PixelEyes.Eyeballs.moveIntervalMin   = s.moveIntervalMin;
	PixelEyes.Eyeballs.moveIntervalMax   = s.moveIntervalMax;
	PixelEyes.Eyeballs.targetIntervalMin = s.targetIntervalMin;
	PixelEyes.Eyeballs.targetIntervalMax = s.targetIntervalMax;
	PixelEyes.Eyeballs.freshMoveInterval();
	PixelEyes.Eyeballs.freshTargetInterval();
	freshStateEyeMovementInterval();
}

//--------------------------------------------------------------
void ofApp::addCurrentState(std::string name /*="Untitled"*/) {
	eyeMovementState state = {
		name,
		PixelEyes.Eyeballs.moveIntervalMin,
		PixelEyes.Eyeballs.moveIntervalMax,
		PixelEyes.Eyeballs.targetIntervalMin,
		PixelEyes.Eyeballs.targetIntervalMax,
		10000,
		60000,
	};
	vEyeMovementStates.push_back(state);
}

//--------------------------------------------------------------
void ofApp::loadStates() {
	vEyeMovementStates.clear();
	ofJson loaded = ofLoadJson("EyeMovementStates.json");
	if(!loaded.empty()) loaded.get_to(vEyeMovementStates);
	if (vEyeMovementStates.size() < 1) addCurrentState();

	vStatesApp.clear();
	loaded = ofLoadJson("AppStates.json");
	if (!loaded.empty()) loaded.get_to(vStatesApp);
	if (vStatesApp.size() < 1) {
		// Add init app states
		vStatesApp.push_back({
			"Eyes",
			esa_Eyes,
			20000,
			50000
			});

		vStatesApp.push_back({
			"Numbers",
			esa_Numbers,
			3000,
			9000
			});
	};
}

//--------------------------------------------------------------
void ofApp::freshStateAppInterval() {
	lastStateAppMillis = currentMillis;
	stateAppInterval = ofRandom(vStatesApp[(int)currentStateApp].stateIntervalMin, vStatesApp[(int)currentStateApp].stateIntervalMax);
}

//--------------------------------------------------------------
void ofApp::freshStateEyeMovementInterval() {
	lastStateEyeMovementMillis = currentMillis;
	stateEyeMovementInterval = ofRandom(vEyeMovementStates[iSelectedStateEyeIndex].stateIntervalMin, vEyeMovementStates[iSelectedStateEyeIndex].stateIntervalMax);
}

//--------------------------------------------------------------
void ofApp::drawAppSettingsWindow() {
	ImGui::Begin("App Settings");

	ImGui::Checkbox("Sound", &soundsOn);
	ImGui::Checkbox("Light", &lightsOn);
	ImGui::Spacing();
	ImGui::Checkbox("Reverse Mapping", &mapUpsideDown);

	ImGui::End();
}

//--------------------------------------------------------------
void ofApp::drawEyeSettingsWindow() {
	ImGui::Begin("Current");
	ImGui::Checkbox("Allow Change Colour On Blink", &PixelEyes.Eyeballs.changeColourOnBlink);
	
	ImGui::Separator();
	ImGui::PushItemWidth(50);
	ImGui::DragScalar("Move Interval Min", ImGuiDataType_U32, &PixelEyes.Eyeballs.moveIntervalMin, 10);
	ImGui::DragScalar("Move Interval Max", ImGuiDataType_U32, &PixelEyes.Eyeballs.moveIntervalMax, 10);
	ImGui::DragScalar("Target Interval Min", ImGuiDataType_U32, &PixelEyes.Eyeballs.targetIntervalMin, 10);
	ImGui::DragScalar("Target Interval Max", ImGuiDataType_U32, &PixelEyes.Eyeballs.targetIntervalMax, 10);
	ImGui::PopItemWidth();
	
	ImGui::Spacing();
	if (ImGui::Button("Add to state collection"))
	{
		addCurrentState();
	}
	ImGui::Separator();

	ImGui::End();
}
//--------------------------------------------------------------
void ofApp::drawAppStateCollectionWindow() {
	ImGui::Begin("App State");

	if (vStatesApp.size() > 0) {
		if (ImGui::BeginTable("App State", 1 + vStatesApp.size()))
		{
			for (int row = 0; row < 3; row++)
			{
				if (row == 0)
				{
					ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
					ImGui::TableNextColumn();
					if (ImGui::Button("Save"))
					{
						ofSavePrettyJson("AppStates.json", vStatesApp);
					}
				}
				else
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					std::string sRowName = "";
					switch (row - 1)
					{
					case 0:
						sRowName = "State Interval Min";
						break;
					case 1:
						sRowName = "State Interval Max";
						break;
					default:
						sRowName = "Bad DATA";
						break;
					}
					ImGui::Text(sRowName.c_str());
				}

				ImGui::TableSetColumnIndex(0);

				for (int column = 0; column < vStatesApp.size(); column++)
				{
					ImGui::TableSetColumnIndex(1 + column);
					if (row == 0)
					{
						// check if active state.
						if (column == currentStateApp)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
							ImGui::TableHeader(vStatesApp[column].name.c_str());
							ImGui::PopStyleColor();
						}
						else {
							ImGui::TableHeader(vStatesApp[column].name.c_str());
							if (ImGui::IsItemClicked(0))
							{
								setState(vStatesApp[column].state);
							}
						}
					}
					else {
						std::string sName;
						sName = "###" + std::to_string(row) + " " + std::to_string(column);
						ImGui::DragScalar(sName.c_str(), ImGuiDataType_U32, &vStatesApp[column].values[row - 1], 10);
					}
				}
			}
			ImGui::EndTable();
		}
	}

	ImGui::End();
}
//--------------------------------------------------------------
void ofApp::drawEyeStateCollectionWindow() {
	ImGui::Begin("Eye State");
	
	if (vEyeMovementStates.size() > 0) {
		if (ImGui::BeginTable("State Collection", 1+vEyeMovementStates.size()))
		{
			for (int row = 0; row < 7; row++)
			{
				if (row == 0)
				{
					ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
					ImGui::TableNextColumn();
					if (ImGui::Button("Save"))
					{
						ofSavePrettyJson("EyeMovementStates.json", vEyeMovementStates);
					}
				}
				else
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					std::string sRowName = "";
					switch (row-1)
					{
					case 0:
						sRowName = "Move Interval Min";
						break;
					case 1:
						sRowName = "Move Interval Max";
						break;
					case 2:
						sRowName = "Target Interval Min";
						break;
					case 3:
						sRowName = "Target Interval Max";
						break;
					case 4:
						sRowName = "State Interval Min";
						break;
					case 5:
						sRowName = "State Interval Max";
						break;
					default:
						sRowName = "Bad DATA";
						break;
					}
					ImGui::Text(sRowName.c_str());
				}

				ImGui::TableSetColumnIndex(0);
				
				for (int column = 0; column < vEyeMovementStates.size(); column++)
				{
					ImGui::TableSetColumnIndex(1 + column);
					if (row == 0)
					{
						// check if active state.
						if (column == iSelectedStateEyeIndex)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
							ImGui::TableHeader(vEyeMovementStates[column].name.c_str());							
							ImGui::PopStyleColor();
						}
						else {
							ImGui::TableHeader(vEyeMovementStates[column].name.c_str());
							if (ImGui::IsItemClicked(0))
							{
								// Change to this state.
								iSelectedStateEyeIndex = column;
								setEyeMovementState(vEyeMovementStates[column]);
							}
						}
					}
					else {
						
						std::string sName;
						sName = "###" + std::to_string(row) + " " + std::to_string(column);
						ImGui::DragScalar(sName.c_str(), ImGuiDataType_U32, &vEyeMovementStates[column].values[row-1], 10);
					}
				}
			}
			ImGui::EndTable();
		}
	}

	ImGui::End();
}

//--------------------------------------------------------------
void ofApp::playSound(std::string fileName) {
	if(!soundsOn) return;
	if (soundPlayer1.ready()){
		soundPlayer1.play(fileName);
	} else if (soundPlayer2.ready()) {
		soundPlayer2.play(fileName);
	} else {
		ofLog() << "Failed to play file " << fileName;
	}
}

//--------------------------------------------------------------
void ofApp::playQuote(int quoteID) {
	if(!soundsOn) return;
 	if(currentStateApp != esa_Eyes) setState(esa_Eyes);

	// Make sure state is on Eye state
	if(PixelEyes.Eyeballs.getState() != eye_Normal) {
	    PixelEyes.Eyeballs.setState(eye_Normal);
	} else {
		// Make sure eyes are not sleeping on speech
		PixelEyes.Eyeballs.sleep(false);
	}

	playSound("MOUNTAINS_QUOTE_" + std::to_string(quoteID) + ".wav");
}

//--------------------------------------------------------------
void ofApp::setEyeballColor(ofColor c){
	PixelEyes.Eyeballs.m_ColourEyeball = c;
}

//--------------------------------------------------------------
void ofApp::exit(){
#ifdef __arm__
	apa.clearAPA102(numLed+5);
#endif
	ofExit(0);
}

//--------------------------------------------------------------
void ofApp::doStateEyes(){
	outputTexture.begin();
	ofClear(0,0,0,0);
	PixelEyes.Eyeballs.draw(0, 0);
	PixelEyes.Eyeballs.draw(6, 0);
	outputTexture.end();
}

//--------------------------------------------------------------
void ofApp::doStateNumbers(){
	if(currentMillis - lastNewNumberMillis > newNumberInterval) newRandomNumbers();

	outputTexture.begin();
	ofPushStyle();
	ofClear(0, 0, 0, 0);
	for (int i = 0; i < numPCBs; i++) {
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				ofColor c = ofColor(0, 0, 0, 255); //OFF
				switch (numberMap[randomNumbers[i]][6 * y + x]) {
				case -1:
					continue; // No Led at this position
					break;
				case 0: // LED OFF
					ofSetColor(c);
					ofDrawRectangle(x + (i * 6), y, 1, 1);
					break;
				case 1: // LED ON
					ofSetColor(PixelEyes.Eyeballs.m_ColourEyeball);
					ofDrawRectangle(x + (i * 6), y, 1, 1);
					break;
				}
			}
		}
	};
	ofPopStyle();
	outputTexture.end();

}

//--------------------------------------------------------------
void ofApp::newRandomNumbers(){
	bool playNewNumberSound = false;
	for(int &i : randomNumbers){
		int old = i;
#ifdef __linux__
		ofSeedRandom();
#endif
		i = ofRandom(0,2);
		if(i != old) playNewNumberSound = true;
	}
	lastNewNumberMillis = currentMillis;
	if(playNewNumberSound) {
		playSound("GURU_BLINK_" + std::to_string((int)ofRandom(1, 5)) + ".wav");
	}
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
		case 1: // Play Quote
		{
			if(pMessage->param[1] == 0 || pMessage->param[1] == pMe->myNetworkID) {
				pMe->playQuote(pMessage->param[0]);
			}
			break;
		}
		case 2: // Update eyeball colour
		{
			pMe->PixelEyes.Eyeballs.m_ColourEyeball.r = pMessage->param[0];
			pMe->PixelEyes.Eyeballs.m_ColourEyeball.g = pMessage->param[1];
			pMe->PixelEyes.Eyeballs.m_ColourEyeball.b = pMessage->param[2];
			break;
		}
		case 3: // Sleep (true), Wakeup (false)
		{
			pMe->PixelEyes.Eyeballs.sleep(pMessage->param[0]);
			break;
		}
		case 4: // Set Number State
		{
			pMe->setState(esa_Numbers);
			break;
		}
		case 5: // Map Upsidedown (true upsideDown, false map Normal)
			pMe->mapUpsideDown = pMessage->param[0];
			pMe->saveConfig();
			break;
		default:
		{	
			ofLog() << "Unhandled Message Type: " << pMessage->_id << std::endl;
			break;
		}
	};
}
