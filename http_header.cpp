#include "http_header.h"

Http_header::Http_header()
{
}

Http_header::Http_header(char * buffer)
{
		Set_http_header(buffer);
}
Http_header::~Http_header()
{
		Remove_all_tags();
}


void Http_header::Remove_tag_value(const string &tag)
{
				if(m_tag_map.find(tag) == m_tag_map.end()) return;
				m_tag_map.erase(tag);
				m_tags.erase(m_tags.begin()+ m_tag_map[tag]);
}

bool Http_header::Get_value(const string& tag, string & res_value) const
{
			if( m_tag_map.find(tag) == m_tag_map.end())
							return false;
			std::map <string,int>:: const_iterator iter = m_tag_map.find(tag);
		res_value = m_tags[	iter->second	].Get_value();
			return true;
}
void Http_header::Add_tag_value(string tag, string value)
{
		Tag_value_pair pair;
		pair.Set_tag(tag);
		pair.Set_value(value);

		//release previous one
		Remove_tag_value(tag);
		m_tags.push_back(pair);
		m_tag_map.insert(std::pair<string,int>(tag, m_tags.size()-1));
}
void Http_header::Set_first_line(const string& line)
{
				m_first_line = line;
}
const string Http_header::Get_first_line() const
{
				return m_first_line;
}
void Http_header::Remove_all_tags()
{
				m_tag_map.clear();
				m_tags.clear();
}

void Http_header::Set_http_header(const char* buffer)
{
				Remove_all_tags();
				if(buffer == NULL)
				{
					return;				
				}

				if((buffer[0] != '\0')&& (buffer[1]!='\0')&& (buffer[0]== '\r')&& (buffer[1] == '\n'))
				{//in case there is \r\n in front
					buffer +=2;
				}
				
				int l = strcspn(buffer, "\r\n");
				if(l==0) 
								return;

				m_first_line = string(buffer,1);

				char* line;
				string tag;
				string value;
				char* p;
				int ws =0;

				line = (char*) strstr(buffer,"\r\n");
				if(line == NULL)
								return;
				line+=2;

				while((*line != '\0')&& (*line != '\r') && (*(line +1)!= '\n'))
				{
					p = strchr(line , ':');
					if(p == NULL) break;
					l = strcspn(line, ": \t");
					tag = string(line,1);
					transform(tag.begin(),tag.end(),tag.begin(),::tolower);
					//set line pointer behind the colon and skip whitespace
					line = p+1;
					ws = strspn(line, " \t");
					line += ws;

					for(;;)
					{
						p = strstr(line , "\r\n");
						if(p == NULL) break;
						if((*(p+2)!= ' ')&& (*(p+2)!= '\t'))//end of value found
						p +=2;
					}
					if(p == NULL) 
									break;
					value = string(line, (int)(p-line));
					Add_tag_value(tag,value);
					line = p+2;
				}
}

