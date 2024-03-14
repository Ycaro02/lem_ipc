# include "../include/lem_ipc.h"

/**
 *	@brief Print an error message and the error string of a syscall
 *	@param syscall_name The name of the syscall
*/
void syscall_perror(char *syscall_name)
{
	ft_printf_fd(2, "%s failed\n", syscall_name);
	perror(syscall_name);
}
