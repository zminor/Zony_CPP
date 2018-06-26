#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <vector>

namespace Utils
{
	void toLower(std::string &str) noexcept;
	std::string getLowerString(const std::string &str);

	void trim(std::string &str);
	std::string getTrimmedString(const std::string &str);

	std::vector<std::string> explode(const std::string &str, const char sep);
	std::string getUniqueName();

	std::string urlEncode(const std::string &str);
	std::string urlDecode(const std::string &str);



}
#endif
