#include "Service.h"


/* Standard includes. */
#include <stdlib.h>
#include <string.h>



char def_Name[] ="A";

void dummy_Create (void * pvParameters);
void dummy_Create (void * pvParameters)
{
	((Service*)pvParameters) -> Body();
}

Service::Service()
{
	strcpy (def_Name, Name);
	StackDepth = configMINIMAL_STACK_SIZE;
	Priority = tskIDLE_PRIORITY;
	FreeRTOS_Handle = NULL;
	def_Name[0] += 1; // ¿eby kolejne nazwy by³y "A", "B", "C", ...
}

void Service::Body()
{
	for ( ;; )
	{
		vTaskDelay(1000000); // Znaczy nic nie robi, ale nie pozwala siê skoñczyæ, co jest wa¿ne.
	}
}


void Service::Run()
{
	xTaskCreate (dummy_Create,
	             (signed char *)Name,
				 StackDepth,
				 (void*)this,
				 Priority,
				 &FreeRTOS_Handle);
}
