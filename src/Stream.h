#ifndef STREAM_H
#define STREAM_H

#include 

// TODO: Add UTF-8 support
// TODO: Add CLI stream support

class Stream {
	public:
		Stream(const std::string & path);
		virtual ~Stream() = default;

		unsigned int get_line();
		unsigned int get_column();

		char peek();
		char advance();

		void error(const std::string & msg);

	private:
		std::string code;

		unsigned int line;
		unsigned int column;
		unsigned int index;
};

#endif