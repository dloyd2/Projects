#ifndef SOURCE
#define SOURCE
#include <logging.h>

class Image;
class Source{
   protected:

	Image *out;
	virtual const char *SourceName() const=0;
	mutable int *timesExecuted;
   public:
		Source(int width, int height);
		Source(void);
		~Source(void);
	Image *GetOutput(void){return out;};
	virtual void Execute(void) const =0;
	virtual void UpdatePipe() const;
};
#endif
