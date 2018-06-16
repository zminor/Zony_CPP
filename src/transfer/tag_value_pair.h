#ifndef TAG_VALUE_PAIR
#define TAG_VALUE_PAIR

#include <string>
using namespace std;
class Tag_value_pair
{
public:
	Tag_value_pair(){}
	virtual ~Tag_value_pair(){}
	Tag_value_pair(string tag, string value):m_tag(tag),m_value(value)
				{
				}
	string Get_tag() const
	{
		return m_tag;
	}

	void Set_tag(string value)
	{
		m_tag = value;
	}
	string Get_value() const
	{
		return m_value;
	}
	void Set_value(string value)
	{
		m_value= value;
	}
private:
	string m_tag;
	string m_value;
};
#endif
