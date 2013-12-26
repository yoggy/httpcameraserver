#ifdef WIN32
	#include <SDKDDKVer.h>
		#define WIN32_LEAN_AND_MEAN
		#include <Windows.h>
		#pragma warning(disable:4996)
		#pragma warning(disable:4819)
	#ifdef _DEBUG
		#pragma comment(lib, "opencv_core247d.lib")
		#pragma comment(lib, "opencv_highgui247d.lib")
	#else
		#pragma comment(lib, "opencv_core247.lib")
		#pragma comment(lib, "opencv_highgui247.lib")
	#endif
#endif

#include <iostream>

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

#include <opencv2/highgui/highgui.hpp>

#include "http_handler.h"

int port         = 10080;
int camera_idx   = 0;
int jpeg_quality = 90;
bool no_display  = false;

boost::program_options::options_description desc("options");
HttpHandler handler;

class HTTPServerThread
{
public:
	HTTPServerThread() {
	}

	void run() {
		std::cout << "start httpcameraserver. listen port=" << port << std::endl;

		http_server::options options(handler);
		options.address("0.0.0.0");
		options.port(boost::lexical_cast<std::string>(port));
		http_server s(options);
		s.run();
	}
};


void usage()
{
	std::cerr << desc << std::endl;
	exit(-1);
}

void process_opt(int argc, char* argv[])
{
	desc.add_options()
	  ("help,h",  "print help")
	  ("port,p", boost::program_options::value<int>(&port)->default_value(port), "listen port")
	  ("camera-idx,i", boost::program_options::value<int>(&camera_idx)->default_value(camera_idx), "open camera idx")
	  ("jpeg-quality,j", boost::program_options::value<int>(&jpeg_quality)->default_value(jpeg_quality), "jpeg quality (1-100)")
	  ("no-display,n", "no display window mode...");

	boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

	if(vm.count("help")) {
		usage();
	}

	// check
	if (vm.count("no_display") > 0) no_display = true;
	if (jpeg_quality < 0) jpeg_quality = 0;
	if (jpeg_quality > 100) jpeg_quality = 100;
}

int main(int argc, char* argv[])
{
	process_opt(argc, argv);

	cv::VideoCapture capture;
	if (capture.open(camera_idx) == false) {
		std::cerr << "capture.open() failed...camera_idx=" << camera_idx << std::endl;
		return -1;
	}

	HTTPServerThread server;
	boost::thread thread(&HTTPServerThread::run, &server);
	
	cv::Mat capture_img; 
	while(true) {
		capture >> capture_img;
		HttpHandler::set_image(capture_img);

		if (no_display == true) {
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));
		}
		else {
			cv::imshow("capture_img", capture_img);
			int c = cv::waitKey(1);
			if (c == 27) break;
		}
	}
	
	cv::destroyAllWindows();

	return 0;
}

