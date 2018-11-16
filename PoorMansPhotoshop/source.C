#include <image.h>
#include <source.h>
#include <stdlib.h>
Source::Source(int width, int height){
	out = new Image(width, height);
	timesExecuted = new int;
	*timesExecuted = 0;
}
Source::Source(){
	out = new Image;
	timesExecuted = new int;
	*timesExecuted = 0;
}
Source::~Source(){
	if (out != NULL){
		delete out;
	}
	if (timesExecuted != NULL){
		delete timesExecuted;
	}
}
void Source::UpdatePipe() const{
	this->Execute();
}
