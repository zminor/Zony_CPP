#include <iostream>
#include "dbg.h"
#include "server.h"

using namespace std;

int main(int argc, char *argv[])
{
	Server server;
	server.run();
	return 0;
}
