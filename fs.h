#ifndef FS_H_INCLUDED
#define FS_H_INCLUDED
#include <fstream>

#define FS_READ ios::in
#define FS_WRITE ios::out
#define FS_BINARY ios::binary

namespace fs
{
	class file
	{
	private:
		fstream _stream;
	public:
		fstream* getStream();
		void write(string data);
		void writeBinary(const char *data, size_t size);
		string read();
		void readBinary(char *data, size_t size);
		void close();
	};

	class exception
	{
		string what_str;
	public:
		exception(string what);
		string what();
	};

	bool exists(std::string path);

	file *open(std::string path, int flags = FS_READ | FS_WRITE);

	void writeData(string path, string data);
	void safeWriteData(string path, string data);
	string readData(string path);
	string getRootPath();
	void rename(string oldname, string newname);
}

#endif
