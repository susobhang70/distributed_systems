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

result *rls_1_svc(input *cmd, struct svc_req *X)
{
	string command = string("ls ");
	command = command + cmd->command;

	cout << "Received Command: " << command << endl;

	string output = exec(command.c_str());
	result* out = new result;
    // result* out = (result*) malloc(sizeof(result));

	strcpy (out->output, output.c_str());

	cout << "Result of query:\n" << out->output << endl;

    return(out);
}
