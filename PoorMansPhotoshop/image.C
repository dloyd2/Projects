#ifndef IMAGE_METHODS 
#define IMAGE_METHODS 
#include <source.h>
#include <image.h>
#include <string.h>
#include <stdio.h>
Image::Image(void){
	w        = 0;
	h        = 0;
	pixels   = NULL;
	upPipe = NULL;
}
		
Image::Image(int width, int height){
	w        = width;
	h        = height;
	pixels   = new Pixel[w*h];
}
		
Image::Image(Image &toCopy){
	w        = toCopy.getW();
	h        = toCopy.getH();
	pixels   = new Pixel[w*h];
	memcpy(pixels, toCopy.pixels, w*h);
}

Image::~Image(void){
	if(pixels != NULL){
		delete [] pixels;
	}
}
void Image::ResetSize(int width, int height){
	this->setW(width);
	this->setH(height);
	if (pixels != NULL){
		delete []pixels;
	}
	Pixel * pix = new Pixel[width*height];
	this->setPixels(pix);
}

void Image::Update() const{
	if (upPipe != NULL){
		upPipe->UpdatePipe();
	}
}
#endif
