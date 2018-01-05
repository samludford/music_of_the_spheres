/* This is an example of how to integrate maximilain into openFrameworks,
 including using audio received for input and audio requested for output.
 
 
 You can copy and paste this and use it as a starting example.
 
 */

#include "ofApp.h"

// Minor Pentatonic
//#define SCALE_SIZE 15
//float scale[SCALE_SIZE] = { 110.0, 130.81, 146.83, 164.81, 196.00, 220.00, 261.63, 293.66, 329.63, 392.00, 440.00, 523.25, 587.33, 659.25, 783.99 };

// Phrygrian mode
//#define SCALE_SIZE 21
//float scale[SCALE_SIZE] = { 110.0, 116.54, 130.81, 146.83, 164.81, 174.61, 196.00, 220.00, 233.08, 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 466.16, 523.25, 587.33, 659.25, 698.46, 783.99};

// Hungarian minor - A B C D# E F G# A
//#define SCALE_SIZE 21
//float scale[SCALE_SIZE] = { 110.0, 123.47, 130.81, 155.56, 164.81, 174.61, 207.65, 220.00, 246.94, 261.63, 311.13, 329.63, 349.23, 415.30, 440.00, 493.88, 523.25, 622.25, 659.25, 698.46, 830.61};

// Major - A B C# D E F# G#
//#define SCALE_SIZE 21
//float scale[SCALE_SIZE] = { 110.0, 123.47, 138.59, 146.83, 164.81, 185.00, 207.65, 220.00, 246.94, 277.18, 293.66, 329.63, 369.99, 415.30, 440.00, 493.88, 554.37, 587.33, 659.25, 739.99, 830.61};

// Minor - A B C D E F G
//#define SCALE_SIZE 21
//float scale[SCALE_SIZE] = { 110.0, 123.47, 130.81, 146.83, 164.81, 174.61, 196.00, 220.00, 246.94, 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25, 587.33, 659.25, 698.46, 783.99 };

// Iwato - A Bb C# D Gb
#define SCALE_SIZE 15
float scale[SCALE_SIZE] = { 110.0, 116.54, 138.59, 146.83, 185.00, 220.00, 233.08, 277.18, 293.66, 369.99, 440.00, 466.16, 554.37, 587.33, 739.99 };




//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    
    // audio setup
    sampleRate 	= 44100;
    bufferSize	= 512;
    
    ofxMaxiSettings::setup(sampleRate, 2, bufferSize);
    
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    ofSoundStreamSetup(2,2,this, sampleRate, bufferSize, 4);
    
    // graphics setup
    
    ofBackground(0);
    ofSetColor(0);
    
    ofSetCircleResolution(100);
    
    float spacing = ofGetWidth() / person_count;
    float start_x = spacing / 2.0;
    for(int i=0 ; i <person_count ; i++) {
        people.push_back(ofPoint(start_x + i*spacing, ofGetHeight()/2.0));
    }
    
    radius_interval = (ofGetWidth()/2.0 - 50) / ring_count;
    
    init_composition();
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // calculate note associated with each person based on their angle from the vertical
    for(int i=0 ; i < person_count ; i++) {
        ofPoint pos = people[i];
        ofPoint delta = pos - ofPoint(ofGetWidth()/2.0, ofGetHeight()/2.0);
        float angle = atan2(delta.y, delta.x);
        angle = ofRadToDeg(angle);
        angle = ((int) angle + 360 + 90) % 360;
        int note_index = (int) ofMap(angle, 0, 360, 0, 15);
        person_notes[i] = note_index;
    }
    
    // calculate planet triggers
    for(int i=0 ; i < ring_count ; i++) {
        float inner = i*radius_interval;
        float outer = (i+1)*radius_interval;
        int trigger = 0;

        planet_people[i] = 0;
        bool found = false;
        for(int j=0 ; j < person_count ; j++) {
            float dist = distance_from_center(people[j]);
            if(trigger < 1) {
                trigger = (dist >= inner && dist < outer);
            }
            if(trigger == 1) {
                if(!found) {
                    p_notes[i][0] = person_notes[j];
                    found = true;
                }
                planet_people[i]++;
            }
        }
        planet_triggers[i] = trigger;
        
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    draw_rings();
    
    for(int i=0 ; i < people.size() ; i++) {
        draw_radial(people[i]);
    }
    
    for(int i=0 ; i < people.size() ; i++) {
        draw_person(people[i]);
    }
    
}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    
    for (int i = 0; i < bufferSize; i++){
        
        currentCount += (int) clock.phasor(tempo);
        
        wave = (mercury() + venus() + earth() + mars() + saturn() + jupiter() + neptune() + uranus()) / 8.0;

        output[i*nChannels    ] = wave;
        output[i*nChannels + 1] = wave;

    }
    
}

