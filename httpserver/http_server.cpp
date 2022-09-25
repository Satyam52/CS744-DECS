#include "http_server.hh"
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <string>

vector<string> split(const string &s, char delim) {
  vector<string> elems;
  stringstream ss(s);
  string item;

  while (getline(ss, item, delim)) {
    if (!item.empty())
      elems.push_back(item);
  }
  return elems;
}

HTTP_Request::HTTP_Request(string request) {
  vector<string> lines = split(request, '\n');
  vector<string> first_line = split(lines[0], ' ');

  this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request

  //BEGIN - extract the request method and URL from first_line here
  if (first_line.size() != 3) {
    cerr << "Wrong RFC format" << endl;
    exit(1);
  }
  this->method = first_line[0];
  this->url = first_line[1];
  //END - extract the request method and URL from first_line here

  if (this->method != "GET") {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }
}

HTTP_Response *handle_request(string req) {
  HTTP_Request *request = new HTTP_Request(req);

  HTTP_Response *response = new HTTP_Response();

  string url = string("html_files") + request->url;

  response->HTTP_version = "1.0";

  struct stat sb;
  if (stat(url.c_str(), &sb) == 0) // requested path exists
  {
    response->status_code = "200";
    response->status_text = "OK";
    response->content_type = "text/html";
    
    string body;

    if (S_ISDIR(sb.st_mode)) {
      /*
      BEGIN - In this case, requested path is a directory.
      TODO : find the index.html file in that directory (modify the url
      accordingly)
      */
      url = url + "/" + "index.html";
      if (stat(url.c_str(), &sb) != 0) {
        response->status_code = "404";
        string htmlError = "<header><h2>The page you requested is not found!</h2></header>";
        response->status_text = "NOT FOUND";
        response->content_type = "text/html";
        response->content_length = to_string(htmlError.length());
        response->body = htmlError;

        delete request;
        return response;
      }
      /*
      END
      */
    }


  // BEGIN - open the file and read its contents
    ifstream infile(url);
    string line;
    stringstream sbody;
    while (getline(infile, line))
    {
      sbody << line << "\n";
    }
    // END - open the file and read its contents

  // BEGIN - set the remaining fields of response appropriately
    response->body = sbody.str();
    response->content_length = to_string(sbody.str().length());
  // END - set the remaining fields of response appropriately
    
  }
  else {
    response->status_code = "404";
  
    // BEGIN - set the remaining fields of response appropriately
    string htmlError = "<header><h2>The page you requested is not found!</h2></header>";
    response->status_text = "NOT FOUND";
    response->content_type = "text/html";
    response->content_length = to_string(htmlError.length());
    response->body = htmlError;
    // END - set the remaining fields of response appropriately
  }

  delete request;
  return response;
}

string HTTP_Response::get_string() {
  
  // BEGIN - implement this function
  stringstream ss;
  ss <<"HTTP/"<< this->HTTP_version << " " << this->status_code << " " << this->status_text;
  ss << "\n" << "Content-Type: " << this->content_type;
  ss << "\n" << "Content-Length: " << this->content_length;
  ss << "\n\n" << this->body;
  
  return ss.str();
}
