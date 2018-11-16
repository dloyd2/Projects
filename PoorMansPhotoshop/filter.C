#include <filter.h>
#include <image.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Filter::Filter(void){
	message = new char[100];
}
Filter::~Filter(void){
	if (message != NULL){
		delete [] message;
	}
}
void Filter::UpdatePipe() const {
	
	(*timesExecuted)++;
	if ((*timesExecuted) == 2){
		error(FilterName(), "Circular dependancy detected");
	}
	if (in1 != NULL && (in1->getUP()!= NULL)){
		sprintf(message, "%s: requesting input 1 to update", FilterName());
		log(message);
		in1->getUP()->UpdatePipe();
		sprintf(message, "%s: input 1 up to update", FilterName());
		log(message);
	}
	if (in2 != NULL && (in2->getUP()!= NULL)){
		sprintf(message, "%s: requesting input 2 to update", FilterName());
		log(message);
		in2->getUP()->UpdatePipe();
		sprintf(message, "%s: input 2 up to update", FilterName());
		log(message);
	}
	sprintf(message, "%s: Up to date", FilterName());
	log(message);
	this->Execute();
	*timesExecuted = 0;
}

Shrinker::Shrinker(){
	this->out->setUP(this);
}

void Shrinker::Execute(void) const{
	sprintf(message, "%s: beginning to execute", FilterName());
	log(message);
	if (in1 == NULL){
		error(FilterName(), "input image is NULL");
	}
	if (out == NULL){
		error(FilterName(), "output image is NULL");
	}
	int w, j = 0, h =0;
	int width  = this->in1->getW();
	int height = this->in1->getH();
	this->out->ResetSize((width/2), (height/2));
	Pixel *outPix = this->out->getPixels();
	Pixel *inPix = this->in1->getPixels();
	for (; h<height-1; h+=2){
		for (w=0; w<width-1; w+=2){
			outPix[j] = inPix[(h*width)+w];
			j++;
		}	
	}
	sprintf(message, "%s: finished executing", FilterName());
	log(message);
}

LRCombine::LRCombine() {
	this->out->setUP(this);
}
void LRCombine::Execute(void) const{
	sprintf(message, "%s: beginning to execute", FilterName());
	log(message);
	if (in1 == NULL){
		error(FilterName(), "first input image is NULL");
	}
	if (in2 == NULL){
		error(FilterName(), "second input image is NULL");
	}
	if (out == NULL){
		error(FilterName(), "output image is NULL");
	}
	int i =0;
	int width   = this->in1->getW();
	int width2  = this->in2->getW();
	int height  = this->in1->getH();
	int height2 = this->in2->getH();
	if (height != height2){
		sprintf(message, "heights of inputs do not match. Image 1: %d    Image 2: %d", width, width2);
		error(FilterName(), message);
	}
	Pixel *rightPicPtr = this->out->getPixels() + width;	
	this->out->ResetSize((width+width2), height2);
	int outWidth = this->out->getW();
	for(; i < this->out->getH(); i++){
		memcpy(this->out->getPixels()+(i*outWidth), this->in1->getPixels()+(i*width), (width * sizeof(Pixel)));
		memcpy(this->out->getPixels()+(i*outWidth)+width, this->in2->getPixels()+(i*width2), (width2 * sizeof(Pixel)));
	}
	sprintf(message, "%s: finished executing", FilterName());
	log(message);
}

TBCombine::TBCombine(){
	this->out->setUP(this);
}
void TBCombine::Execute(void) const {
	sprintf(message, "%s: beginning to execute", FilterName());
	log(message);
	if (in1 == NULL){
		error(FilterName(), "first input image is NULL");
	}
	if (in2 == NULL){
		error(FilterName(), "second input image is NULL");
	}
	if (out == NULL){
		error(FilterName(), "output image is NULL");
	}
	int width   = this->in1->getW();
	int width2  = this->in2->getW();
	int height  = this->in1->getH();
	int height2 = this->in2->getH();
	if (width != width2){
		sprintf(message, "widths of inputs do not match. Image 1: %d    Image 2: %d", width, width2);
		error(FilterName(), message);
	}
	this->out->ResetSize(width, (height + height2));
	int topPicSize = width*height;
		memcpy(this->out->getPixels(), this->in1->getPixels(), (width*height * sizeof(Pixel)));
		memcpy(this->out->getPixels()+topPicSize, this->in2->getPixels(), (width2*height2 * sizeof(Pixel)));	
	sprintf(message, "%s: finished executing", FilterName());
	log(message);
}

