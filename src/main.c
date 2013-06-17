#include <stdio.h>
#include <assert.h>
#include <string.h>


#include "core.h"


/** 
   * callback
   * 
   * @param event 
   * @param conn 
   * 
   * @return NULL if error processing, or anything else
   */
static void *web_server_callback(enum mg_event event,
                      struct mg_connection *conn) {
    const struct mg_request_info *request_info;
    if (event == MG_NEW_REQUEST) {
        request_info = mg_get_request_info(conn);
        if (strcmp(request_info->request_method, "GET") == 0) {
            if (strstr(request_info->uri, "flvParser")) {
                handleFlvParserGetRequest(conn, request_info);
            }
        }
        return "";
    } else {
        return NULL;
    }
}

/** 
 * main function
 * it starts a mongoose web server and registers web_server_callback to it
 * 
 * @param argc 
 * @param argv 
 * 
 * @return 
 */
int main(int argc, char **argv)
{

    struct mg_context *web_server_context;
    const char *web_server_options[] = {"listening_ports", "8080", NULL};

    web_server_context = mg_start(&web_server_callback, NULL, web_server_options);//@}
    getchar();  // Wait until user hits "enter"
    mg_stop(web_server_context);
    
    return(0);
}
//@} 
