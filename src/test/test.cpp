#include <string>
#include "tag_value_pair.h"
#include "http_header.h"
#include <iostream>
using namespace std;
int main(int argc, char* argv[])
{
	Http_header * h = new Http_header();
	h->Add_tag_value("ff", "fk");
	string s;
	h->Get_value("ff",s);
	std::cout << s << std::endl;
	return 0;
}
