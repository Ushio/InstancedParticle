#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include <vector>

class QuadRenderer {
public:
	struct Vertex {
		Vertex() {
		}
		Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) {
			position[0] = x;
			position[1] = y;
			position[2] = z;

			texcoord[0] = u;
			texcoord[1] = v;

			normal[0] = nx;
			normal[1] = ny;
			normal[2] = nz;
		}
		float position[3] = {};
		float texcoord[2] = {};
		float normal[3] = {};
	};
	struct Instance {
		float position[3] = {};
		float scale = 1.0f;
		float rotation[4] = { 0.0f , 0.0f , 0.0f , 1.0f };
		float saturation = 1.0f;
	};
	ofBufferObject _vertexBuffer;
	ofBufferObject _indexBuffer;
	ofBufferObject _instanceBuffer;
	ofShader _shader;
	int _instanceCap = 10;
	int _instanceCount = 0;
	GLuint _vao = 0;

	QuadRenderer() {
		glGenVertexArrays(1, &_vao);
		

		// 左上から時計回り
		std::vector<Vertex> vertices = {
			Vertex { -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
			Vertex { 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
			Vertex { 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			Vertex { -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		};
		_vertexBuffer.allocate(vertices, GL_STATIC_DRAW);

		std::vector<uint16_t> indices = {
			1, 0, 3,
			1, 3, 2
		};
		_indexBuffer.allocate<uint16_t>(indices, GL_STATIC_DRAW);
		_instanceBuffer.allocate(sizeof(Instance) * _instanceCap, GL_STREAM_DRAW);

		_shader.load("instanced_vert.glsl", "instanced_frag.glsl");
	}
	~QuadRenderer() {
		glDeleteVertexArrays(1, &_vao);
	}

	void reserve(int instanceCount) {
		if (_instanceCap <= instanceCount) {
			_instanceCap = instanceCount;
			_instanceBuffer.allocate(sizeof(Instance) * _instanceCap, GL_STREAM_DRAW);
		}
	}
	Instance *map(int instanceCount) {
		reserve(instanceCount);
		_instanceCount = instanceCount;
		return (Instance *)_instanceBuffer.map(GL_WRITE_ONLY);
	}
	void unmap() {
		_instanceBuffer.unmap();
	}

	void draw(ofTexture &texture) {
		if (_instanceCount <= 0) {
			return;
		}
		glBindVertexArray(_vao);

		auto modelview = ofGetCurrentMatrix(OF_MATRIX_MODELVIEW);
		auto proj = ofGetCurrentMatrix(OF_MATRIX_PROJECTION);

		int a_position = _shader.getAttributeLocation("a_position");
		int a_texcoord = _shader.getAttributeLocation("a_texcoord");
		int a_normal = _shader.getAttributeLocation("a_normal");
		int a_location = _shader.getAttributeLocation("a_location");
		int a_scale = _shader.getAttributeLocation("a_scale");
		int a_rotation = _shader.getAttributeLocation("a_rotation");
		int a_saturation = _shader.getAttributeLocation("a_saturation");

		_vertexBuffer.bind(GL_ARRAY_BUFFER);
		glEnableVertexAttribArray(a_position);
		glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0 + offsetof(Vertex, position));
		glEnableVertexAttribArray(a_texcoord);
		glVertexAttribPointer(a_texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0 + offsetof(Vertex, texcoord));
		glEnableVertexAttribArray(a_normal);
		glVertexAttribPointer(a_normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)0 + offsetof(Vertex, normal));
		_vertexBuffer.unbind(GL_ARRAY_BUFFER);

		_instanceBuffer.bind(GL_ARRAY_BUFFER);
		glEnableVertexAttribArray(a_location);
		glVertexAttribPointer(a_location, 3, GL_FLOAT, GL_FALSE, sizeof(Instance), (char *)0 + offsetof(Instance, position));
		glVertexAttribDivisor(a_location, 1);

		glEnableVertexAttribArray(a_scale);
		glVertexAttribPointer(a_scale, 1, GL_FLOAT, GL_FALSE, sizeof(Instance), (char *)0 + offsetof(Instance, scale));
		glVertexAttribDivisor(a_scale, 1);

		glEnableVertexAttribArray(a_rotation);
		glVertexAttribPointer(a_rotation, 4, GL_FLOAT, GL_FALSE, sizeof(Instance), (char *)0 + offsetof(Instance, rotation));
		glVertexAttribDivisor(a_rotation, 1);

		glEnableVertexAttribArray(a_saturation);
		glVertexAttribPointer(a_saturation, 1, GL_FLOAT, GL_FALSE, sizeof(Instance), (char *)0 + offsetof(Instance, saturation));
		glVertexAttribDivisor(a_saturation, 1);

		_instanceBuffer.unbind(GL_ARRAY_BUFFER);

		_indexBuffer.bind(GL_ELEMENT_ARRAY_BUFFER);
		_shader.begin();
		_shader.setUniformMatrix4f("u_modelview", modelview);
		_shader.setUniformMatrix4f("u_proj", proj);
		_shader.setUniformTexture("u_texture", texture, 1);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, _instanceCount);
		_shader.end();
		_indexBuffer.unbind(GL_ELEMENT_ARRAY_BUFFER);

		glDisableVertexAttribArray(a_position);
		glDisableVertexAttribArray(a_texcoord);
		glDisableVertexAttribArray(a_normal);
		glDisableVertexAttribArray(a_location);
		glDisableVertexAttribArray(a_scale);
		glDisableVertexAttribArray(a_rotation);
		glDisableVertexAttribArray(a_saturation);

		glBindVertexArray(0);
	}
};

struct Xor {
	Xor() {

	}
	Xor(uint32_t seed) {
		_y = std::max(seed, 1u);
	}

	// 0 <= x <= 0x7FFFFFFF
	uint32_t generate() {
		_y = _y ^ (_y << 13); _y = _y ^ (_y >> 17);
		uint32_t value = _y = _y ^ (_y << 5); // 1 ~ 0xFFFFFFFF(4294967295
		return value >> 1;
	}
	// 0.0 <= x < 1.0
	double uniform() {
		return double(generate()) / double(0x80000000);
	}
	double uniform(double a, double b) {
		return a + (b - a) * uniform();
	}
public:
	uint32_t _y = 2463534242;
};

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
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
		
	QuadRenderer *_renderer = nullptr;
	ofEasyCam _camera;
	ofImage _image;

	ofxImGui _imgui;
	int _instanceCount = 100000;
};
