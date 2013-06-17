#include <stdio.h>
#include <assert.h>
#include <string.h>


#include "inputFilters.h"



xmlXPathObjectPtr filter990(htmlDocPtr* pdoc, xmlXPathContextPtr* pxpathCtx, char * path)
{
    xmlXPathObjectPtr xpathObj = NULL;
    
    if (path == NULL) {
        /// main page
        
        * pdoc = htmlReadFile ("http://990.ro", NULL, 0);
        if (*pdoc == NULL) {
            printf("Error: cannot read file\n");
            return(NULL);        
        }
        printf("File was read\n");
        
        *pxpathCtx = xmlXPathNewContext(*pdoc);
        if(*pxpathCtx == NULL) {
            printf("Error: unable to create new XPath context\n");
            xmlFreeDoc(*pdoc); 
            return(NULL);
        }
        /* Evaluate xpath expression */
        xmlChar* xpathExpr = "//*[@id='ddtopmenubar']//a";
        xpathObj = xmlXPathEvalExpression(xpathExpr, *pxpathCtx);
        if(xpathObj == NULL) {
            fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
            xmlXPathFreeContext(*pxpathCtx); 
            xmlFreeDoc(*pdoc); 
            return(NULL);
        }
        
    }
    else if (strcmp(path,"seriale.html") == 0) {
        /// main page
        
        * pdoc = htmlReadFile ("http://990.ro/seriale.html", NULL, 0);
        if (*pdoc == NULL) {
            printf("Error: cannot read file\n");
            return(NULL);        
        }
        printf("File was read\n");
        
        *pxpathCtx = xmlXPathNewContext(*pdoc);
        if(*pxpathCtx == NULL) {
            printf("Error: unable to create new XPath context\n");
            xmlFreeDoc(*pdoc); 
            return(NULL);
        }
        /* Evaluate xpath expression */
        xmlChar* xpathExpr = "//a[@class='titlu']";
        xpathObj = xmlXPathEvalExpression(xpathExpr, *pxpathCtx);
        if(xpathObj == NULL) {
            fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
            xmlXPathFreeContext(*pxpathCtx); 
            xmlFreeDoc(*pdoc); 
            return(NULL);
        }
        
    }

    return(xpathObj);
    
}


xmlXPathObjectPtr handleInput(htmlDocPtr* pdoc, xmlXPathContextPtr* pxpathCtx, const struct mg_request_info *request_info)
{
    xmlXPathObjectPtr xpathObj = NULL;
    char* query_string = NULL;
    char* path = NULL;
    char* server = NULL;    
    /// detecting server, to select right input plugin
    printf("Query string: %s\n", request_info->query_string);
    query_string = strdup(request_info->query_string);
    if (server = strtok(query_string, "&"))
    {
        path = strtok(NULL, "&" );
    }
    else {
        server = query_string;
    }
    
    printf("Server: %s, path: %s\n", server, path);

    if (strcmp(server, "www.990.ro") == 0) {
        /// 990 server, so handle it there
        
        xpathObj = filter990( pdoc, pxpathCtx, path);
    }
    free(query_string);
    query_string = NULL;
    return xpathObj;

}
