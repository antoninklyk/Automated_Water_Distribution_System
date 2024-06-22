#ifndef DEF_NETWORK
#define DEF_NETWORK

#include <WiFi.h>
#include <HTTPClient.h>
#include "string.h"

void network_setup();
void http_send_data(String dataToSend);

#endif