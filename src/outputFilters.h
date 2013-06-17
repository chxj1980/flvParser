#ifndef _OUTPUTFILTERS_H_
#define _OUTPUTFILTERS_H_

#include "libxml/HTMLparser.h"
#include "libxml/xpath.h"

#include "mongoose.h"

int handleWgetOutput(xmlNodeSetPtr nodes, char* content, char* server);
int handleOutput(const struct mg_request_info *request_info, xmlNodeSetPtr nodes, char * content);



#endif /* _OUTPUTFILTERS_H_ */
