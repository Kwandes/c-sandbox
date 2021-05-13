#include "accessoryDataGenerator.h"

// control of distance between trains aka collision prevention system aka CPS
// the system only works with two trains on the track, and the track having a switch for a separate, longer route
unsigned long timeBetweenTrains = 0; // time between detections
unsigned long lastDetectionTime = 0;
unsigned long minimumDistanceBetweenTrains = 3000; // 5000 miliseconds aka 5 seconds
unsigned int isTimeBetweenTrainsDecreasing = 0;    // bool - true if the trains keep taking less time between each reading set,aka get closer to each other
unsigned int isFirstTrain = 1;                     //
unsigned int setSwitchForLongerRoute = 0;          // if true, the switch is set for the longer route
unsigned long timeBetween = 0;

unsigned long time1 = 0;
unsigned long time2 = 0;
unsigned int diff1 = 0;
unsigned int diff2 = 0;
unsigned int oldDiff1 = 0;
unsigned int oldDiff2 = 0;

void semiWorking()
{
    // only triggered on the first time the trains pass the sensor
    if (lastDetectionTime == 0)
    {
        lastDetectionTime = millis();
        return;
    }

    // if the first train passes and the time between the train is decreasing, send the second train onto the longer track
    if (isFirstTrain == 1)
    {
        Serial.println("First train");

        if (isTimeBetweenTrainsDecreasing == 1)
        {
            // Set trains to be on different tracks
            setSwitchForLongerRoute = 1;
            Serial.println("Setting the route to be longer");
            // enQueue message to change switches after a specific delay
        }
        else
        {
            // send both trains on the same track
            setSwitchForLongerRoute = 0;
            Serial.println("Setting the route to be shorter");
            // enQueue message for the shorter route
        }
    }
    else if (isFirstTrain == 0) // if the second train is passing, calculate the distance
    {
        Serial.println("The second train");
        setSwitchForLongerRoute = 0;

        Serial.print("Millis: ");
        Serial.println(millis());

        Serial.print("Time between trains: ");
        Serial.println(timeBetweenTrains);

        Serial.print("Millis - Last detection time: ");
        Serial.println(millis() - lastDetectionTime);

        // decide if the second train needs to be slowed down
        if (timeBetweenTrains < (millis() - lastDetectionTime))
        {
            Serial.println("The distance is decreasing!");
            isTimeBetweenTrainsDecreasing = 1; // true
        }
        else
            Serial.println("The distance is increasing");
        isTimeBetweenTrainsDecreasing = 0; //false
    }

    // flip which train will we detected by the next interrupt
    isFirstTrain = !isFirstTrain;
    // reset the timers
    timeBetweenTrains = millis() - lastDetectionTime;
    lastDetectionTime = millis();

    // if the distance between trains is too short, hardstop both (emergency)
    if (timeBetweenTrains < minimumDistanceBetweenTrains)
    {
        Serial.println("The trains are too close. EMERGENCY STOP!");
        triggerEmergencyStop();
    }
}
void collisionPreventionAlgorithm()
{
    //semiWorking();
    //advancedVersion();
    simpleVersion();
}

unsigned short bitch = 0;

void simpleVersion()
{
    Serial.print("Bitch is ");
    Serial.println(bitch);
    Serial.println("Changing the switch");
    enQueue(commandQueue, accessoryDataGenerator(102, 1, bitch));
    enQueue(commandQueue, accessoryDataGenerator(102, 0, bitch));

    Serial.print("waiting ");
    Serial.print(DELAY_VALUE);
    Serial.println(" seconds");
    delay(DELAY_VALUE);
    Serial.println("Stopping bitch");
    // stop the train that just got on
    if (SPEED_VALUE > 11)
    {
        enQueue(commandQueue, ((locoAddresses[bitch] << 8) | 0x61));
    }
    else
    {
        enQueue(commandQueue, ((locoAddresses[bitch] << 8) | 0x60));
    }

    bitch = !bitch;
    // start the other bitch
    Serial.println("Starting the other bitch");
    enQueue(commandQueue, createSpeedCommand(locoAddresses[bitch]));
}

void advancedVersion()
{
    if (isFirstTrain == 1)
    {
        Serial.println("First train");
        time1 = millis();
        if (diff1 = 0)
        {
            return;
        }

        // once the second train has passed at least once
        if (time2 != 0)
        {
            diff2 = time2 - time1;
        }

        // changeTrack
        // start counting
    }
    else
    {
        Serial.println("Second train");
        time2 = millis();
        diff1 = time1 - time2;
    }

    if (diff2 < diff2)
    {
        Serial.println("The train order hs flipped");
        isFirstTrain = !isFirstTrain;
    }

    if (true)
    {
        Serial.print("Time1: ");
        Serial.println(time1);
        Serial.print("Time2: ");
        Serial.println(time2);
        Serial.print("Diff1: ");
        Serial.println(diff1);
        Serial.print("Diff2: ");
        Serial.println(diff2);
        Serial.print("OldDiff1: ");
        Serial.println(oldDiff1);
        Serial.print("OldDiff2: ");
        Serial.println(oldDiff2);
    }

    if (diff1 < minimumDistanceBetweenTrains)
    {
        Serial.println("The trains are too close!");
        //triggerEmergencyStop();
    }

    if (diff1 > oldDiff1)
    {
        Serial.println("The diff has increased");
    }
    else
    {
        Serial.println("The diff has decreased");
    }

    oldDiff1 = diff1;
    oldDiff2 = diff2;
    isFirstTrain = !isFirstTrain;
}