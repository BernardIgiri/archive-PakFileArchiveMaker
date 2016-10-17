#pragma once
#include "log.h"

namespace ex
{
	class error : std::exception
	{
	public:
		error() {};
		template<typename LOG>
		error(LOG &log,std::string msg_) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		error(LOG &log,T &t,std::string msg_) { Log(*this,log,t,msg); };
		virtual const std::string &Msg() const { return msg; }; // returns error message
		virtual const char *ExName() { return "general fault"; }; // name of exception
	protected:
		template<typename LOG>
		void Log(error& e,LOG &log,std::string &msg_)
		{ msg=msg_;log.Error()<<e.ExName()<<": "<<msg<<lg::endl(); };
		template<typename LOG,typename T>
		void Log(error& e,LOG &log,T &t,std::string &msg_)
		{ msg=msg_;log.Error()<<e.ExName()<<": "<<msg<<t<<lg::endl(); };
		std::string msg; // error message
	};

	class out_of_memory : public error
	{
	public:
		out_of_memory() {};
		template<typename LOG>
		out_of_memory(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		out_of_memory(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "out of memory"; }; // name of exception
	};

	class logic_error : public error
	{
	public:
		logic_error() {};
		template<typename LOG>
		logic_error(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		logic_error(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "logic error"; }; // name of exception
	};

	class invalid_input : public logic_error
	{
	public:
		invalid_input() {};
		template<typename LOG>
		invalid_input(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		invalid_input(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "invalid input"; }; // name of exception
	};
	
	class value_out_of_bounds : public invalid_input
	{
	public:
		value_out_of_bounds() {};
		template<typename LOG>
		value_out_of_bounds(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		value_out_of_bounds(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "value out of bounds"; }; // name of exception
	};

	class expected_object : public invalid_input
	{
	public:
		expected_object() {};
		template<typename LOG>
		expected_object(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		expected_object(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "expected object"; }; // name of exception
	};

	class feature_not_found : public logic_error
	{
	public:
		feature_not_found() {};
		template<typename LOG>
		feature_not_found(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		feature_not_found(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "required feature not found"; }; // name of exception
	};

	class io_error : public error
	{
	public:
		io_error() {};
		template<typename LOG>
		io_error(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		io_error(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "i/o error"; }; // name of exception
	};

	class read_error : public io_error
	{
	public:
		read_error() {};
		template<typename LOG>
		read_error(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		read_error(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "read error"; }; // name of exception
	};

	class write_error : public io_error
	{
	public:
		write_error() {};
		template<typename LOG>
		write_error(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		write_error(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "write error"; }; // name of exception
	};

	class read_only_error : public write_error
	{
	public:
		read_only_error() {};
		template<typename LOG>
		read_only_error(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		read_only_error(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "write attempt to read only data"; }; // name of exception
	};

	class open_failed_error : public io_error
	{
	public:
		open_failed_error() {};
		template<typename LOG>
		open_failed_error(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		open_failed_error(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "open failed"; }; // name of exception
	};

	class file_not_found : public open_failed_error
	{
	public:
		file_not_found() {};
		template<typename LOG>
		file_not_found(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		file_not_found(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "file not found"; }; // name of exception
	};

	class eof_error : public io_error
	{
	public:
		eof_error() {};
		template<typename LOG>
		eof_error(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		eof_error(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "end of file reached"; }; // name of exception
	};

	class disk_full : public io_error
	{
	public:
		disk_full() {};
		template<typename LOG>
		disk_full(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		disk_full(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "disk drive is full"; }; // name of exception
	};

	class access_denied : public io_error
	{
	public:
		access_denied() {};
		template<typename LOG>
		access_denied(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		access_denied(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "access to resource denied"; }; // name of exception
	};

	class corrupt_data : public io_error
	{
	public:
		corrupt_data() {};
		template<typename LOG>
		corrupt_data(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		corrupt_data(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "corrupt stream data"; }; // name of exception
	};

	class unrecognized_file_format : public io_error
	{
	public:
		unrecognized_file_format() {};
		template<typename LOG>
		unrecognized_file_format(LOG &log,std::string msg) { Log(*this,log,msg); };
		template<typename LOG,typename T>
		unrecognized_file_format(LOG &log,T &t,std::string msg) { Log(*this,log,t,msg); };
		virtual const char *ExName() { return "unrecognized file format"; }; // name of exception
	};
};