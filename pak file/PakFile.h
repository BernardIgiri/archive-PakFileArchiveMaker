#pragma once

namespace pk
{
	lg::Log<std::fstream> Log(); // logs messages from PakFile operations

	namespace io = boost::iostreams;

	class PakFile; // PakFile class
	class IS; // input stream
	class OS; // output stream
	class IOS; // input/output stream
	class Entry // entry in PakFile
	{
	public:
		friend PakFile;

		typedef size_t keyType_t; // hash key type
		inline static keyType_t Hash(const std::string &path) { return stdext::hash_value(path); }; // hash function

		Entry() : size(0), location(0), id(0) {};
		Entry(const std::string &path) : size(0), location(0), id(Hash(path)) {};
		~Entry() {};
		Entry(const Entry &e) : size(e.size), location(e.location), id(e.id) {};
		Entry &operator=(const Entry &e) { size=e.size;location=e.location;id=e.id;return *this; };

		bool IsValid() const { return (size&&location); }; // returns true if this entry is valid
		uint32 Start() const { return location; }; // returns start position of entry in PakFile
		uint32 End() const { return location+size; }; // returns end position of entry in PakFile
		uint32 Size() const { return size; }; // returns size of entry in PakFile

		std::ostream &Print(std::ostream &os) const; // serializes object data to stream
		friend std::ostream &operator<<(std::ostream &os,const Entry &o); // serializes object data to stream

		bool operator==(const Entry &e) const { return id==e.id; };
		bool operator<(const Entry &e) const { return id<e.id; };
		bool operator>(const Entry &e) const { return id>e.id; };
	private:
		keyType_t id; // hashed id to entry
		uint32 size; // size of entry in bytes
		uint32 location; // position of entry in PakFile
	};

	const char PKLabel[] = "PKFile"; // identifing label for all PakFile's

	class Header // header in PakFile
	{
	public:		
		Header() : version(0), fileSize(0), nEntries(0), FATEndPos(0) { memcpy((char*)label,PKLabel,sizeof(PKLabel)); };
		~Header() {};

		bool IsValid() const { return strcmp(PKLabel,(const char*)label)==0; }; // returns true if header is valid

		byte label[7]; // label for PakFile, contains binary equivalent of PKLabel
		uint32 version; // version number for file
		uint32 nEntries; // number of entries in file

		// updated in final step of file creation
		uint32 fileSize; // size of file
		uint32 FATEndPos; // end position of FAT

		uint32 FATEnd() const { return FATEndPos; }; // end position of FAT

		uint32 FATBegin() const { return sizeof(*this); }; // start position of FAT table in PakFile

		std::ostream &Print(std::ostream &os) const; // serializes object data to stream
		friend std::ostream &operator<<(std::ostream &os,const Header &o); // serializes object data to stream
	};

	class PakFile
	{
	public:
		typedef std::vector<Entry> FAT_t; // typedef for FAT vector in PakFile

		PakFile(const std::string& path) { Open(path); }; // opens specified PakFile
		~PakFile(void) { Close(); }; // closes PakFile

		// returns entry for file with given path (if Entry::IsValid()==false then entry was not found)
		Entry GetEntry(const std::string &path);

		std::ostream &Print(std::ostream &os) const; // serializes object data to stream

		friend std::ostream &operator<<(std::ostream &os,const PakFile &o); // serializes object data to stream
		friend IS; // input stream
		friend OS; // output stream
		friend IOS; // input/output stream
	private:
		void Open(const std::string& path); // opens the PakFile
		void Close() { fs.close(); }; // closes the PakFile

		FAT_t FAT; // sorted vector of file allocation table
		Header header; // PakFile header
		std::fstream fs; // file stream for pack file
	};

	struct is_tag // tag for input streams
		: virtual io::input_seekable,
		  virtual io::device_tag,
		  virtual io::peekable_tag
		{ };

	struct os_tag // tag for output streams
		: virtual io::output_seekable,
		  virtual io::device_tag,
		  virtual io::peekable_tag
		{ };

	struct ios_tag // tag for input/output streams
		: io::seekable_device_tag,
		  is_tag,
		  os_tag
		{ };

	typedef char ios_char_type;

	class IS
	{
	public:
		typedef ios_char_type	char_type;
		typedef is_tag			category;

		IS(PakFile &pkf_,const std::string &path) : pos(0), pkf(pkf_), entry(pkf_.GetEntry(path)) { };

		IS(PakFile &pkf_) :  pos(0), pkf(pkf_) {};
		void open(const std::string &path) { entry=pkf.GetEntry(path); };

		std::streamsize read(char_type* s, std::streamsize n);

		io::stream_offset seek(io::stream_offset off, std::ios_base::seekdir way);

	private:
		uint32 pos; // current read position
		PakFile &pkf; // reference to pak file
		Entry entry; // entry data for file in pak file
	};

	class OS
	{
	public:
		typedef ios_char_type	char_type;
		typedef os_tag			category;

		OS(PakFile &pkf_,const std::string &path) : pos(0), pkf(pkf_), entry(pkf_.GetEntry(path)) { };

		OS(PakFile &pkf_) :  pos(0), pkf(pkf_) {};
		void open(const std::string &path) { entry=pkf.GetEntry(path); };

		std::streamsize write(const char_type* s, std::streamsize n);

		io::stream_offset seek(io::stream_offset off, std::ios_base::seekdir way);

	private:
		uint32 pos; // current read position
		PakFile &pkf; // reference to pak file
		Entry entry; // entry data for file in pak file
	};

	class IOS
	{
	public:
		typedef ios_char_type	char_type;
		typedef ios_tag			category;

		IOS(PakFile &pkf_,const std::string &path) : pos(0), pkf(pkf_), entry(pkf_.GetEntry(path)) { };

		IOS(PakFile &pkf_) :  pos(0), pkf(pkf_) {};
		void open(const std::string &path) { entry=pkf.GetEntry(path); };
		
		std::streamsize read(char_type* s, std::streamsize n);

		std::streamsize write(const char_type* s, std::streamsize n);

		io::stream_offset seek(io::stream_offset off, std::ios_base::seekdir way);

	private:
		uint32 pos; // current read position
		PakFile &pkf; // reference to pak file
		Entry entry; // entry data for file in pak file
	};
};