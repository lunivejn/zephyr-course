#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static int new_board_init(void)
{
	printk("Board Initialized\n");
	return 0;
}

SYS_INIT(new_board_init, APPLICATION, 0);