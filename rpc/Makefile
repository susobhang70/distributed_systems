all:
	rpcgen prog.x
	g++ client.cpp prog_clnt.c prog_xdr.c -o rls
	g++ server.cpp prog_svc.c prog_xdr.c -o server
