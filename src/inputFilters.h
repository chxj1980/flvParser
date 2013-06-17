#ifndef _INPUTFILTERS_H_
#define _INPUTFILTERS_H_

#include "libxml/HTMLparser.h"
#include "libxml/xpath.h"

#include "mongoose.h"

xmlXPathObjectPtr filter990(htmlDocPtr* pdoc, xmlXPathContextPtr* pxpathCtx, char * path);
xmlXPathObjectPtr handleInput(htmlDocPtr* pdoc, xmlXPathContextPtr* pxpathCtx, const struct mg_request_info *request_info);

#endif /* _INPUTFILTERS_H_ */

