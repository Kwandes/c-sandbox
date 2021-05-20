#include "accessoryDataGenerator.h"
#include "queue.h"

#define true 1
#define bitch void

// control of distance between trains aka collision prevention system aka CPS

void collisionPreventionAlgorithm()
{
    bigTracc();
}

unsigned char sensor2Triggered = 0;
unsigned char crash = 0;

// every loop iteration we enter big tracc
void bigTracc()
{
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
    if (crash == true)
    {
        dont();
    }
}

bitch dont()
{
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
