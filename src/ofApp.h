#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGui.h"

#define ATTACK_DEF 6000.0
#define DECAY_DEF 1000.0
#define SUSTAIN_DEF 0.5
#define RELEASE_DEF 1000.0

#define PLANETS 8
#define RAD_OFFSET 3
#define PEOPLE 4
#define PERSON_SIZE 15

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    // Audio output and input methods
    
        void audioOut(float * output, int bufferSize, int nChannels);
        void audioIn(float * input, int bufferSize, int nChannels);
     
     
    
    int		bufferSize; /* buffer size */
    int		sampleRate;
    
    ofxPanel gui;
    ofParameterGroup params;
    
    // composition
    
    double wave;
    
    int currentCount;
    maxiOsc clock;
    double tempo {0.5};
    
    
    // mercury (subtractive saw detuned oscillators lo-pass filter)
    maxiOsc o1_1, o1_2;
    maxiFilter f1;
    maxiOsc lfo1;
    double o1_1_out, o1_2_out, s1, t1_out;
    ofParameter<double> f1_cutoff {1000};
    ofParameter<double> f1_res {15};
    ofParameter<double> detune1 {1};
    ofParameter<double> lfo1_freq {0.2};
    ofParameter<double> lfo1_amt {200};
    
    
    // venus (fm)
    maxiOsc car2, mod2, mod3;;
    ofParameter<double> mod2_freq {1066};
    ofParameter<double> mod2_index {151};
    ofParameter<double> mod3_freq {1186};
    ofParameter<double> mod3_index {2751};
    
    // earth (glide sin)
    maxiOsc o3;
    maxiFilter glide;
    maxiOsc trem;
    
    ofParameter<double> trem_freq {0.5};
    ofParameter<double> trem_amt {0.4};
    ofParameter<double> glide_cutoff {0.0007};
    
    
    // mars
    // hires square
    maxiOsc o4_1, o4_2;
    maxiFilter f4;
    ofParameter<double> detune4 {0.5};
    ofParameter<double> f4_cutoff {6000};
    ofParameter<double> f4_res {20};
    
    double o4_1_out, o4_2_out;
    
    // saturn
    maxiOsc o5;
    maxiFilter f5;
    
    // jupiter
    maxiOsc o6_1, o6_2;
    maxiFilter f6_1, f6_2;
    maxiOsc lfo6;
    double o6_1_out, o6_2_out, f6_1_out;
    double detune5 {2};
    double f6_cutoff {2300};
    double f6_res {10};
    
    // netpune
    maxiOsc o7;
    maxiFilter f7;
    maxiFilter glide7;
    maxiOsc lfo7;
    
    // uranus
    maxiOsc o8;
    
    maxiEnv e1, e2, e3, e4, e5, e6, e7, e8;
    
    // tremolos
    maxiOsc t1, t2, t3, t4, t5, t6, t7, t8;
    double t1_freq {0.5}, t2_freq {0.5}, t3_freq {0.5}, t4_freq {0.5}, t5_freq {0.5}, t6_freq {0.5}, t7_freq {0.5}, t8_freq {0.5};
    double t_amt {0.4};
    double t_base {0.3};
    
    int planet_triggers[PLANETS];
    float planet_people[PLANETS];
    vector<vector<int>> p_notes;
    
    double mercury();
    double venus();
    double earth();
    double mars();
    double saturn();
    double jupiter();
    double neptune();
    double uranus();
    
    void init_composition();
    
    // drawing
    
    void draw_person(ofPoint pos);
    void draw_radial(ofPoint pos);
    void draw_rings();
    
    vector<ofPoint> people;
    float distance_from_center(ofPoint pos);
    
    const float person_size {PERSON_SIZE};
    const int person_count {PEOPLE};
    const int ring_count {PLANETS};
    float radius_interval;
    int person_notes[PEOPLE];
    
    bool focused {false};
    ofPoint focus_offset;
};
