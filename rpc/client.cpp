#include <iostream>
#include <rpc/rpc.h>
#include <unistd.h>
#include <string>
#include "prog.h"

using namespace std;

int main(int argc, char *argv[])
{
    CLIENT *Client;
    input cmd;
    result *res;
    char host[256];

    gethostname(host, 256);
    Client = clnt_create(host, RLSPROG, RLSVERS, "udp"); //Create a handle for Client
    
    if(Client == NULL)
    {
        cout << "Error: Client not formed" << endl;
        exit(-1);
    }

	string command = string("");
	for (int i = 1; i < argc; i++)
		command = command + " " + argv[i];

	strcpy (cmd.command, command.c_str());

    res = rls_1(&cmd, Client);
    cout << "Result of rls:\n" << res->output << endl;
    return 0;
}
