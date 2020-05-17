#ifndef STREAM_H
#define STREAM_H

class Stream {
	public:
		Stream();

		virtual ~Stream() = default;

	private:
		unsigned int line;
		unsigned int column;

		unsigned int index;
		char peek();
		char advance();
		
};

#endif