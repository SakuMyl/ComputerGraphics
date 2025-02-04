#define _CRT_SECURE_NO_WARNINGS

#include "App.hpp"

#include "utility.hpp"
#include "base/Main.hpp"
#include "gpu/GLContext.hpp"
#include "gpu/Buffer.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <type_traits>

using namespace FW;
using namespace std;

// Anonymous namespace. This is a C++ way to define things which
// do not need to be visible outside this file.
namespace {

enum VertexShaderAttributeLocations {
	ATTRIB_POSITION = 0,
	ATTRIB_NORMAL = 1,
	ATTRIB_COLOR = 2
};

const Vertex reference_plane_data[] = {
	{ Vec3f(-1, -1, -1), Vec3f(0, 1, 0) },
	{ Vec3f( 1, -1, -1), Vec3f(0, 1, 0) },
	{ Vec3f( 1, -1,  1), Vec3f(0, 1, 0) },
	{ Vec3f(-1, -1, -1), Vec3f(0, 1, 0) },
	{ Vec3f( 1, -1,  1), Vec3f(0, 1, 0) },
	{ Vec3f(-1, -1,  1), Vec3f(0, 1, 0) }
};

vector<Vertex> loadExampleModel() {
	static const Vertex example_data[] = {
		{ Vec3f( 0.0f,  0.5f, 0), Vec3f(0.0f, 0.0f, -1.0f) },
		{ Vec3f(-0.5f, -0.5f, 0), Vec3f(0.0f, 0.0f, -1.0f) },
		{ Vec3f( 0.5f, -0.5f, 0), Vec3f(0.0f, 0.0f, -1.0f) }
	};
	vector<Vertex> vertices;
	for (auto v : example_data)
		vertices.push_back(v);
	return vertices;
}

vector<Vertex> unpackIndexedData(
	const vector<Vec3f>& positions,
	const vector<Vec3f>& normals,
	const vector<array<unsigned, 6>>& faces)
{
	vector<Vertex> vertices;

	// This is a 'range-for' loop which goes through all objects in the container 'faces'.
	// '&' gives us a reference to the object inside the container; if we omitted '&',
	// 'f' would be a copy of the object instead.
	// The compiler already knows the type of objects inside the container, so we can
	// just write 'auto' instead of having to spell out 'array<unsigned, 6>'.
	for (auto& f : faces) {

		// YOUR CODE HERE (R3)
		// Unpack the indexed data into a vertex array. For every face, you have to
		// create three vertices and add them to the vector 'vertices'.

		// f[0] is the index of the position of the first vertex
		// f[1] is the index of the normal of the first vertex
		// f[2] is the index of the position of the second vertex
		// ...
		vertices.push_back({ positions[f[0]], normals[f[1]] });
		vertices.push_back({ positions[f[2]], normals[f[3]] });
		vertices.push_back({ positions[f[4]], normals[f[5]] });
	}

	return vertices;
};

// This is for testing your unpackIndexedData implementation.
// You should get a tetrahedron like in example.exe.
vector<Vertex> loadIndexedDataModel() {
	static const Vec3f point_data[] = {
		Vec3f(0.0f, 0.407f, 0.0f),
		Vec3f(0.0f, -0.3f, -0.5f),
		Vec3f(0.433f, -0.3f, 0.25f),
		Vec3f(-0.433f, -0.3f, 0.25f),
	};
	static const Vec3f normal_data[] = {
		Vec3f(0.8165f, 0.3334f, -0.4714f),
		Vec3f(0.0f, 0.3334f, 0.9428f),
		Vec3f(-0.8165f, 0.3334f, -0.4714f),
		Vec3f(0.0f, -1.0f, 0.0f)
	};
	static const unsigned face_data[][6] = {
		{0, 0, 1, 0, 2, 0},
		{0, 2, 3, 2, 1, 2},
		{0, 1, 2, 1, 3, 1},
		{1, 3, 3, 3, 2, 3}
	};
	vector<Vec3f> points(point_data, point_data + SIZEOF_ARRAY(point_data));
	vector<Vec3f> normals(normal_data, normal_data + SIZEOF_ARRAY(normal_data));
	vector<array<unsigned, 6>> faces;
	for (auto arr : face_data) {
		array<unsigned, 6> f;
		copy(arr, arr+6, f.begin());
		faces.push_back(f);
	}
	return unpackIndexedData(points, normals, faces);
}

// Generate an upright cone with tip at (0, 0, 0), a radius of 0.25 and a height of 1.0.
// You can leave the base of the cone open, like it is in example.exe.
vector<Vertex> loadUserGeneratedModel() {
	static const float radius = 0.25f;
	static const float height = 1.0f;
	static const unsigned faces = 40;
	static const float angle_increment = 2 * FW_PI / faces;

	// Empty array of Vertex structs; every three vertices = one triangle
	vector<Vertex> vertices;
	
	Vertex v0, v1, v2;

	// Generate one face at a time
	for(auto i = 0u; i < faces; ++i)	{
		// YOUR CODE HERE (R2)
		v0.position = Vec3f(0, 0, 0);
		v1.position = Vec3f(FW::sin(i * angle_increment) * radius, -height, FW::cos(i * angle_increment) * radius);
		v2.position = Vec3f(FW::sin((i + 1) * angle_increment) * radius, -height, FW::cos((i + 1) * angle_increment) * radius);

		// Calculate the normal of the face from the positions and use it for all vertices.
		v0.normal = v1.normal = v2.normal = (v1.position - v0.position).cross(v2.position - v1.position).normalized();
		vertices.push_back(v0); vertices.push_back(v1); vertices.push_back(v2);
	}
	return vertices;
}

}

