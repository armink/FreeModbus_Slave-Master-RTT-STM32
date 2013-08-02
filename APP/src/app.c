#include "app_task.h"

int main(void)
  {
	rt_uint32_t UNUSED level;

	/* disable interrupt first */
	level = rt_hw_interrupt_disable();

	/* init system setting */
	SystemInit();

	/* startup RT-Thread RTOS */
	rtthread_startup();

	return 0;
}


