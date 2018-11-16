#ifndef PNM_WRITER
#define PNM_WRITER
#include <sink.h>
class PNMwriter : public Sink{
	virtual const char *SinkName() const {return "PNMwriter";};	
   public:
	void Write(const char *filename);
};
#endif
