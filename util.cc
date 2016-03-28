#include "util.h"
#include <codecvt>
#include <ctime>

void msg_q(string msg, int severity)
{
	chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();
	time_t tnow = chrono::system_clock::to_time_t(now);
	
	string stime(ctime(&tnow));

	if(severity == 0)
	{
		cout << "\x1B[32m[" << stime.substr(0, stime.length()-1) <<"][コンソール]\x1B[36m " << msg << endl; 
	}
	else if(severity == 3)
	{
		cout << "\x1B[32m[" << stime.substr(0, stime.length()-1) <<"]\x1B[31m[クリティカル] " << msg << endl;
	}
	else if(severity >= 4)
	{
		cout << "\x1B[32m[" << stime.substr(0, stime.length()-1) <<"]\x1B[31m[クリティカル] " << endl; 
		cerr << msg;
		exit(1);
	}
}

string uniconv(wstring toconv)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>,wchar_t> convert; // http://en.cppreference.com/w/cpp/locale/wstring_convert/to_bytes
	string n = convert.to_bytes(toconv);
	return n; 
}