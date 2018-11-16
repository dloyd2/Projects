#include <PNMwriter.h>
#include <stdlib.h>
#include <stdio.h>
void PNMwriter::Write(const char *filename) {
	char msg[50];
	sprintf(msg, "%s: beginning to write to output", SinkName());
	log(msg);
	FILE *f_out = fopen(filename, "w");
	if (f_out == NULL){error(SinkName(), "could not open output file");}
	if (in1 == NULL){error(SinkName(), "output image is NULL");}
	fprintf(f_out, "P6\n%d %d\n255\n", this->getIn1()->getW(), this->getIn1()->getH());
	fwrite(this->getIn1()->getPixels(), sizeof(char), 3*this->getIn1()->getW()*this->getIn1()->getH(), f_out);
	fclose(f_out);
	sprintf(msg, "%s: finished writing to output", SinkName());
	log(msg);
}
