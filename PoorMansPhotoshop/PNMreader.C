#include <PNMreader.h>
#include <logging.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
PNMreader::PNMreader(const char *filename){
	int i = 0;
	int len = strlen(filename);
	buffer = new char[len+1];	
	for(; i<len; i++){
		buffer[i] = filename[i];
	}
	buffer[i] = '\0';
	this->out->setUP(this);
}

PNMreader::~PNMreader(void){
	if (buffer != NULL){
		delete [] buffer;
	}

}

void PNMreader::Execute(void) const{
	char msg[50];
	sprintf(msg, "%s: Reading file", SourceName());
	log(msg);
	char *magicNum = new char[8];
	int width, height, maxval, i;
	FILE *f_in=fopen(this->buffer, "r");
	if (f_in == NULL){	
		error(SourceName(), "could not open input file");
	}
	fscanf(f_in, "%s\n%d %d\n%d\n", magicNum, &width, &height, &maxval);  
	this->out->ResetSize(width, height);
	fread(this->out->getPixels(), sizeof(Pixel), width*height, f_in);
	fclose(f_in);
	sprintf(msg, "%s: finished reading file", SourceName());
	log(msg);
	if (magicNum != NULL){
		delete []magicNum;
	}
	
}

