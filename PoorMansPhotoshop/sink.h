#ifndef SINK
#define SINK
#include <stdio.h>
#include <logging.h>
#include <image.h>
class Sink{
   protected:
	const Image *in1;
	const Image *in2;
	virtual const char *SinkName() const =0;
   public:	
		Sink(void);
	void SetInput(Image *image) {in1 = image;};
	void SetInput2(Image *image) {in2 = image;};
	const Image *getIn1(void) const {return in1;};
	const Image *getIn2(void) const {return in2;};
};
#endif
