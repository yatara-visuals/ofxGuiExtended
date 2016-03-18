#include "ofxGuiZoomableGraphics.h"
#include "ofGraphics.h"
using namespace std;

ofxGuiZoomableGraphics::ofxGuiZoomableGraphics(string canvasName, const ofJson& config)
	:ofxGuiGraphics(){
	_bLoaded = false;
	setName(canvasName);
	_setConfig(config);
}

ofxGuiZoomableGraphics::ofxGuiZoomableGraphics(string canvasName, ofBaseDraws * graphics, const ofJson& config)
	:ofxGuiGraphics(){
	_bLoaded = false;
	setup(canvasName, graphics);
	_setConfig(config);
}

ofxGuiZoomableGraphics::ofxGuiZoomableGraphics(string canvasName, ofBaseDraws * graphics, float w, float h){
	_bLoaded = false;
	setup(canvasName,graphics,w,h);
}

ofxGuiZoomableGraphics::~ofxGuiZoomableGraphics(){

}

void ofxGuiZoomableGraphics::setup(string graphicsName, ofBaseDraws * graphics, float w, float h){
	ofxGuiGraphics::setup(graphicsName, graphics, w, h);
	zoom_factor = 0;
	zoom_speed = 0.1;
	dragging_dst = false;
}

void ofxGuiZoomableGraphics::setSize(float w, float h){
	ofxGuiGraphics::setSize(w, h);
	contentFbo.clear();
	if(_bLoaded){
		contentFbo.allocate(getWidth(), getHeight(), GL_RGBA);
	}
	setNeedsRedraw();
}

void ofxGuiZoomableGraphics::setShape(float x, float y, float w, float h){
	ofxGuiGraphics::setShape(x, y, w, h);
	contentFbo.clear();
	if(_bLoaded){
		contentFbo.allocate(getWidth(), getHeight(), GL_RGBA);
	}
	setNeedsRedraw();
}

void ofxGuiZoomableGraphics::setShape(const ofRectangle &r){
	setShape(r.x, r.y, r.width, r.height);
}

void ofxGuiZoomableGraphics::generateDraw(){
	ofxGuiGraphics::generateDraw();

}

void ofxGuiZoomableGraphics::render(){

	if(_bLoaded){

		contentFbo.begin();
		ofClear(0, 0, 0, 0);

		ofPushMatrix();

//		ofTranslate(-b.getPosition());

		zoom_translation = zoom_point - zoom_point_scaled + zoom_point_offset;
		if(zoom_translation.x > 0){
			zoom_translation.x = 0;
		}
		if(zoom_translation.y > 0){
			zoom_translation.y = 0;
		}
		if(zoom_translation.x < -addZoom(getWidth()) + getWidth()){
			zoom_translation.x = -addZoom(getWidth()) + getWidth();
		}
		if(zoom_translation.y < -addZoom(getHeight()) + getHeight()){
			zoom_translation.y = -addZoom(getHeight()) + getHeight();
		}
		ofTranslate(zoom_translation);

		graphics->draw(0, 0, addZoom(getWidth()), addZoom(getHeight()));

		ofPopMatrix();

		contentFbo.end();

	}

	ofColor c = ofGetStyle().color;

	bg.draw();
	if(_bLoaded){
		contentFbo.draw(0, 0, getWidth(), getHeight());
	}

	if(showName){
		ofBlendMode blendMode = ofGetStyle().blendingMode;
		if(blendMode != OF_BLENDMODE_ALPHA){
			ofEnableAlphaBlending();
		}
		ofSetColor(textColor);

		bindFontTexture();
		textMesh.draw();
		unbindFontTexture();

		ofSetColor(c);
		if(blendMode != OF_BLENDMODE_ALPHA){
			ofEnableBlendMode(blendMode);
		}
	}
}

bool ofxGuiZoomableGraphics::mouseDragged(ofMouseEventArgs & args){
	ofPoint mouse(args.x, args.y);
	if(dragging_dst){
		zoom_point_offset += mouse - last_mouse;
		last_mouse = mouse;
		setNeedsRedraw();
	}
	return false;
}

bool ofxGuiZoomableGraphics::mousePressed(ofMouseEventArgs & args){

	if(isMouseOver()){
		dragging_dst = true;
		last_mouse = ofPoint(args.x, args.y);
	}
	return false;
}

bool ofxGuiZoomableGraphics::mouseReleased(ofMouseEventArgs & args){
	dragging_dst = false;
	return false;
}

bool ofxGuiZoomableGraphics::mouseScrolled(ofMouseEventArgs & args){

	if(isMouseOver()){
		setZoomFactor(args.scrollY);
		setNeedsRedraw();
	}

	return false;
}

void ofxGuiZoomableGraphics::setZoomFactor(int factor){

	int old_zoom_factor = zoom_factor;

	zoom_factor += factor;
	if(zoom_factor < 0){
		zoom_factor = 0;
	}

	ofPoint zoom_point_old = zoom_point;

	ofPoint tmp_zoom_point;
	tmp_zoom_point.x = ofGetMouseX() - getPosition().x - zoom_point_offset.x;
	tmp_zoom_point.y = ofGetMouseY() - getPosition().y - zoom_point_offset.y;

	ofVec2f diff = tmp_zoom_point - zoom_point_old;

	if(old_zoom_factor == 0){
		diff = ofPoint(0, 0);
		zoom_point_offset = ofPoint(0, 0);
		zoom_point_old = tmp_zoom_point;
	}

	zoom_point = zoom_point_old + removeZoom(diff);
	zoom_point_offset += tmp_zoom_point - zoom_point;
	zoom_point_scaled = addZoom(zoom_point);

}

ofPoint ofxGuiZoomableGraphics::addZoom(ofPoint p){
	return p * (1 + zoom_factor * zoom_speed);
}

float ofxGuiZoomableGraphics::addZoom(float p){
	return p * (1 + zoom_factor * zoom_speed);
}


ofPoint ofxGuiZoomableGraphics::removeZoom(ofPoint p){
	return p / (1 + zoom_factor * zoom_speed);
}
