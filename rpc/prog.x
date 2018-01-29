struct buffer {
	string data<>;
};

program RLSPROG { 
    version RLSVERS {
		buffer RLS(buffer) = 1;
    } = 1; /*Verison of RPC being used -> used to name functions*/
} = 0x20000008; /*Unique identification of the server*/