Blender::Blender(){
	this->out->setUP(this);
}
void Blender::Execute(void) const {
	sprintf(message, "%s: beginning to execute", FilterName());
	log(message);
	if (in1 == NULL){
		error(FilterName(), "first input image is NULL");
	}
	if (in2 == NULL){
		error(FilterName(), "second input image is NULL");
	}
	if (out == NULL){
		error(FilterName(), "output image is NULL");
	}
	if (factor < 0){
		sprintf(message, "blend factor is too low: %f is smaller than minimum of 0", factor);
		error(FilterName(), message);
	}
	if (factor > 1){
		sprintf(message, "blend factor is too high: %f is larger than maximum of 1", factor);
		error(FilterName(), message);
	}
	if (in1->getW() !=  in2->getW()){
		sprintf(message, "input widths do not match. Input1 W: %d    Input2 W: %d\n", in1->getW(), in2->getW());
		error(FilterName(), message);
	}
	if (in1->getH() != in2->getH()){
		sprintf(message, "input heights do not match. Input1 H: %d    Input2 H: %d\n", in1->getH(), in2->getH());
		error(FilterName(), message);
	}
	Pixel *in1Pixels = this->in1->getPixels();
	Pixel *in2Pixels = this->in2->getPixels();
	int i =0;
	this->out->ResetSize(this->in1->getW(), this->in2->getH());
	Pixel *outPixels = this->out->getPixels();
	for(; i < this->out->getH()*this->out->getW(); i++){
		outPixels[i].setR(factor*(in1Pixels[i].getR()) + remainder*(in2Pixels[i].getR()));
		outPixels[i].setG(factor*(in1Pixels[i].getG()) + remainder*(in2Pixels[i].getG()));
		outPixels[i].setB(factor*(in1Pixels[i].getB()) + remainder*(in2Pixels[i].getB()));
	}
	sprintf(message, "%s: finished executing", FilterName());
	log(message);
}

Mirror::Mirror(void){
	this->out->setUP(this);
}
void Mirror::Execute(void) const {
	sprintf(message, "%s: beginning execution", FilterName());
	log(message);
	if (in1 == NULL){
		error(FilterName(), "input image is NULL");
	}
	if (out == NULL){
		error(FilterName(), "output image is NULL");
	}
	int width = this->in1->getW();
	int height = this->in1->getH();
	this->out->ResetSize(width, height);
	Pixel *inPixels = this->in1->getPixels();
	Pixel *outPixels = this->out->getPixels();
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			outPixels[(i*width)+j] = inPixels[(i*width) + width - j-1];
		}	
	}
	sprintf(message, "%s: finished execution", FilterName());
	log(message);
}

