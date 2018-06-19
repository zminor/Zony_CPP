#include "System.h"

namespace System{

native_processid_type getProcessId() noexcept
{

}

bool sendSignal( const native_processid_type pid, const int signal) noexcept
{

}

void filterSharedMemoryName( std::string & memName)
{
	std::wstring_convert <std::codecvt_utf8_utf16 <wchar_t> > convertor;	
	std::wstring memory_name = converter.from_bytes(memName);
	const std::wstring file_ext = L".exe";   

	const size_t pos = memory_name.rfind(file_ext);	//Reverse 1st found index ,return index from beginning

	if(pos == (memory_name.length() - file_ext.length()) )
	{
		memory_name.erase(
			pos,
			file_ext.length()		//memory_name.length()
			);
	}

	::TCHAR buf[MAX_PATH +1] {};
	::GetFullPathName(memory_name.c_str(),MAX_PATH, buf, nullptr);
	//Insert '/' at front of memName
	if(memName.front() != '/')
	{
		memName = '/' + memName;
	}
	//Replace '/' or '\\' with '-'
	for(size_t i = 1; i< memName.length(); ++i )
	{
		if('/' == memName[i] || '\\' == memName[i])
		{
			memName[i] = '-';
		}
	}
}
}
