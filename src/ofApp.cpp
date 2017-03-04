#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	_renderer = new QuadRenderer();
	_camera.setDistance(5.0f);
	_camera.setNearClip(0.1f);
	_camera.setFarClip(500.0f);

	ofDisableArbTex();
	_image.load("lena.jpg");
	ofEnableArbTex();

	_imgui.setup();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);
	ofEnableDepthTest();
	
	_camera.begin();
	ofDrawAxis(5);

	float e = ofGetElapsedTimef();

	auto random_axis = [](Xor &xorshift) {
		ofVec3f axis;
		do {
			axis.x = xorshift.uniform(-1.0f, 1.0f);
			axis.y = xorshift.uniform(-1.0f, 1.0f);
			axis.z = xorshift.uniform(-1.0f, 1.0f);
		} while (axis.lengthSquared() < 0.001f);
		axis.normalize();
		return axis;
	};

	Xor xorshift;
	int N = _instanceCount;

	// これの引数は数のヒント
	_renderer->reserve(N);

	// これの引数が実際に描画される数
	QuadRenderer::Instance *instance = _renderer->map(N);
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < 3; ++j) {
			instance[i].position[j] = xorshift.uniform(-30, 30);
		}

		instance[i].scale = xorshift.uniform(0.1f, 0.5f);

		// ベース姿勢
		auto axis_base = random_axis(xorshift);
		auto axis_rotate = random_axis(xorshift);
		ofQuaternion q_base(xorshift.uniform(0.0f, 180.0f), axis_base);
		ofQuaternion q_rotate(e * 90.0f, axis_rotate);
		ofQuaternion q = q_base * q_rotate;
		for (int j = 0; j < 4; ++j) {
			instance[i].rotation[j] = q[j];
		}
		instance[i].saturation = xorshift.uniform(0.1f, 2.0f);
	}
	_renderer->unmap();
	_renderer->draw(_image.getTexture());

	_camera.end();


	_imgui.begin();
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ofVec4f(0.0f, 0.2f, 0.2f, 1.0f));
	ImGui::SetNextWindowPos(ofVec2f(10, 30), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ofVec2f(500, ofGetHeight() * 0.8), ImGuiSetCond_Once);

	ImGui::Begin("Config Panel");
	ImGui::Text("fps: %.2f", ofGetFrameRate());
	ImGui::SliderInt("Instance Count", &_instanceCount, 0, 300000);
	ImGui::End();
	ImGui::PopStyleColor();

	_imgui.end();
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
