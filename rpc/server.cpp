#include <iostream>
#include <rpc/rpc.h>
#include "prog.h"
#include <cstdlib>
#include <string>
#include <cstring>

using namespace std;

string exec(const char* cmd)
{
	char buffer[128];
	string result = "";
	FILE* pipe = popen(cmd, "r");
	if (!pipe)
	{
		perror("popen() failed!");
		exit(1);
	}
	try
	{
		while (!feof(pipe))
		{
			if (fgets(buffer, 128, pipe) != NULL)
				result += buffer;
		}
	}
	catch (...)
	{
		pclose(pipe);
		exit(1);
	}
	pclose(pipe);
	return result;
}

buffer *rls_1_svc(buffer *cmd, struct svc_req *X)
{
	string command = string("ls ");
	command = command + cmd->data;

	cout << "Received Command: " << command << endl;

	string output = exec(command.c_str());

	buffer* result = (buffer*) malloc(sizeof(buffer));
	char c[10000];
	strcpy (c, output.c_str());
	result->data = c;

	cout << "Result of query:\n" << result->data << endl;

	return(result);
}
