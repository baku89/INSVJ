#pragma once

#include "ofMain.h"

#include "ofxOsc.h"
#include "ofxDmx.h"
#include "ofxImGui.h"

#define	MARGIN		10
#define MAX_TIME	5.0f

struct Light {
	bool on;
	float time;
};

class ofApp : public ofBaseApp{

public:
	void setup() {
		
		osc.setup(8000);
		
		for (int i = 0; i < 3; i++) {
			channels.push_back((Light){false, 0.0f});
		}
		
		dmx.connect("/dev/tty.usbserial-EN159284");
	}
	
	void update(){
		
		static float elapsedTime, prevElapsedTime, deltaTime;
		
		elapsedTime = ofGetElapsedTimef();
		deltaTime = elapsedTime - prevElapsedTime;
		prevElapsedTime = elapsedTime;
		
		
		while (osc.hasWaitingMessages()) {
			ofxOscMessage m;
			osc.getNextMessage(&m);
			string address = m.getAddress();
			
			if (address == "/light/ch1") {
				channels[0].on = m.getArgAsBool(0);
			} else if (address == "/light/ch2") {
				channels[1].on = m.getArgAsBool(0);
			} else if (address == "/light/ch3") {
				channels[2].on = m.getArgAsBool(0);
			} else if (address == "/light/amp") {
				amp = m.getArgAsFloat(0);
			}
		}
		
//
		
		for (int i = 0; i < channels.size(); i++) {
			
			if (channels[i].on) {
				dmx.setLevel(i + 1, amp * 255.0);
				channels[i].time += deltaTime;
			} else {
				dmx.setLevel(i + 1, 0);
			}
		}
		dmx.update();
	}
	
	void draw() {
		
		static float w, h, cw;
		
		ofBackground(0);
		
		ofSetColor(255);
		
		w = ofGetWidth();
		h = ofGetHeight();
		
		ofDrawRectangle(0, 0, w * amp, h * 0.1);
		
		ofPushMatrix();
		ofTranslate(0, h * 0.1 + MARGIN);
		
		cw = (h * 0.9 - MARGIN * channels.size())  / (float)channels.size();
		
		for (auto& ch : channels) {
			
			if (ch.on)	ofDrawRectangle(0, 0, w * (ch.time / MAX_TIME * 0.5 + 0.5), cw);
			
			ofTranslate(0, cw + MARGIN);
		}
		
		ofPopMatrix();
	}
	
	void setChannelByKey(int key, bool value) {
		int ci = key - '0';
		
		if (1 <= ci && ci <= 9 && ci <= channels.size())  {
			channels[ci - 1].on = value;
			if (!value)
				channels[ci - 1].time = 0;
		}
	}

	void keyPressed(int key) {
		setChannelByKey(key, true);
	}
	
	void keyReleased(int key) {
		setChannelByKey(key, false);
	}
	
	void mouseMoved(int x, int y) {
		amp = (float)ofGetMouseX() / ofGetWidth();
	}
	
	ofxImGui::Gui	gui;
	ofxDmx			dmx;
	ofxOscReceiver osc;
	
	float			amp;
	vector<Light>	channels;
		
};
