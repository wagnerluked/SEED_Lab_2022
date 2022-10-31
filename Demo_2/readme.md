Demo 2 integrates the controller with the Pi Camera.

The goal is to search for and move to an Aruco marker between 3 and 5 feet away from the robot. 
There will be two types of trials:
    1. In the first type, the robot must rotate to find the marker, drive to the marker, and then stop within 1 foot of the
        marker. If the robot does not start to move within 30 seconds of the go signal, the trial counts as a failure.
    2. In the second type, the robot must rotate to find the marker, drive to the marker, and stop so that the center of
        rotation of the robot lies at the marker position. The maneuver is a success if the marker lies between the robot
        wheels when the robot stops.
        