App::App(void)
:   common_ctrl_			(CommonControls::Feature_Default & ~CommonControls::Feature_RepaintOnF5),
	current_model_			(MODEL_EXAMPLE),
	model_changed_			(true),
	shading_toggle_			(false),
	shading_mode_changed_	(false),
	cam_rot_around_y_	 	(0.0f),
	cam_rot_around_x_		(0.0f),
	mouseDragging_			(false),
	lastMousePos_			(Vec2i(0, 0)),
	translation_			(Vec3f(0, 0, 0)),
	object_y_rotation_      (0.0f),
	object_x_scale_			(1.0f)
{
	static_assert(is_standard_layout<Vertex>::value, "struct Vertex must be standard layout to use offsetof");
	initRendering();
	
	common_ctrl_.showFPS(true);
	common_ctrl_.addToggle((S32*)&current_model_, MODEL_EXAMPLE,			FW_KEY_1, "Triangle (1)",				&model_changed_);
	common_ctrl_.addToggle((S32*)&current_model_, MODEL_USER_GENERATED,		FW_KEY_2, "Generated cone (2)",			&model_changed_);
	common_ctrl_.addToggle((S32*)&current_model_, MODEL_FROM_INDEXED_DATA,	FW_KEY_3, "Unpacked tetrahedron (3)",	&model_changed_);
	common_ctrl_.addToggle((S32*)&current_model_, MODEL_FROM_FILE,			FW_KEY_4, "Model loaded from file (4)",	&model_changed_);
	common_ctrl_.addSeparator();
	common_ctrl_.addToggle(&shading_toggle_,								FW_KEY_T, "Toggle shading mode (T)",	&shading_mode_changed_);

	window_.setTitle("Assignment 1");

	window_.addListener(this);
	window_.addListener(&common_ctrl_);

	window_.setSize( Vec2i(800, 800) );
}

