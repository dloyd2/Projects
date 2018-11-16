#include <logging.h>
#include <stdio.h>
FILE * Logger::logger = fopen("log", "w");
void Logger::LogEvent(const char *event){
	fprintf(Logger::logger, "%s", event);		
	fprintf(Logger::logger, "\n");		
}

void Logger::Finalize(){
	fclose(Logger::logger);
}

DataFlowException::DataFlowException(const char *type, const char *error){
	sprintf(msg, "Caught error at %s\nERROR WAS: %s\nExiting to avoid crash", type, error);
	Logger::LogEvent(msg);	
}

void error(const char *type, const char *error){
	DataFlowException e(type, error);
	throw e;
}
void log(const char *msg){
	Logger::LogEvent(msg);
}