Rotate::Rotate(void){
	this->out->setUP(this);
}
void Rotate::Execute(void) const{
	sprintf(message, "%s: beginning execution", FilterName());
	log(message);
	if (in1 == NULL){
		error(FilterName(), "input image is NULL");
	}
	if (out == NULL){
		error(FilterName(), "output image is NULL");
	}
	int inWidth = this->in1->getW();
	int inHeight = this->in1->getH();
	int outHeight = inWidth;
	int outWidth = inHeight;
	this->out->ResetSize(outWidth, outHeight);
	Pixel *inPixels = this->in1->getPixels();
	Pixel *outPixels = this->out->getPixels();
	for (int i = 0; i < outHeight; i++){
		for (int j = 0; j < outWidth; j++){
			outPixels[(i*outWidth)+j] = inPixels[((inHeight - j-1)*inWidth) + i];
		}	
	}
	sprintf(message, "%s: finished execution", FilterName());
	log(message);
}
Subtract::Subtract(void){
	this->out->setUP(this);
}
void Subtract::Execute(void) const{
	sprintf(message, "%s: beginning execution", FilterName());
	log(message);
	if (in1 == NULL){
		error(FilterName(), "first input image is NULL");
	}
	if (in2 == NULL){
		error(FilterName(), "second input image is NULL");
	}
	if (out == NULL){
		error(FilterName(), "output image is NULL");
	}
	int j, i;
	int in1W = this->in1->getW();
	int in1H = this->in1->getH();
	int in2W = this->in2->getW();
	int in2H = this->in2->getH();
	if (in1W != in2W || in1H != in2H){
		sprintf(message, "input dimenstions do not match.\nFirst input dimensions: %d %d Second input\ndimensions: %d %d", in1W, in1H, in2W, in2H);
		error(FilterName(), message); 
	}
	this->out->ResetSize(in1W, in1H);
	Pixel *in1Pixels = this->in1->getPixels();
	Pixel *in2Pixels = this->in2->getPixels();
	Pixel *outPixels = this->out->getPixels();
	for (i = 0; i < in1H; i++){
		for (j = 0; j < in1W; j++){
			int tempR = in1Pixels[(i*in1W) + j].getR() - in2Pixels[(i*in2W) + j].getR();
			int largerR = tempR < 0 ? 0 : tempR;
			int tempG = in1Pixels[(i*in1W) + j].getG() - in2Pixels[(i*in2W) + j].getG();
			int largerG = tempG < 0 ? 0 : tempG;
			int tempB = in1Pixels[(i*in1W) + j].getB() - in2Pixels[(i*in2W) + j].getB();
			int largerB = tempB < 0 ? 0 : tempB;
			outPixels[(i*in1W)+j].setR(largerR);
			outPixels[(i*in1W)+j].setG(largerG);
			outPixels[(i*in1W)+j].setB(largerB);
		}	
	}
	sprintf(message, "%s: finished execution", FilterName());
	log(message);
}

Grayscale::Grayscale(void){
	this->out->setUP(this);
}
void Grayscale::Execute(void) const {
	sprintf(message, "%s: beginning execution", FilterName());
	log(message);
	if (in1 == NULL){
		error(FilterName(), "first input image is NULL");
	}
	if (out == NULL){
		error(FilterName(), "output image is NULL");
	}
	int in1W = this->in1->getW();
	int in1H = this->in1->getH();
	this->out->ResetSize(in1W, in1H);
	Pixel *in1Pixels = this->in1->getPixels();
	Pixel *outPixels = this->out->getPixels();
	for (int i = 0; i < in1H; i++){
		for (int j = 0; j < in1W; j++){
			int tempGray = (in1Pixels[(i*in1W) + j].getR()/5) + (in1Pixels[(i*in1W) + j].getG()/2) + (in1Pixels[(i*in1W) + j].getB()/4);
			outPixels[(i*in1W)+j].setR(tempGray);
			outPixels[(i*in1W)+j].setG(tempGray);
			outPixels[(i*in1W)+j].setB(tempGray);
		}	
	}
	sprintf(message, "%s: finished execution", FilterName());
	log(message);
}

