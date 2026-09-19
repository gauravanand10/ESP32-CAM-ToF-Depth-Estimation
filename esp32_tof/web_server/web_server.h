#ifndef WEB_SERVER_H
#define WEB_SERVER_H


// ============================================================
// INITIALIZE HTTP SERVER
// ============================================================
//
// Registers all HTTP endpoints and starts the server.
//

void web_server_init();


// ============================================================
// HANDLE HTTP CLIENTS
// ============================================================
//
// Processes incoming HTTP requests.
//
// This should be called continuously from loop().
//

void web_server_handle();


#endif
