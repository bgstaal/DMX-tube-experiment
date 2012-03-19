#pragma once

#include "ofMain.h"
#include "DmxPro.h"
#include "ofxCvMain.h"
#include "ofxUI.h"

#define SERIAL_PORT_ADDRESS "/dev/tty.usbserial-EN079717"
#define NUM_LED_POINTS 16
#define NUM_CHANNELS 48

class testApp : public ofBaseApp
{

	public:
		void setup();
		void update();
		void draw();

    
        void resetDmxPackage();
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);
        void updateVideo();
        void updateColors();
        void createDMXPackageFromColors();
        void updateHRatio();
        void updateHue();
        void drawTube();
        void drawVideoFrames();
        void setupVideo();
        void setupDMX();
        void setupColors();
        void setupOpenCV();
        void setupGUI(); 
        void guiEvent(ofxUIEventArgs &e);
        
        float brightness;
        float contrast;
        int threshold;
    
        ofxCvBlob biggestBlob;
        bool isConnected;
        float hRatio;
        int currentIndex;
        ofColor *colors;
        ofColor theColor;
        int dmxChannelAmount;
        unsigned char* dmxPacket;
        DmxPro dmx;
        int minBlobSize;
        int maxBlobSize;
    
        ofxUICanvas *gui;
    
        ofxCvContourFinder contourFinder;
        ofxCvColorImage colorFrame; 
        ofxCvGrayscaleImage grayImage;
    
        ofVideoGrabber videoGrabber;
};