bool App::handleEvent(const Window::Event& ev) {
	if (model_changed_)	{
		model_changed_ = false;
		switch (current_model_)
		{
		case MODEL_EXAMPLE:
			vertices_ = loadExampleModel();
			break;
		case MODEL_FROM_INDEXED_DATA:
			vertices_ = loadIndexedDataModel();
			break;
		case MODEL_USER_GENERATED:
			vertices_ = loadUserGeneratedModel();
			break;
		case MODEL_FROM_FILE:
			{
				auto filename = window_.showFileLoadDialog("Load new mesh");
				if (filename.getLength()) {
					vertices_ = loadObjFileModel(filename.getPtr());
				} else {
					current_model_ = MODEL_EXAMPLE;
					model_changed_ = true;
				}
			}
			break;
		default:
			assert(false && "invalid model type");
		}
		// Load the vertex buffer to GPU.
		glBindBuffer(GL_ARRAY_BUFFER, gl_.dynamic_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_.size(), vertices_.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	if (shading_mode_changed_) {
		common_ctrl_.message(shading_toggle_ ?
			"Directional light shading using vertex normals; direction to light (0.5, 0.5, -0.6)" :
			"High visibility shading, color from vertex ID");
		shading_mode_changed_ = false;
	}


	if (ev.type == Window::EventType_KeyDown) {
		// YOUR CODE HERE (R1)
		// React to user input and move the model.
		// Look in framework/gui/Keys.hpp for more key codes.
		// Visual Studio tip: you can right-click an identifier like FW_KEY_HOME
		// and "Go to definition" to jump directly to where the identifier is defined.
		if (ev.key == FW_KEY_DOWN)
			translation_.y -= 0.05;
		else if (ev.key == FW_KEY_UP)
			translation_.y += 0.05;
		else if (ev.key == FW_KEY_LEFT)
			translation_.x -= 0.05;
		else if (ev.key == FW_KEY_RIGHT)
			translation_.x += 0.05;
		else if (ev.key == FW_KEY_PAGE_DOWN)
			translation_.z -= 0.05;
		else if (ev.key == FW_KEY_PAGE_UP)
			translation_.z += 0.05;
		else if (ev.key == FW_KEY_A)
			object_y_rotation_ += 0.05;
		else if (ev.key == FW_KEY_D)
			object_y_rotation_ -= 0.05;
		else if (ev.key == FW_KEY_W)
			object_x_scale_ += 0.05;
		else if (ev.key == FW_KEY_S)
			object_x_scale_ -= 0.05;
		else if (ev.key == FW_KEY_MOUSE_LEFT) {
			mouseDragging_ = true;
			lastMousePos_ = ev.mousePos;
		}
	}
	
	if (ev.type == Window::EventType_KeyUp) {
		if (ev.key == FW_KEY_MOUSE_LEFT)
			mouseDragging_ = false;
	}

	if (ev.type == Window::EventType_Mouse) {
		// EXTRA: you can put your mouse controls here.
		if (ev.mouseDragging) {
			Vec2i newMousePos = ev.mousePos;
			Vec2i posChange = newMousePos - lastMousePos_;
			float newXRot = cam_rot_around_x_ + posChange.y * 0.005;
			cam_rot_around_x_ = std::min(std::max(newXRot, -FW_PI / 2), FW_PI / 2);
			cam_rot_around_y_ += posChange.x * 0.005;
			lastMousePos_ = newMousePos;
		}
		// Event::mouseDelta gives the distance the mouse has moved.
		// Event::mouseDragging tells whether some mouse buttons are currently down.
		// If you want to know which ones, you have to keep track of the button down/up events
		// (e.g. FW_KEY_MOUSE_LEFT).
	}

	if (ev.type == Window::EventType_Close) {
		window_.showModalMessage("Exiting...");
		delete this;
		return true;
	}


	window_.setVisible(true);
	if (ev.type == Window::EventType_Paint)
		render();

	window_.repaint();

	return false;
}

void App::initRendering() {
	// Ask the Nvidia framework for the GLContext object associated with the window.
	// As a side effect, this initializes the OpenGL context and lets us call GL functions.
	auto ctx = window_.getGL();
	
	// Create vertex attribute objects and buffers for vertex data.
	glGenVertexArrays(1, &gl_.static_vao);
	glGenVertexArrays(1, &gl_.dynamic_vao);
	glGenBuffers(1, &gl_.static_vertex_buffer);
	glGenBuffers(1, &gl_.dynamic_vertex_buffer);
	
	// Set up vertex attribute object for static data.
	glBindVertexArray(gl_.static_vao);
	glBindBuffer(GL_ARRAY_BUFFER, gl_.static_vertex_buffer);
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal));

	// Load the static data to the GPU; needs to be done only once.
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * SIZEOF_ARRAY(reference_plane_data), reference_plane_data, GL_STATIC_DRAW);
	
	// Set up vertex attribute object for dynamic data. We'll load the actual data later, whenever the model changes.
	glBindVertexArray(gl_.dynamic_vao);
	glBindBuffer(GL_ARRAY_BUFFER, gl_.dynamic_vertex_buffer);
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	// Compile and link the shader program.
	// We use the Nvidia FW for creating the program; it's not difficult to do manually,
	// but takes about half a page of OpenGL boilerplate code.
	// This shader program will be used to draw everything except the user interface.
	// It consists of one vertex shader and one fragment shader.
	auto shader_program = new GLContext::Program(
		"#version 330\n"
		FW_GL_SHADER_SOURCE(
		layout(location = 0) in vec4 aPosition;
		layout(location = 1) in vec3 aNormal;
		
		out vec4 vColor;
		
		uniform mat4 uNormalTransform;
		uniform mat4 uModelToWorld;
		uniform mat4 uWorldToClip;
		uniform float uShading;
		
		const vec3 distinctColors[6] = vec3[6](
			vec3(0, 0, 1), vec3(0, 1, 0), vec3(0, 1, 1),
			vec3(1, 0, 0), vec3(1, 0, 1), vec3(1, 1, 0));
		const vec3 directionToLight = normalize(vec3(0.5, 0.5, -0.6));
		
		void main()
		{
<<<<<<< HEAD
			// EXTRA: oops, someone forgot to transform normals here...
			mat4 transformNormalMat = transpose(inverse(uModelToWorld));
			float clampedCosine = clamp(dot(mat3(transformNormalMat) * aNormal, directionToLight), 0.0, 1.0);
=======
			// EXTRA: oops, someone forgot to transform normals here;
			vec3 normalTransformed = mat3(uNormalTransform) * aNormal;
			float clampedCosine = clamp(dot(normalize(normalTransformed), directionToLight), 0.0, 1.0);
>>>>>>> aa724520397a9ae3d5cea812807f836da582633d
			vec3 litColor = vec3(clampedCosine);
			vec3 generatedColor = distinctColors[gl_VertexID % 6];
			// gl_Position is a built-in output variable that marks the final position
			// of the vertex in clip space. Vertex shaders must write in it.
			gl_Position = uWorldToClip * uModelToWorld * aPosition;
			vColor = vec4(mix(generatedColor, -litColor, uShading), 1);
		}
		),
		"#version 330\n"
		FW_GL_SHADER_SOURCE(
		in vec4 vColor;
		out vec4 fColor;
		void main()
		{
			fColor = vColor;
		}
		));
	// Tell the FW about the program so it gets properly destroyed at exit.
	ctx->setProgram("shaders", shader_program);

	// Get the IDs of the shader program and its uniform input locations from OpenGL.
	gl_.shader_program = shader_program->getHandle();
	gl_.world_to_clip_uniform = glGetUniformLocation(gl_.shader_program, "uWorldToClip");
	gl_.model_to_world_uniform = glGetUniformLocation(gl_.shader_program, "uModelToWorld");
	gl_.normal_transform_uniform = glGetUniformLocation(gl_.shader_program, "uNormalTransform");
	gl_.shading_toggle_uniform = glGetUniformLocation(gl_.shader_program, "uShading");
}

