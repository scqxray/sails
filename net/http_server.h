// Copyright (C) 2014 sails Authors.
// All rights reserved.
//
// Official git repository and contact information can be found at
// https://github.com/sails/sails and http://www.sailsxu.com/.
//
// Filename: http_server.h
// Description: http server extends epoll server
//
// Author: sailsxu <sailsxu@gmail.com>
// Created: 2014-10-15 13:37:11

#ifndef NET_HTTP_SERVER_H_
#define NET_HTTP_SERVER_H_

#include <string>
#include <map>
#include <functional>
#include "sails/net/http.h"
#include "sails/net/epoll_server.h"
#include "sails/net/http_parser.h"

namespace sails {
namespace net {

typedef std::function<void(sails::net::HttpRequest* request,
                           sails::net::HttpResponse* response)> HttpProcessor;

#define HTTPBIND(server, path, obj, fun)                                 \
server->RegisterProcessor(path,                                          \
std::bind(&fun, obj, std::placeholders::_1, std::placeholders::_2));


class HttpServerHandle;

class HttpServer : public EpollServer<HttpRequest> {
 public:
  HttpServer();
  ~HttpServer();

  // 成功返回0,已经存在针对path的处理器返回1,processor为NULL返回-1,其它返回-100
  int RegisterProcessor(std::string path, HttpProcessor processor);

  HttpProcessor FindProcessor(std::string path);

  void SetStaticResourcePath(std::string path) {
    staticResourcePath = path;
  }
  const std::string& StaticResourcePath() {
    return staticResourcePath;
  }

 private:
  // HttpRequest删除器
  void Tdeleter(HttpRequest* data);

  // 创建连接后,为每个connector建立一个http parser,挂在connector上
  void CreateConnectorCB(std::shared_ptr<net::Connector> connector);

  // 调用http parser进行解析
  HttpRequest* Parse(std::shared_ptr<net::Connector> connector);

  // 处理
  void handle(
      const sails::net::TagRecvData<sails::net::HttpRequest> &recvData);

  // 找到对应的processor来处理
  void process(sails::net::HttpRequest* request,
               sails::net::HttpResponse* response);

  // 删除http parser
  void ClosedConnectCB(std::shared_ptr<net::Connector> connector,
                       CloseConnectorReason reason);

 private:
  std::map<std::string, HttpProcessor> processorMap;

  std::string staticResourcePath;
};

}  // namespace net
}  // namespace sails


#endif  // NET_HTTP_SERVER_H_
