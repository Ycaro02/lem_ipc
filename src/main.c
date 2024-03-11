# include "../include/lem_ipc.h"

int main(int argc, char **argv) 
{
	(void) argc;	
	int shid = init_shared_memory(argv[0]);
	ft_printf_fd(1, "Test main ok, id %d\n", shid);
	int ret = destroy_shared_memory(shid);
	return (ret);
}