void App::render() {
	// Clear screen.
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);
	
	// Set up a matrix to transform from world space to clip space.
	// Clip space is a [-1, 1]^3 space where OpenGL expects things to be
	// when it starts drawing them.

	// Our camera is aimed at origin, and orbits around origin at fixed distance.
	static const float camera_distance = 2.1f;	
	Mat4f C;
	Mat3f yRot = Mat3f::rotation(Vec3f(0, 1, 0), -cam_rot_around_y_);
	Mat3f xRot = Mat3f::rotation(Vec3f(1, 0, 0), -cam_rot_around_x_);
	Mat3f rot = xRot * yRot;
	C.setCol(0, Vec4f(rot.getCol(0), 0));
	C.setCol(1, Vec4f(rot.getCol(1), 0));
	C.setCol(2, Vec4f(rot.getCol(2), 0));
	C.setCol(3, Vec4f(0, 0, camera_distance, 1));

	//Rotate the camera around the object
	Mat4f translateCamera = Mat4f();
	translateCamera.setCol(3, Vec4f(translation_.x, translation_.y, translation_.z, 1));
	
	// Simple perspective.
	static const float fnear = 0.1f, ffar = 4.0f;
	Mat4f P;
	P.setCol(0, Vec4f(1, 0, 0, 0));
	P.setCol(1, Vec4f(0, 1, 0, 0));
	P.setCol(2, Vec4f(0, 0, (ffar+fnear)/(ffar-fnear), 1));
	P.setCol(3, Vec4f(0, 0, -2*ffar*fnear/(ffar-fnear), 0));

	Mat4f world_to_clip = P * C * invert(translateCamera);
	
	// Set active shader program.
	glUseProgram(gl_.shader_program);
	glUniform1f(gl_.shading_toggle_uniform, shading_toggle_ ? 1.0f : 0.0f);
	glUniformMatrix4fv(gl_.world_to_clip_uniform, 1, GL_FALSE, world_to_clip.getPtr());

	// Draw the reference plane. It is already in world coordinates.
	auto identity = Mat4f();
	glUniformMatrix4fv(gl_.model_to_world_uniform, 1, GL_FALSE, identity.getPtr());
	glBindVertexArray(gl_.static_vao);
	glDrawArrays(GL_TRIANGLES, 0, SIZEOF_ARRAY(reference_plane_data));
	
	// YOUR CODE HERE (R1)
	// Set the model space -> world space transform to translate the model according to user input.
	
	Mat4f translation = Mat4f().translate(translation_);

	Mat4f rotation = Mat4f();
	rotation.setRow(0, Vec4f(FW::cos(object_y_rotation_), 0, FW::sin(object_y_rotation_), 0));
	rotation.setRow(1, Vec4f(0, 1, 0, 0));
	rotation.setRow(2, Vec4f(-FW::sin(object_y_rotation_), 0, FW::cos(object_y_rotation_), 0));
	rotation.setRow(3, Vec4f(0, 0, 0, 1));

	Mat4f scale = Mat4f().scale(Vec3f(object_x_scale_, 1, 1));
	Mat4f modelToWorld = translation * rotation * scale;
	Mat4f normalTransform = transpose(invert(modelToWorld));
	// Draw the model with your model-to-world transformation.
	glUniformMatrix4fv(gl_.normal_transform_uniform, 1, GL_FALSE, normalTransform.getPtr());
	glUniformMatrix4fv(gl_.model_to_world_uniform, 1, GL_FALSE, modelToWorld.getPtr());
	glBindVertexArray(gl_.dynamic_vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices_.size());

	// Undo our bindings.
	glBindVertexArray(0);
	glUseProgram(0);
	
	// Check for OpenGL errors.
	GLContext::checkErrors();
	
	// Show status messages. You may find it useful to show some debug information in a message.
	common_ctrl_.message(sprintf("Use Home/End to rotate camera."), "instructions");
	common_ctrl_.message(sprintf("Camera is at (%.2f %.2f %.2f) looking towards origin.",
		-FW::sin(cam_rot_around_y_) * camera_distance, 0.0f,
		-FW::cos(cam_rot_around_y_) * camera_distance), "camerainfo");
}