Blur::Blur(void){
	this->out->setUP(this);
}
void Blur::Execute(void) const{
	sprintf(message, "%s: beginning execution", FilterName());
	log(message);
	if (in1 == NULL){
		error(FilterName(), "first input image is NULL");
	}
	if (out == NULL){
		error(FilterName(), "output image is NULL");
	}
	int in1W = this->in1->getW();
	int in1H = this->in1->getH();
	this->out->ResetSize(in1W, in1H);
	Pixel *in1Pixels = this->in1->getPixels();
	Pixel *outPixels = this->out->getPixels();
	int j, i;
	for (i = 0; i < in1H; i++){
		for (j = 0; j < in1W; j++){
			if (i != 0 && i != in1H-1 && j != 0 && j != in1W-1){
				int tempR = (in1Pixels[((i-1)*in1W) + j].getR()/8) + (in1Pixels[((i+1)*in1W) + j].getR()/8) + (in1Pixels[(i*in1W) + j - 1].getR()/8) + (in1Pixels[(i*in1W) + j + 1].getR()/8) + (in1Pixels[((i-1)*in1W) + j - 1].getR()/8) + (in1Pixels[((i-1)*in1W) + j + 1].getR()/8) + (in1Pixels[((i+1)*in1W) + j - 1].getR()/8) + (in1Pixels[((i+1)*in1W) + j + 1].getR()/8);
				int tempG = (in1Pixels[((i-1)*in1W) + j].getG()/8) + (in1Pixels[((i+1)*in1W) + j].getG()/8) + (in1Pixels[(i*in1W) + j - 1].getG()/8) + (in1Pixels[(i*in1W) + j + 1].getG()/8) + (in1Pixels[((i-1)*in1W) + j - 1].getG()/8) + (in1Pixels[((i-1)*in1W) + j + 1].getG()/8) + (in1Pixels[((i+1)*in1W) + j - 1].getG()/8) + (in1Pixels[((i+1)*in1W) + j + 1].getG()/8);
				int tempB = (in1Pixels[((i-1)*in1W) + j].getB()/8) + (in1Pixels[((i+1)*in1W) + j].getB()/8) + (in1Pixels[(i*in1W) + j - 1].getB()/8) + (in1Pixels[(i*in1W) + j + 1].getB()/8) + (in1Pixels[((i-1)*in1W) + j - 1].getB()/8) + (in1Pixels[((i-1)*in1W) + j + 1].getB()/8) + (in1Pixels[((i+1)*in1W) + j - 1].getB()/8) + (in1Pixels[((i+1)*in1W) + j + 1].getB()/8);
				outPixels[(i*in1W)+j].setR(tempR);
				outPixels[(i*in1W)+j].setG(tempG);
				outPixels[(i*in1W)+j].setB(tempB);
			}else{
				outPixels[(i*in1W)+j].setR( this->in1->getPixels()[(i*in1W)+j].getR());
				outPixels[(i*in1W)+j].setG( this->in1->getPixels()[(i*in1W)+j].getG());
				outPixels[(i*in1W)+j].setB( this->in1->getPixels()[(i*in1W)+j].getB());
			}
		}	
	}
	sprintf(message, "%s: finished execution", FilterName());
	log(message);
}
Color::Color(int width, int height, int r, int g, int b) : Source(width, height){
	Pixel *pixels = this->out->getPixels();
	for (int i = 0; i < width *height; i++){
		pixels[i].setR(r);
		pixels[i].setG(g);
		pixels[i].setB(b);
	}
	this->out->setUP(this);
}
void Color::Execute() const{
	
}
CheckSum::CheckSum(void){
	totalR = 0;
	totalG = 0;
	totalB = 0;
}
void CheckSum::OutputCheckSum(const char *file) {
	char msg[50];
	sprintf(msg, "%s: beginning to write checksum to output", SinkName());
	log(msg);
	if (in1 == NULL){error(SinkName(), "output image is NULL");}
	int in1W = in1->getW();
	int in1H = in1->getH();
	Pixel *pixels = in1->getPixels();
	for (int i = 0; i < in1W*in1H; i++){
		totalR += pixels[i].getR();
		totalG += pixels[i].getG();
		totalB += pixels[i].getB();
	} 
	FILE *f_out = fopen(file, "w");
	if (f_out == NULL){error(SinkName(), "could not open output file");}
	fprintf(f_out, "CHECKSUM: %d, %d, %d\n", totalR, totalG, totalB);
	fclose(f_out);
	sprintf(msg, "%s: finsihed writing to output", SinkName());
	log(msg);
}
