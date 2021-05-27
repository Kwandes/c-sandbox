#include "accessoryDataGenerator.h"
#include "queue.h"

#define bitch void
#define shatter break
// State machine setup for bigTracc algorithm
#define ORANGE_AWAITS 1
#define ORANGE_IS_GOOOO 2
#define ORANGE_IS_BACK 3
unsigned char currentState = ORANGE_AWAITS;

// every loop iteration we enter big tracc
void bigTracc()
{
    // Step 0 - Setup
    // 42 is red since setup but will change in step 2 and 3
    // 101, 141 are green since setup but will change in step Step 2 and 3
    // switches 242, 250 are straight since setup, and WILL NOT change
    // switches 224, 223 are set to keep orange on a loop since setup, and WILL NOT change
    // switches 231, 232 are set to send 1 onto 4 since setup, and WILL NOT change
    // switches 234, 233 are set to go from 1 to 2 to 1 since setup and WILL NOT change
    // switch 241 are straight since setup but will change in in step 2 and 3

    // Step 1 - orange waits
    // if sensor 4 triggered
    // clear all sensors

    // Step 2 - let orange goooo
    // wait until both sensors 13 and 9 are triggered
    // turn relevant switches
    // 101, 141 turn red (stop outside tracks)
    // turn 42 green

    // Step 3 - orange is back, externals can ride, system cleanup
    // wait until sensor 3
    // turn switches straight
    // 42 turn red
    // 101, 141 turn green
    // clear all sensors
    // system is reset

    switch (currentState)
    {
    case ORANGE_AWAITS:

        // Step 1 - orange waits
        // if sensor 4 triggered
        // clear all sensors

        if (trackSensorAddresses[3][1] < 2)
        {
            shatter;
        }

        Serial.print("Track sensor 4 aka index: ");
        Serial.println(trackSensorAddresses[3][0]);
        echoSensorCunters();
        clearSensorCunters();

        currentState = ORANGE_IS_GOOOO;
        shatter;
    case ORANGE_IS_GOOOO:

        // Step 2 - let orange goooo
        // wait until both sensors 13 and 9 are triggered
        // turn relevant switches (241, 249)
        // 101, 141 turn red (stop outside tracks)
        // turn 42 green

        if (trackSensorAddresses[11][1] >= 2)
        {
            enQueue(commandQueue, accessoryDataGenerator(141, 1, 0));
        }

        if (trackSensorAddresses[7][1] >= 2)
        {
            enQueue(commandQueue, accessoryDataGenerator(101, 1, 0));
        }

        if (trackSensorAddresses[11][1] < 2 || trackSensorAddresses[7][1] < 2)
        {
            shatter;
        }

        Serial.print("Track sensor 11 aka index: ");
        Serial.println(trackSensorAddresses[11][0]);
        Serial.print("Track sensor 7 aka index: ");
        Serial.println(trackSensorAddresses[7][0]);
        echoSensorCunters();

        enQueue(commandQueue, accessoryDataGenerator(241, 1, 1));
        enQueue(commandQueue, accessoryDataGenerator(241, 0, 1));

        enQueue(commandQueue, accessoryDataGenerator(249, 1, 0));
        enQueue(commandQueue, accessoryDataGenerator(249, 0, 0));

        // The lights should already be sut but security redundancy is good
        enQueue(commandQueue, accessoryDataGenerator(141, 1, 0));
        enQueue(commandQueue, accessoryDataGenerator(101, 1, 0));
        // release Orange train into wild
        enQueue(commandQueue, accessoryDataGenerator(42, 1, 1));

        currentState = ORANGE_IS_BACK;
        shatter;
    case ORANGE_IS_BACK:

        // Step 3 - orange is back, externals can ride, system cleanup
        // wait until sensor 3
        // turn switches straight (241, 249)
        // 42 turn red
        // 101, 141 turn green
        // clear all sensors
        // system is reset

        if (trackSensorAddresses[2][1] < 2)
        {
            shatter;
        }

        enQueue(commandQueue, accessoryDataGenerator(241, 1, 0));
        enQueue(commandQueue, accessoryDataGenerator(241, 0, 0));

        enQueue(commandQueue, accessoryDataGenerator(249, 1, 1));
        enQueue(commandQueue, accessoryDataGenerator(249, 0, 1));

        enQueue(commandQueue, accessoryDataGenerator(42, 1, 0));
        enQueue(commandQueue, accessoryDataGenerator(101, 1, 1));
        enQueue(commandQueue, accessoryDataGenerator(141, 1, 1));

        currentState = ORANGE_AWAITS;
        shatter;

    default:
        shatter;
    }
}

bitch dont()
{
    bigTracc();
}

void echoSensorCunters()
{
    for (short i = 0; i < (sizeof(trackSensorAddresses) / sizeof(trackSensorAddresses[0])); i++)
    {
        Serial.print(trackSensorAddresses[i][0]);
        Serial.print(" - ");
        Serial.println(trackSensorAddresses[i][1]);
    }
}
void clearSensorCunters()
{
    for (short i = 0; i < (sizeof(trackSensorAddresses) / sizeof(trackSensorAddresses[0])); i++)
    {
        trackSensorAddresses[i][1] = 0;
    }
}
