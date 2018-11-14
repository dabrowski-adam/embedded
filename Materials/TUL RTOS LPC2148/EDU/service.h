#ifndef SERVICE_H
#define SERVICE_H

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"


class Service{
protected:
	char                 Name[configMAX_TASK_NAME_LEN];
	unsigned portSHORT   StackDepth;
	portBASE_TYPE        Priority;
	xTaskHandle          FreeRTOS_Handle;
public:
	Service();
	~Service(){};
	virtual void Body();
	void Run();
};

#endif
