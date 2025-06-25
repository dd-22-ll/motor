#include "App_Task.h"

void         startTask(void *);
TaskHandle_t startTaskHandle;

void         commu(void *);
TaskHandle_t commuTaskHandle;

void         keyScan(void *);
TaskHandle_t keyScanTaskHandle;

void         display(void *);
TaskHandle_t displayTaskHandle;

void         motorControl(void *);
TaskHandle_t motorControlTaskHandle;

/* Initialize FreeRTOS */
void App_Task_StartFreeRTOS(void)
{
    debug_printfln("Starting FreeRTOS...");

    /* 1. Create a startup task: It will initialize the other application modules */
    BaseType_t r = xTaskCreate(startTask,
                               "startTask",
                               256,
                               NULL,
                               10,
                               &startTaskHandle); /* Store task handle */
    if(r)
    {
        debug_printfln("Startup task created successfully");
    }
    else
    {
        debug_printfln("Failed to create startup task");
    }

    /* 2. Start the scheduler */
    vTaskStartScheduler();

    /* The following code should not be executed */
    debug_printfln("Execution continued unexpectedly...");
}

/* Startup task function */
void startTask(void *args)
{
    debug_printfln("Startup task is running...");

    BaseType_t result;

    /* Initialize motor module */
    App_MotorControl_Start();

    /* Initialize display module */
    App_Display_Start();

    // Start communication
    App_Commu_Start();

    xTaskCreate(commu,
                "commuTask",
                256,
                NULL,
                8,
                &commuTaskHandle);

    result = xTaskCreate(keyScan,
                "keyScanTask",
                256,
                NULL,
                9,
                &keyScanTaskHandle);
    if(result != pdPASS)
    {
        debug_printfln("Failed to create keyScan task");
    }

    result = xTaskCreate(display,
                "displayTask",
                256,
                NULL,
                7,
                &displayTaskHandle);
    if(result != pdPASS)
    {
        debug_printfln("Failed to create display task");
    }

    xTaskCreate(motorControl,
                "motorControl",
                256,
                NULL,
                8,
                &motorControlTaskHandle);
    if(result != pdPASS)
    {
        debug_printfln("Failed to create motorControl task");
    }
    /* Delete startup task: self-delete */
    vTaskDelete(NULL); /* NULL means delete self */
}

/* Communication task */
void commu(void *args)
{
    vTaskDelay(100);
    debug_printfln("Communication task is running...");
    TickType_t lastWakeTime = xTaskGetTickCount();
    while(1)
    {
        App_Commu_RecvModbusData();
        vTaskDelayUntil(&lastWakeTime, 5);
    }
}

void motorControl(void *args)
{
    debug_printfln("motorControl task is running...");
    TickType_t lastWakeTime = xTaskGetTickCount();
    uint8_t    dt           = 50;
    while(1)
    {
        /* 1. Get actual motor RPM */
        int16_t realRPM = App_MotorControl_GetRealRPM(dt / 1000.0);

        /* 2. Perform PID calculation */
        App_MotorControl_PIDCacl(realRPM, dt);

        App_MotorControl_MotorRun();

        vTaskDelayUntil(&lastWakeTime, dt);
    }
}

/* Key scan task */
void keyScan(void *args)
{
    debug_printfln("keyScan task is running...");
    TickType_t lastWakeTime = xTaskGetTickCount();
    while(1)
    {
        App_MotorControl_KeyScan();
        vTaskDelayUntil(&lastWakeTime, 50);
    }
}

void display(void *args)
{
    debug_printfln("display task is running...");
    TickType_t lastWakeTime = xTaskGetTickCount();

    while(1)
    {
        App_Display_Show();
        vTaskDelayUntil(&lastWakeTime, 20);
    }
}

