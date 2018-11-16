#ifndef FILTER
#define FILTER
#include <sink.h>
#include <source.h>
#include <logging.h>
class Filter : public Source, public Sink{
   public:
		Filter();
		~Filter();
   protected:
	char *message;
	virtual void UpdatePipe(void) const;
	virtual const char *FilterName() const =0;
	virtual const char *SinkName() const {return FilterName();};
	virtual const char *SourceName() const {return FilterName();};
};

class Shrinker : public Filter{
	virtual const char *FilterName() const {return "Shrinker";};
   public:
		Shrinker();
	void Execute(void) const;
};
class LRCombine : public Filter{
	virtual const char *FilterName() const {return "LRCombine";};
   public:
	   	LRCombine();
	void Execute(void) const;
};
class TBCombine : public Filter{
	virtual const char *FilterName() const {return "TBCombine";};
   public:
	   	TBCombine();
	void Execute(void) const;
};
class Blender : public Filter{
	virtual const char *FilterName() const {return "Blender";};
   	double factor;
	double remainder;
   public:
		Blender();
	void SetFactor(double f) {factor = f; remainder =(1-f);};
	void Execute(void) const;
};
class Mirror : public Filter{
   public:
		Mirror(void);
	virtual const char *FilterName() const {return "Mirror";};
	virtual void Execute(void) const;
};
class Rotate : public Filter{
   public:
		Rotate(void);
	virtual const char *FilterName() const {return "Rotate";};
	virtual void Execute(void) const;
};
class Subtract : public Filter{
   public:
		Subtract(void);
	virtual const char *FilterName() const {return "Subtract";};
	virtual void Execute(void) const;
};
class Grayscale : public Filter{
   public:
		Grayscale(void);
	virtual const char *FilterName() const {return "Grayscale";};
	virtual void Execute(void) const;
};
class Blur : public Filter{
   public:
		Blur(void);
	virtual const char *FilterName() const {return "Blur";};
	virtual void Execute(void) const;
};
class Color : public Source{
   public:
		Color (int width, int height, int r, int g, int b);
	virtual const char *SourceName() const {return "Color";};
	virtual void Execute(void) const;
};

class CheckSum :public Sink{
	unsigned char totalR;
	unsigned char totalG;
	unsigned char totalB;

   public:
		CheckSum(void);
	virtual const char *SinkName() const {return "CheckSum";};
	void OutputCheckSum(const char *file);
};
#endif