// remove - not needed
//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    for(int i = 0; i < bufferSize; i++){
        
        
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(focused) {
        people[people.size()-1] = ofPoint(x,y) - focus_offset;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    ofPoint mouse_pos = ofPoint(x,y);
    int focus = -1;
    // cycle backwards to pick up top elements first
    for(int i=people.size()-1 ; i >= 0 ; i--) {
        ofPoint delta = mouse_pos - people[i];
        float dist = sqrt(delta.x * delta.x + delta.y * delta.y);
        if(dist <= person_size) {
            focus = i;
            focus_offset = delta;
            break;
        }
    }
    if(focus >= 0) {
        ofPoint focused_person = people[focus];
        people.erase(people.begin() + focus);
        people.push_back(focused_person);
        focused = true;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    focused = false;
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

// utilities

void ofApp::draw_person(ofPoint pos) {
    
    ofPushStyle();
    
    ofPoint delta = pos - ofPoint(ofGetWidth()/2.0, ofGetHeight()/2.0);
    float dist = sqrt(delta.x * delta.x + delta.y * delta.y);
    if(dist > ring_count*radius_interval) {
        ofSetColor(255,0,0, 30);
    } else {
        ofSetColor(255,0,0, 255);
    }
    
    
    ofFill();
    ofDrawCircle(pos.x, pos.y, person_size);
    ofSetColor(0, 255);
    ofNoFill();
    ofDrawCircle(pos.x, pos.y, person_size);
    ofPopStyle();
    
}

void ofApp::draw_rings() {
    
    // base ring color on planet notes
    
    ofPushMatrix();
    ofPushStyle();

    ofTranslate(ofGetWidth()/2.0, ofGetHeight()/2.0);
    for(int i=ring_count-1 ; i >= 0 ; i--) {
        ofFill();
        ofSetColor(ofMap(p_notes[i][0], 0, SCALE_SIZE, 70, 185));
        ofDrawCircle(0,0,(i+1)*radius_interval);
        ofNoFill();
        ofSetColor(0);
        ofDrawCircle(0,0,(i+1)*radius_interval);
    }
    ofPopStyle();
    ofPopMatrix();
}

void ofApp::draw_radial(ofPoint pos) {
    ofPushMatrix();
    ofPushStyle();
    ofSetColor(0, 40);
    ofPoint delta = pos - ofPoint(ofGetWidth()/2.0, ofGetHeight()/2.0);
    float dist = sqrt(delta.x * delta.x + delta.y * delta.y);
    if(dist <= ring_count*radius_interval) {
        ofDrawLine(ofGetWidth()/2.0, ofGetHeight()/2.0, pos.x, pos.y);
    }
    ofPopStyle();
    ofPopMatrix();
}

float ofApp::distance_from_center(ofPoint pos) {
    ofPoint delta = pos - ofPoint(ofGetWidth()/2.0, ofGetHeight()/2.0);
    return sqrt(delta.x * delta.x + delta.y * delta.y);
}

// audio

void ofApp::init_composition() {
    
    p_notes.resize(PLANETS);
    for(int i=0 ; i<PLANETS ; i++) {
        p_notes[i].push_back(0);
    }
    
    // init envelopes
    e1.setAttack(ATTACK_DEF);
    e1.setDecay(DECAY_DEF);
    e1.setSustain(SUSTAIN_DEF);
    e1.setRelease(RELEASE_DEF);
    
    e2.setAttack(ATTACK_DEF);
    e2.setDecay(DECAY_DEF);
    e2.setSustain(SUSTAIN_DEF);
    e2.setRelease(RELEASE_DEF);
    
    e3.setAttack(ATTACK_DEF);
    e3.setDecay(DECAY_DEF);
    e3.setSustain(SUSTAIN_DEF);
    e3.setRelease(RELEASE_DEF);
    
    e4.setAttack(ATTACK_DEF);
    e4.setDecay(DECAY_DEF);
    e4.setSustain(SUSTAIN_DEF);
    e4.setRelease(RELEASE_DEF);
    
    e5.setAttack(ATTACK_DEF);
    e5.setDecay(DECAY_DEF);
    e5.setSustain(SUSTAIN_DEF);
    e5.setRelease(RELEASE_DEF);
    
    e6.setAttack(ATTACK_DEF);
    e6.setDecay(DECAY_DEF);
    e6.setSustain(SUSTAIN_DEF);
    e6.setRelease(RELEASE_DEF);
    
    e7.setAttack(ATTACK_DEF);
    e7.setDecay(DECAY_DEF);
    e7.setSustain(SUSTAIN_DEF);
    e7.setRelease(RELEASE_DEF);
    
    e8.setAttack(ATTACK_DEF);
    e8.setDecay(DECAY_DEF);
    e8.setSustain(SUSTAIN_DEF);
    e8.setRelease(RELEASE_DEF);
}

double ofApp::mercury() {
    o1_1_out = o1_1.sawn(scale[p_notes[0][0]]);
    o1_2_out = o1_2.sawn(scale[p_notes[0][0]] - detune1);
    s1 = f1.lores((o1_1_out + o1_2_out) * 0.5, f1_cutoff + lfo1.sinewave(lfo1_freq) * lfo1_amt, f1_res) * e1.adsr(1.0, planet_triggers[0]) * 0.6;
    if(planet_people[0]==1 || planet_people[0] == 0) {
        return s1;
    } else {
        double t1_out = t1.sinewave(t_base * planet_people[0]) * t_amt + (1-t_amt);
        return s1 * t1_out;
    }
    
}

double ofApp::venus() {
    double signal = car2.sinewave(scale[p_notes[1][0]] + (mod2.sinewave(mod2_freq + mod3.sinewave(mod3_freq)*mod3_index) * mod2_index)) * e2.adsr(1.0, planet_triggers[1]) * 0.3;
    if(planet_people[1]==1 || planet_people[1]==0) {
        return signal;
    } else {
        double t = t2.sinewave(t_base * planet_people[1]) * t_amt + (1-t_amt);
        return signal * t;
    }
}

double ofApp::earth() {
    double signal = o3.sinewave(glide.lopass(scale[p_notes[2][0]], glide_cutoff)) * e3.adsr(1.0, planet_triggers[2]);
    if(planet_people[2]==1 || planet_people[2]==0) {
        return signal;
    } else {
        double t = t1.sinewave(t_base * planet_people[2]) * t_amt + (1-t_amt);
        return signal * t;
    }
}

double ofApp::mars() {
    o4_1_out = o4_1.square(scale[p_notes[3][0]]);
    o4_2_out = o4_2.sawn(scale[p_notes[3][0]] - detune4);
    double signal = f4.hires((o4_1_out + o4_2_out) * 0.5, 1000.0, 10) * e4.adsr(1.0, planet_triggers[3]) * 0.1;
    if(planet_people[3]==1 || planet_people[3]==0) {
        return signal;
    } else {
        double t = t4.sinewave(t_base * planet_people[3]) * t_amt + (1-t_amt);
        return signal * t;
    }

}

double ofApp::saturn() {
    double signal = f5.lores(o5.triangle(scale[p_notes[4][0]]), 500, 20) * e5.adsr(1.0, planet_triggers[4]) * 0.4;
    if(planet_people[4]==1 || planet_people[4]==0) {
        return signal;
    } else {
        double t = t5.sinewave(t_base * planet_people[4]) * t_amt + (1-t_amt);
        return signal * t;
    }
}

double ofApp::jupiter() {
    o6_1_out = o6_1.sawn(scale[p_notes[5][0]]);
    o6_2_out = o6_2.sawn(scale[p_notes[5][0]] - detune5);
    double ct = f6_cutoff + lfo6.sinewave(0.3) * 1000;
    f6_1_out = f6_1.lores((o6_1_out + o6_2_out)*0.5, ct, f6_res*0.5);
    double signal = f6_2.hires(f6_1_out, ct, f6_res*0.5) * e6.adsr(1.0, planet_triggers[5]) * 0.2;
    if(planet_people[5]==1 || planet_people[5]==0) {
        return signal;
    } else {
        double t = t6.sinewave(t_base * planet_people[5]) * t_amt + (1-t_amt);
        return signal * t;
    }
}

double ofApp::neptune() {
    double signal = f7.lores(o7.square(glide7.lopass(scale[p_notes[6][0]],0.001) + lfo7.sinewave(4) * 0.8), 2000, 10) * e7.adsr(1.0, planet_triggers[6]) * 0.15;
    if(planet_people[6]==1 || planet_people[6]==0) {
        return signal;
    } else {
        double t = t7.sinewave(t_base * planet_people[6]) * t_amt + (1-t_amt);
        return signal * t;
    }
}

double ofApp::uranus() {
    double signal = o8.square(scale[p_notes[7][0]]) * e8.adsr(1.0, planet_triggers[7]) * 0.1;
    if(planet_people[7]==1 || planet_people[7]==0) {
        return signal;
    } else {
        double t = t8.sinewave(t_base * planet_people[7]) * t_amt + (1-t_amt);
        return signal * t;
    }
}


