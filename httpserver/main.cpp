#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include "http_server.hh"
using namespace std;

int main() {
    string ip = "GET /apart1 HTTP/1.0";
    HTTP_Response* response = handle_request(ip);
    string op = response->get_string();
    cout << op << endl;
    return 0;
}
