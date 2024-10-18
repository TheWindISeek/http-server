#include <sys/resource.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <fstream>
#include <dirent.h>  // 提供 opendir, readdir, closedir
#include <cstring>  // 提供 strlen

#include "http_message.h"
#include "http_server.h"
#include "uri.h"

using simple_http_server::HttpMethod;
using simple_http_server::HttpRequest;
using simple_http_server::HttpResponse;
using simple_http_server::HttpServer;
using simple_http_server::HttpStatusCode;

void ensure_enough_resource(int resource, std::uint32_t soft_limit,
                            std::uint32_t hard_limit) {
  rlimit new_limit, old_limit;

  new_limit.rlim_cur = soft_limit;
  new_limit.rlim_max = hard_limit;
  getrlimit(resource, &old_limit);

  std::cout << "Old limit: " << old_limit.rlim_cur << " (soft limit), "
            << old_limit.rlim_cur << " (hard limit)." << std::endl;
  std::cout << "New limit: " << new_limit.rlim_cur << " (soft limit), "
            << new_limit.rlim_cur << " (hard limit)." << std::endl;

  if (setrlimit(resource, &new_limit)) {
    std::cerr << "Warning: Could not update resource limit ";
    std::cerr << "(" << strerror(errno) << ")." << std::endl;
    std::cerr << "Consider setting the limit manually with ulimit" << std::endl;
    exit(-1);
  }
}

void getFilesInDirectory(DIR *dirp, const char *baseDir, std::vector<std::string> &html_filenames) {
    // assert(dirp != NULL);

    struct dirent *entry;
    while ((entry = readdir(dirp)) != NULL) {
        // 跳过 "." 和 ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char fullPath[PATH_MAX];
        snprintf(fullPath, PATH_MAX, "%s/%s", baseDir, entry->d_name);

        struct stat statbuf;
        if (stat(fullPath, &statbuf) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                // 如果是目录，则递归遍历
                DIR *subDir = opendir(fullPath);
                if (subDir) {
                    printf("Directory: %s\n", fullPath);
                    getFilesInDirectory(subDir, fullPath, html_filenames);
                    closedir(subDir);
                } else {
                    perror("opendir");
                }
            } else {
                // 如果是文件，则打印文件名
                html_filenames.push_back(fullPath);
                // printf("File: %s\n", fullPath);
            }
        } else {
            perror("stat");
        }
    }
}

int main(void) {
  std::string host = "0.0.0.0";
  int port = 8080;
  const std::string default_page = "/main.html";
  HttpServer server(host, port);

  // Register a few endpoints for demo and benchmarking
  // auto say_hello = [](const HttpRequest& request) -> HttpResponse {
  //   HttpResponse response(HttpStatusCode::Ok);
  //   response.SetHeader("Content-Type", "text/plain");
  //   response.SetContent("Hello, world\n");
  //   return response;
  // };
  auto send_html = [](const HttpRequest& request) -> HttpResponse {
    HttpResponse response(HttpStatusCode::Ok);
    std::string content;
    content += "<!doctype html>\n";
    content += "<html>\n<body>\n\n";
    content += "<h1>Hello, world in an Html page</h1>\n";
    content += "<p>A Paragraph</p>\n\n";
    content += "</body>\n</html>\n";

    response.SetHeader("Content-Type", "text/html");
    response.SetContent(content);
    return response;
  };

  // server.RegisterHttpRequestHandler("/", HttpMethod::HEAD, say_hello);
  // server.RegisterHttpRequestHandler("/", HttpMethod::GET, say_hello);
  server.RegisterHttpRequestHandler("/hello.html", HttpMethod::HEAD, send_html);
  server.RegisterHttpRequestHandler("/hello.html", HttpMethod::GET, send_html);
  
  char dirPath[] = "../html";
  DIR *dir = opendir(dirPath);
  if(dir == NULL) {
    perror("dir is null!\n");
    return -1;
  }

  std::vector<std::string> html_filenames;
  getFilesInDirectory(dir, dirPath, html_filenames);
  for(auto filename : html_filenames) {
    auto get_html = [filename](const HttpRequest& request) -> HttpResponse {
      HttpResponse response(HttpStatusCode::Ok);
      std::string context = "";

      std::cout << filename << std::endl;
      std::ifstream inputFile(filename);

      if (!inputFile.is_open()) {
          std::cerr << "Failed to open file." << std::endl;
          response.SetStatusCode(HttpStatusCode::NotFound);
      } else {
        std::string line;
        while (std::getline(inputFile, line)) {
            // std::cout << line << std::endl;
            context += line + '\n';
        }

        inputFile.close();        
      }

      response.SetHeader("Content-Type", "text/html");
      response.SetContent(context);
      return response;
    };
    auto it = filename.rfind("/");
    if(it == filename.length()) {
      std::cerr << "error: split " << filename << std::endl; 
      return -1;
    } 
    std::cout << filename.substr(it) << std::endl;
    server.RegisterHttpRequestHandler(filename.substr(it), HttpMethod::HEAD, get_html);
    server.RegisterHttpRequestHandler(filename.substr(it), HttpMethod::GET, get_html);
    if(filename.substr(it) == default_page) {
      std::cout << "default page " << default_page << std::endl;
      server.RegisterHttpRequestHandler("/", HttpMethod::HEAD, get_html);
      server.RegisterHttpRequestHandler("/", HttpMethod::GET, get_html);
    }
  }



  try {
    // std::cout << "Setting new limits for file descriptor count.." <<
    // std::endl; ensure_enough_resource(RLIMIT_NOFILE, 15000, 15000);

    // std::cout << "Setting new limits for number of threads.." << std::endl;
    // ensure_enough_resource(RLIMIT_NPROC, 60000, 60000);

    std::cout << "Starting the web server.." << std::endl;
    server.Start();
    std::cout << "Server listening on " << host << ":" << port << std::endl;

    std::cout << "Enter [quit] to stop the server" << std::endl;
    std::string command;
    while (std::cin >> command, command != "quit") {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "'quit' command entered. Stopping the web server.."
              << std::endl;
    server.Stop();
    std::cout << "Server stopped" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "An error occurred: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}