vector<Vertex> App::loadObjFileModel(string filename) {
	window_.showModalMessage(sprintf("Loading mesh from '%s'...", filename.c_str()));

	vector<Vec3f> positions, normals;
	vector<array<unsigned, 6>> faces;

	// Open input file stream for reading.
	ifstream input(filename, ios::in);

	// Read the file line by line.
	string line;
	while(getline(input, line)) {
		// Replace any '/' characters with spaces ' ' so that all of the
		// values we wish to read are separated with whitespace.
		for (auto& c : line)
			if (c == '/')
				c = ' ';
			
		// Temporary objects to read data into.
		array<unsigned, 6>  f; // Face index array
		Vec3f               v;
		string              s;

		// Create a stream from the string to pick out one value at a time.
		istringstream        iss(line);

		// Read the first token from the line into string 's'.
		// It identifies the type of object (vertex or normal or ...)
		iss >> s;

		if (s == "v") { // vertex position
			iss >> v.x >> v.y >> v.z;
			positions.push_back(v);
		} else if (s == "vn") { // normal
			iss >> v.x >> v.y >> v.z;
			normals.push_back(v);
		} else if (s == "f") { // face
			// YOUR CODE HERE (R4)
			unsigned sink; // Temporary variable for reading the unused texture indices.

			iss >> f[0] >> sink >> f[1] >> f[2] >> sink >> f[3] >> f[4] >> sink >> f[5];
			for (auto& v : f) v--;
			faces.push_back(f);
		}
	}
	common_ctrl_.message(("Loaded mesh from " + filename).c_str());
	return unpackIndexedData(positions, normals, faces);
}

void FW::init(void) {
	new App;
}
