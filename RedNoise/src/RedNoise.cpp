#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#define WIDTH 320
#define HEIGHT 240
#define ALPHA 255 

std::vector<float> interpolateSingleFloats(float from, float to, uint32_t numberOfValues){
	std::vector<float> result;
	for (size_t i = 0; i < numberOfValues; i++){
		float t = float(i) / float(numberOfValues - 1);
		float val = from + t*(to - from);
		result.push_back(val);
	}
	return result;
}

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, uint32_t numberOfValues){
	std::vector<glm::vec3> result;
	for (size_t i = 0; i < numberOfValues; i++){
		float t = float(i) / float(numberOfValues - 1);
		glm::vec3 val = from + t*(to - from);
		result.push_back(val);
	}
	return result;
}

uint32_t packRGBA(float red, float green, float blue, uint8_t alpha){
	return (alpha << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
}

void draw1DGreyscale(DrawingWindow &window){
	window.clearPixels();

	std::vector<float> interpolations = interpolateSingleFloats(255.0f, 0.0f, window.width);
	for(size_t y = 0; y < window.height; y++){
		for(size_t x = 0; x < window.width; x++){
			float red = interpolations[x];
			float blue = interpolations[x];
			float green = interpolations[x];

			uint32_t colour = packRGBA(red, blue, green, ALPHA);
			window.setPixelColour(x, y, colour);
		}
	}
}

void draw2DColour(DrawingWindow &window){
	window.clearPixels();

	glm::vec3 topLeft(255, 0, 0);        // red 
	glm::vec3 topRight(0, 0, 255);       // blue 
	glm::vec3 bottomRight(0, 255, 0);    // green 
	glm::vec3 bottomLeft(255, 255, 0);   // yellow

	std::vector<glm::vec3> interpolationLeftMost = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
	std::vector<glm::vec3> interpolationRightMost = interpolateThreeElementValues(topRight, bottomRight, window.height);

	for (size_t y = 0; y < window.height; y++){
		std::vector<glm::vec3> interpolation = interpolateThreeElementValues(interpolationLeftMost[y], interpolationRightMost[y], window.width);
		for (size_t x = 0; x < window.width; x++){
			float red = interpolation[x].r, green = interpolation[x].g, blue = interpolation[x].b;
			uint32_t colour = packRGBA(red, green, blue, ALPHA);
			window.setPixelColour(x, y, colour);
		}
	}
}

bool insideTriangle(float u, float v, float w){
	return 	(u >= 0 && u <= 1) &&
       		(v >= 0 && v <= 1) &&
       		(w >= 0 && w <= 1);

}

void drawBarycentricTriangle(DrawingWindow &window){
	window.clearPixels();

	glm::vec2 v0(0, window.height-1);
	glm::vec2 v1(int((window.width-1) / 2), 0);
	glm::vec2 v2(window.width-1, window.height-1);

	glm::vec3 c0(255, 0, 0); // Red
	glm::vec3 c1(0, 255, 0); // Green
	glm::vec3 c2(0, 0, 255); // Blue

	for (size_t y = 0; y < window.height; y++){
		for (size_t x = 0; x < window.width; x++){
			glm::vec2 r(x, y);
			glm::vec3 bc = convertToBarycentricCoordinates(v0, v1, v2, r);

			float u = bc.r, v = bc.g, w = bc.b;
			glm::vec3 colourVec = u*c1 + v*c2 + w*c0;
			uint32_t colour = packRGBA(colourVec.r, colourVec.g, colourVec.b, ALPHA);
			if (insideTriangle(u, v, w)){
				window.setPixelColour(x, y, colour);
			}
		}
	}

}

void testInterpolateSingleFloat(){
	std::vector<float> result;
	result = interpolateSingleFloats(2.2, 8.5, 7);
	for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
	std::cout << std::endl;
}

void testInterpolateThreeElementValues(){
	std::vector<glm::vec3> result;
	glm::vec3 from(1.0, 4.0, 9.2);
	glm::vec3 to(4.0, 1.0, 9.8);
	result = interpolateThreeElementValues(from, to, 4);
	for(size_t i=0; i<result.size(); i++){
		std::cout << "(" << result[i][0] << "," << result[i][1] << "," << result[i][2] << ")" << std::endl;
	}
}

void draw(DrawingWindow &window) {
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = rand() % 256;
			float green = 0.0;
			float blue = 0.0;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	testInterpolateThreeElementValues();
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		drawBarycentricTriangle(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
