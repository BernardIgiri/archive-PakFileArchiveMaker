// pak file.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "fileio.h"
#include "algs.h"
#include "PakFileMaker.h"
#include "pngloader.h"


class PrintAny
{
public:
	PrintAny(){};
	~PrintAny(){};
	template<typename T>
	void operator()(const T& t) { std::cout<<t<<std::endl; };
};

namespace main
{
	lg::Log<std::fstream> Log()
	{
		boost::shared_ptr<std::fstream> pLogFile(new std::fstream("log.txt",std::ios::out|std::ios::app));
		return lg::Log<std::fstream>(pLogFile,"Main");
	}
};

int _tmain(int argc, char ** argv)
{
	int i=0;
	std::string pathStr("../file this/");
	io::fs::path p(pathStr);
	
	io::MatchAll pred;

	std::vector<std::string> v;
	io::FindFiles(p,pred,std::back_inserter(v),true);
	std::cout<<"The files matching extension: *.* are:"<<std::endl;
	std::for_each(v.begin(),v.end(),PrintAny());
	std::cout<<v.size()<<" files found."<<std::endl;
	std::cout<<"Building PakFile..."<<std::endl;
	std::transform(v.begin(),v.end(),v.begin(),std::bind1st(std::plus<std::string>(), pathStr));
	pk::PakFileMaker()("PK File",1000,v.begin(),v.end());

	pk::PakFile pkf("PK File");

	pk::IS is(pkf);
	is.open("../file this/pngtest.png");
	bmp::Bitmap bitmap;
	image_loader::PNGLoad<pk::IS>(is,bitmap);

	std::ofstream test("../test.png",std::ios::binary|std::ios::out|std::ios::trunc);
	image_loader::PNGSave<std::ofstream>(test,bitmap);
	test.close();

	main::Log().Message()<<pkf<<lg::endl();
	ex::io_error(main::Log(),p,"test error");
	/*std::ofstream test("../test.jpg",std::ios::binary|std::ios::out|std::ios::trunc);

	const uint32 MAXBUFF = 4096;
	char buff[MAXBUFF]; // file copy bufferosition of FAT

	pk::CopyFile(pkf.GetEntry("../file this/h.jpg").Size(),is,test,buff,MAXBUFF);*/

	return 0;
}