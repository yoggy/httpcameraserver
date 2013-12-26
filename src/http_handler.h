#pragma once

#ifdef WIN32
	#include <SDKDDKVer.h>
	#pragma warning(disable:4996)
	#pragma warning(disable:4819)
#endif

#include <opencv2/core/core.hpp>
#include <boost/network/protocol/http/server.hpp>

class HttpHandler;
typedef boost::network::http::server<HttpHandler> http_server;

class HttpHandler
{
public:
	HttpHandler();
	virtual ~HttpHandler();
	void operator()(http_server::request const &req, http_server::response &res);
	void log(http_server::string_type const &info);

protected:
	void process_index_html(http_server::request const &req, http_server::response &res);
	void process_camera_jpg(http_server::request const &req, http_server::response &res);

public:
	static void set_image(const cv::Mat &image);
	static cv::Mat get_image();

private:
	static cv::Mat image_;
	static boost::mutex mutex_;
};

