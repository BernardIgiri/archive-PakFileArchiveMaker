#include "StdAfx.h"
#include "PakFile.h"

using namespace pk;

lg::Log<std::fstream> pk::Log()
{
	boost::shared_ptr<std::fstream> pLogFile(new std::fstream("log.txt",std::ios::out|std::ios::app));
	return lg::Log<std::fstream>(pLogFile,"PakFile");
}

std::ostream &Header::Print(std::ostream &os) const
{
	os<<"Header dump { IsValid: "<<IsValid()<<
		" Version #: "<<version<<
		" nEntries: "<<nEntries<<
		" FATbegin: "<<FATBegin()<<
		" FATend: "<<FATEnd()<<
		" File Size: "<<fileSize<<" }";
	return os;
}
std::ostream &pk::operator <<(std::ostream &os,const pk::Header &o)
{
	return o.Print(os);
}

std::ostream &Entry::Print(std::ostream &os) const
{
	os<<"Entry dump { IsValid: "<<IsValid()<<
		" Start: "<<Start()<<
		" End: "<<End()<<
		" Size: "<<Size()<<
		" ID: "<<id<<" }";
	return os;
}

std::ostream &pk::operator <<(std::ostream &os,const pk::Entry &o)
{
	return o.Print(os);
}

class PrintEntry
{
public:
	PrintEntry(std::ostream &os_) : os(os_) {};
	void operator()(const Entry &e) { os<<e; };
	std::ostream &os;
};

std::ostream &PakFile::Print(std::ostream &os) const
{
	os<<"PakFile dump { Header: "<<header<<
		" Entries: { ";
	PrintEntry p(os);
	std::for_each(FAT.begin(),FAT.end(),p);
	os<<" } }";
	return os;
}
std::ostream &pk::operator <<(std::ostream &os,const pk::PakFile &o)
{
	return o.Print(os);
}

void PakFile::Open(const std::string& path)
{
	// Open PakFile
	fs.open(path.c_str(),std::ios::binary|std::ios::in);
	if (!fs.is_open())
		throw ex::open_failed_error(Log(),std::string("Cannot open pack file: ")+path);
	// Read header
	iol::Read(fs,header);
	// Verify header
	if (!header.IsValid())
		throw ex::corrupt_data(Log(),std::string("Header in invalid for pack file: ")+path);
	// read table entries
	uint32 count = header.nEntries;
	FAT.reserve(count);
	
	std::string fname; // buffer for file names
	while (count--)
	{
		// read entry file name length in bytes
		uint32 nameLen=0;
		iol::Read(fs,nameLen);
		// read entry file name
		fname.clear();
		fname.reserve(nameLen);
		iol::Read(fs,std::back_inserter(fname),nameLen);
		// add entry to FATMap
		Entry e(fname); // set id
		// read entry data into FATMap
		iol::Read(fs,e.location);
		iol::Read(fs,e.size);
		// add completed entry to vector
		FAT.push_back(e);
	}
};
pk::Entry PakFile::GetEntry(const std::string &path)
{
	typedef FAT_t::iterator it_t;
	std::pair<it_t,it_t> result=std::equal_range(FAT.begin(),FAT.end(),Entry(path));
	if (result.first!=result.second)
		return Entry(*result.first);
	return Entry(path);
}
// { return std::equal_range(FAT.begin(),FAT.end(),); }

std::streamsize ReadPK(ios_char_type *s,std::streamsize n,const Entry &entry,uint32 &pos,std::fstream &fs)
{
	// if at or past end, return EOF
	if (pos>=entry.Size())
		return -1;
	// prevent overruns
	n=std::min(n,static_cast<std::streamsize>(entry.Size()-pos));
	// seek to file position
	fs.seekg(pos+entry.Start());
	// read n bytes
	fs.read(s,n);
	// increment read position
	pos+=n;
	// return # of bytes copied
	return n;
}

std::streamsize WritePK(const ios_char_type *s,std::streamsize n,const Entry &entry,uint32 &pos,std::fstream &fs)
{
	// if at or past end, return EOF
	if (pos>=entry.Size())
		return -1;
	// prevent overruns
	n=std::min(n,static_cast<std::streamsize>(entry.Size()-pos));
	// seek to file position
	fs.seekg(pos+entry.Start());
	// read n bytes
	fs.write(s,n);
	// increment read position
	pos+=n;
	// return # of bytes copied
	return n;
}

io::stream_offset SeekPK(io::stream_offset off, std::ios_base::seekdir way,uint32 &pos,const Entry &entry)
{
	// Determine new value of pos
	if (way == std::ios_base::beg)
		pos = off;
	else if (way == std::ios_base::cur)
		pos += off;
	else if (way == std::ios_base::end)
		pos = entry.Size() + off - 1;
	// Check for errors
	if (pos < 0 || pos > entry.Size())
	{
		pos=0;
		throw std::ios_base::failure("bad seek offset");
	}
	return pos;
}

std::streamsize IS::read(char_type *s, std::streamsize n)
{
	return ReadPK(s,n,entry,pos,pkf.fs);
}

io::stream_offset IS::seek(io::stream_offset off, std::ios_base::seekdir way)
{
	return SeekPK(off,way,pos,entry);
}

std::streamsize OS::write(const char_type *s, std::streamsize n)
{
	return WritePK(s,n,entry,pos,pkf.fs);
}

io::stream_offset OS::seek(io::stream_offset off, std::ios_base::seekdir way)
{
	return SeekPK(off,way,pos,entry);
}

std::streamsize IOS::write(const char_type *s, std::streamsize n)
{
	return WritePK(s,n,entry,pos,pkf.fs);
}

std::streamsize IOS::read(char_type *s, std::streamsize n)
{
	return ReadPK(s,n,entry,pos,pkf.fs);
}

io::stream_offset IOS::seek(io::stream_offset off, std::ios_base::seekdir way)
{
	return SeekPK(off,way,pos,entry);
}