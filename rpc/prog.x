struct input {
	char command[1024];
};

struct result {
	char output[1024];
};

program RLSPROG { 
    version RLSVERS {
		result RLS(input) = 1;
    } = 1; /*Verison of RPC being used -> used to name functions*/
} = 0x20000008; /*Unique identification of the server*/
