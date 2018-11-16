#ifndef PNM_READER
#define PNM_READER
#include <image.h>
#include <source.h>
class PNMreader : public Source{
	char *buffer;
	virtual const char *SourceName() const{return "PNMreader";};
   public:
		PNMreader(const char *filename);
		~PNMreader(void);
	virtual void Execute() const;
};
#endif
