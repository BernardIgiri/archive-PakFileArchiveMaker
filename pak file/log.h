#pragma once

namespace lg
{
	class endl
	{
	};

	template <typename Stream>
	Stream& operator << (Stream& os,const endl& end) { os<<")>-"<<std::endl;return os; };
	template <typename Stream>
	class Log
	{
	public:
		Log(boost::shared_ptr<Stream> pOutputStream,const std::string ownerLabel) : ownerLbl(ownerLabel), pLS(pOutputStream) { };
		~Log() { pLS->flush(); }; // writes output to log

		// returns stream for logging errors
		Stream &Error(void) { (*pLS)<<"-<("<<ownerLbl<<"-Error: ";pLS->flush();return *pLS; };
		// returns stream for logging warnings
		Stream &Warning(void) { (*pLS)<<"-<("<<ownerLbl<<"-Warning: ";pLS->flush();return *pLS; };
		// returns stream for logging messages
		Stream &Message(void) { (*pLS)<<"-<("<<ownerLbl<<"-Message: ";pLS->flush();return *pLS; };

	private:
		boost::shared_ptr<Stream> pLS; // pointer to stream for log output
		std::string ownerLbl; // string label for owner of log
	};
};