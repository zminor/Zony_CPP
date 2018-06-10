#ifndef HTTP_HEADER
#define HTTP_HEADER

#include <string>
#include <vector>
#include <map>
#include "tag_value_pair.h"
#include <string.h>
#include <ctype.h>
#include <algorithm>

using namespace std;
class Http_header
{
public:
	Http_header();
	Http_header(char* buffer); //UTF-8 based stream
	virtual ~Http_header();

	void Remove_tag_value(const string &tag);

	bool Get_value(const string& tag, string & res_value)const ;
	void Add_tag_value(string, string);
	void Set_first_line(const string& line);

	const string Get_first_line() const;

	const std::vector<Tag_value_pair> & Get_tag_values() const
	{
		return m_tags;
	}

	void Remove_all_tags();
	void Set_http_header(const char* buffer);
private:
	string m_first_line;
	std::vector<Tag_value_pair> m_tags;
	std::map <string, int> m_tag_map;
};


#endif
