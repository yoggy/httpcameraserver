#include "http_handler.h"

#include <boost/format.hpp>
#include <opencv2/highgui/highgui.hpp>

extern int jpeg_quality;

cv::Mat HttpHandler::image_;
boost::mutex HttpHandler::mutex_;

HttpHandler::HttpHandler()
{
}

HttpHandler::~HttpHandler()
{
}

void HttpHandler::log(http_server::string_type const &info)
{
	std::cout << "log : " << info << std::endl;
}

void HttpHandler::operator()(http_server::request const &req, http_server::response &res)
{
	std::string path = req.destination;
	if (path == "/" || path == "/index.html") {
		process_index_html(req, res);
	}
	else if (path.length() >= 11 && path.substr(0, 11) == "/camera.jpg") {
		process_camera_jpg(req, res);
	}
	else {
		res = http_server::response::stock_reply(
			http_server::response::ok, "not found...");
	}
}

void HttpHandler::process_index_html(http_server::request const &req, http_server::response &res)
{
	res = http_server::response::stock_reply(
		http_server::response::ok, "<html><head><script>function reload(){document.camaera_jpg.src='camera.jpg?t='+new Date().getTime();}function init() {setInterval(reload, 1000);}</script></head><body onLoad='init()'><img name='camaera_jpg' src='camera.jpg'/></body><html>");
}

void HttpHandler::process_camera_jpg(http_server::request const &req, http_server::response &res)
{
	// encode jpeg image
	cv::Mat image = get_image();
	if (image.empty()) {
		image.create(cv::Size(640, 480), CV_8UC3);
		image = CV_RGB(0, 0, 255);
	}

	std::vector<uchar> buf;
	std::vector<int> params(2);

	params[0] = CV_IMWRITE_JPEG_QUALITY;
	params[1] = jpeg_quality;
	cv::imencode(".jpg", image, buf, params);

	// create response
	res = http_server::response::stock_reply(http_server::response::ok, "dummy...");
	res.headers[1].value = "image/jpeg";
	res.headers[0].value = (boost::format("%d") % buf.size()).str();

	// for CORS restriction...
	res.headers.resize(res.headers.size() + 2);
	res.headers[res.headers.size() - 2].name = "X-Content-Type-Options";
	res.headers[res.headers.size() - 2].value = "nosniff";
	res.headers[res.headers.size() - 1].name = "Access-Control-Allow-Origin";
	res.headers[res.headers.size() - 1].value = "*";

	res.content = std::string((const char*)&buf[0], buf.size());
}

void HttpHandler::set_image(const cv::Mat &image)
{
	boost::mutex::scoped_lock(mutex_);

	image.copyTo(image_);
}

cv::Mat HttpHandler::get_image()
{
	boost::mutex::scoped_lock(mutex_);

	cv::Mat copy_img;
	image_.copyTo(copy_img);
	return copy_img;
}