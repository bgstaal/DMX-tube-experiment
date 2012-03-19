#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofBackground(50, 50, 50);
    ofSetFrameRate(40);
    setupColors();
    setupVideo();
    setupOpenCV();
    setupDMX();
    setupGUI();
}

void testApp::setupGUI()
{
    gui = new ofxUICanvas(0, ofGetHeight()-300, ofGetWidth(), 300);
    gui->setFont("GUI/faucet.ttf");
    gui->addWidgetDown(new ofxUIRotarySlider(100, -1, 1, 0, "brightness"));
    gui->addWidgetRight(new ofxUIRotarySlider(100, -1, -1, 0, "contrast"));
    gui->addWidgetRight(new ofxUIRotarySlider(100, 0, 255, 200, "threshold"));
    gui->addWidgetRight(new ofxUIRotarySlider(100, 0, 5000, 200, "minBlobSize"));
    gui->addWidgetRight(new ofxUIRotarySlider(100, 0, 5000, 400, "maxBlobSize"));
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}

void testApp::setupColors()
{
    colors = new ofColor [NUM_LED_POINTS];
    theColor = ofColor(255, 255, 0);
}

void testApp::setupOpenCV()
{
    colorFrame.allocate(videoGrabber.width, videoGrabber.height);
    grayImage.allocate(videoGrabber.width, videoGrabber.height);
}

void testApp::setupVideo()
{
    brightness = 0.0;
    contrast = 0.5;
    threshold = 200;
    minBlobSize = 100;
    maxBlobSize = 5000;
    videoGrabber.initGrabber(640, 480);
}

void testApp::setupDMX()
{
    dmxChannelAmount = NUM_CHANNELS;
    dmxPacket = new unsigned char[ dmxChannelAmount +1];
    
    string serialPortAddress = SERIAL_PORT_ADDRESS;
    isConnected = dmx.connect(serialPortAddress, 512);
    
    if( !isConnected )
    {
        ofLogError() << "We failed to connect to " << serialPortAddress << endl;
    }
    else 
    {
        printf("Yeah! connected!");
        resetDmxPackage();
    }
}


//--------------------------------------------------------------
void testApp::update()
{
    updateVideo();
    updateHue();
    resetDmxPackage();
    updateColors();
    createDMXPackageFromColors();
    if (isConnected) dmx.sendLevels(dmxPacket, dmxChannelAmount);
}

void testApp::updateVideo()
{
    videoGrabber.update();
    
    if(videoGrabber.isFrameNew())
    {
        colorFrame.setFromPixels(videoGrabber.getPixels(), videoGrabber.width, videoGrabber.height);
        grayImage = colorFrame;
        grayImage.brightnessContrast(brightness, contrast);
        grayImage.threshold(threshold);
        contourFinder.findContours(grayImage, minBlobSize, maxBlobSize, 100, false, false);
        
        if (contourFinder.nBlobs > 0)
        {
            biggestBlob = contourFinder.blobs[0];
            
            for (int i = 0; i < contourFinder.nBlobs; i++)
            {
                ofxCvBlob currentBlob = contourFinder.blobs[i];
                if (currentBlob.area > biggestBlob.area)
                {
                    biggestBlob = currentBlob;
                }
            }
            
            hRatio = (float)biggestBlob.centroid.x / grayImage.width;
            if (hRatio > 1) hRatio = 1;
            if (hRatio < 0) hRatio = 0;
            
            currentIndex = (NUM_LED_POINTS-1) * hRatio;
            
            
        }
    }
}

void testApp::updateHue()
{
    float hue = theColor.getHue()+1;
    if (hue > 244.0f) hue = 0.0f;
    theColor.setHue(hue);
}

void testApp::resetDmxPackage()
{
    for (int i = 0; i <= dmxChannelAmount; i++)
    {
        dmxPacket[i+1] = 0;
    }
}

void testApp::updateColors()
{
    for (int i = 0; i <= NUM_LED_POINTS; i++)
    {
        if (i == currentIndex)
        {
            colors[i] = theColor;
        }
        else 
        {
            colors[i] = ofColor(0, 0, 0);
        }
    }
}

void testApp::createDMXPackageFromColors()
{
    for (int i = 0; i <= NUM_LED_POINTS; i++)
    {
        int colorIndex = i-1;
        if (colorIndex < 0) colorIndex = 0;
        ofColor color = colors[i];
        
        int index = (i*3)+1;
        dmxPacket[index] = color.r;
        dmxPacket[index+1] = color.g;
        dmxPacket[index+2] = color.b;
    }
}

//--------------------------------------------------------------
void testApp::draw()
{
    drawVideoFrames();
    drawTube();
    gui->draw();
}

void testApp::drawVideoFrames()
{
    ofSetColor(255, 255, 255);
    colorFrame.draw(0, 0);
    grayImage.draw(videoGrabber.width, 0);
    
    biggestBlob.draw();

}


void testApp::drawTube()
{
    float stepSize = ofGetWidth()/(NUM_LED_POINTS-1);
    
    glLineWidth(50.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < NUM_LED_POINTS; i++)
    {
        ofColor color = colors[i];
        ofSetColor(color);
        glVertex2f(stepSize * i, videoGrabber.height);
    }
    glEnd();
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
    ofxUISlider *slider = (ofxUISlider *)e.widget;
    string name = slider->getName();
    float value = slider->getScaledValue();
    
    if (name == "brightness")
    {
        brightness = value;
    }
    else if (name == "contrast")
    {
        contrast = value;
    }
    else if (name == "threshold")
    {
        threshold = value;
    }
    else if (name == "minBlobSize")
    {
        minBlobSize = (int)value;
        cout << minBlobSize;
    }
    else if (name == "maxBlobSize")
    {
        maxBlobSize = (int)value;
        cout << maxBlobSize;
    }
}


//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{
    hRatio = (float)x / ofGetWidth();
    if (hRatio > 1) hRatio = 1;
    if (hRatio < 0) hRatio = 0;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}