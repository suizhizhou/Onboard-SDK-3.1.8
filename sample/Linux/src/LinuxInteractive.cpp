/*! @file LinuxInteractive.cpp
 *  @version 3.1.8
 *  @date Aug 05 2016
 *
 *  @brief
 *  Barebones interactive UI for executing Onboard SDK commands.
 *  Calls functions from the new Linux example based on user input.
 *
 *  @copyright
 *  2016 DJI. All rights reserved.
 * */

#include "LinuxInteractive.h"
#include "server.h"
extern int time_break_flag;

using namespace std;


char userInput()
{
  usleep(1000000);
  cout << endl;
  cout << "|------------------DJI Onboard SDK Interactive Sample------------|" << endl;
  cout << "|                                                                |" << endl;
  cout << "| The interactive mode allows you to execute a few commands      |" << endl;
  cout << "| to help you get a feel of the DJI Onboard SDK. Try them out!   |" << endl;
  cout << "|                                                                |" << endl;
  cout << "| Standard DJI Onboard SDK Usage Flow:                           |" << endl;
  cout << "| 1. Activate (The sample has already done this for you)         |" << endl;
  cout << "| 2. Obtain Control (The sample has already done this for you)   |" << endl;
  cout << "| 3. Takeoff                                                     |" << endl;
  cout << "| 4. Execute Aircraft control (Movement control/Missions/Camera) |" << endl;
  cout << "| 5. Return to Home/Land                                         |" << endl;
  cout << "| 6. Release Control (The sample will do this for you on exit)   |" << endl;
  cout << "|                                                                |" << endl;
  cout << "| Available commands:                                            |" << endl;
  cout << "| [a] Request Control                                            |" << endl;
  cout << "| [b] Release Control                                            |" << endl;
  cout << "| [c] Arm the Drone                                              |" << endl;
  cout << "| [d] Disarm the Drone                                           |" << endl;
  cout << "| [e] Takeoff                                                    |" << endl;
  cout << "| [f] Waypoint Sample                                            |" << endl;
  cout << "| [g] Position Control Sample: Draw Square                       |" << endl;
  cout << "| [h] Landing                                                    |" << endl;
  cout << "| [i] Go Home                                                    |" << endl;
  cout << "| [m] Move                                                        |" << endl;
  cout << "| [p] Printf Location                                            |" << endl;
  cout << "| [r] Rotate Control                                            |" << endl;
  cout << "| [j] Exit this sample                                           |" << endl;
  cout << "|                                                                |" << endl;
  cout << "| Type one of these letters and then press the enter key.        |" << endl;
  cout << "|                                                                |" << endl;
  cout << "| If you're new here, try following the usage flow shown above.  |" << endl;
  cout << "|                                                                |" << endl;
  cout << "| Visit developer.dji.com/onboard-sdk/documentation for more.    |" << endl;
  cout << "|                                                                |" << endl;
  cout << "|------------------DJI Onboard SDK Interactive Sample------------|" << endl;

  //char inputChar;
  //cin >> inputChar;
  
  sem_wait(&s);
  cout << msg[head_size] << endl;
  return msg[head_size];
  //return inputChar;
}

void interactiveSpin(CoreAPI* api, Flight* flight, WayPoint* waypointObj)
{
	
  bool userExitCommand = false;

  ackReturnData takeControlStatus;
  ackReturnData releaseControlStatus;
  ackReturnData armStatus;
  ackReturnData disArmStatus;
  ackReturnData takeoffStatus;
  ackReturnData landingStatus;
  ackReturnData goHomeStatus;
  int drawSqrPosCtrlStatus;
	int positionControlStatus;

	PositionData pos;
	sem_init(&s,0,0);
	sem_init(&ss,0,0);

	flight_head.protocol = 0x01;
	flight_head.message_length = 1000;

	c_function();

	


  while (!userExitCommand)
  {
    char getUserInput = userInput();
    switch (getUserInput)
    {
      case 'a':
        takeControlStatus = takeControl(api);
	 sem_post(&ss);
        break;
      case 'b':
        releaseControlStatus = releaseControl(api);
	 sem_post(&ss);
        break;
      case 'c':
        armStatus = arm(flight);
	 sem_post(&ss);
        break;
      case 'd':
        disArmStatus = disArm(flight);
	 sem_post(&ss);
        break;
      case 'e': 
        takeoffStatus = monitoredTakeoff(api, flight);
	 add_z = add_z + 1;
	 sem_post(&ss);
        break;
      case 'f':
        wayPointMissionExample(api, waypointObj,1);
	 sem_post(&ss);
        break;
      case 'g':
        drawSqrPosCtrlStatus = drawSqrPosCtrlSample(api, flight);
	 sem_post(&ss);
        break;
      case 'h':
        landingStatus = landing(api,flight);
	 add_z = 0;
	 sem_post(&ss);
        break;
      case 'i':
        goHomeStatus = goHome(flight);
	 add_z = 0;
	 add_x = 0;
	 add_y = 0;
	 sem_post(&ss);
        break;
      case 'j':
        userExitCommand = true;
	 sem_post(&ss);
        break;
		
/***************************************************************/
 	case 'm':
		
		 cout << "x and y are offsets to  North and the East directions" << endl;
		 cout << "move x,y,z  please input:x,y,z (metre)"                << endl;
		 cout << "x,y,z limit form 0 to 20" << endl;
		 sem_wait(&s);
		
		 cout << flight_position.x << "," << flight_position.y << "," << flight_position.z << endl;
		 
		 positionControlStatus = moveByPositionOffset(api, flight, flight_position.x, flight_position.y, flight_position.z, local_rotate);

		 add_z = add_z + flight_position.z;
		 add_x = add_x + flight_position.x;
		 add_y = add_y + flight_position.y;
		 
		 sem_post(&ss);
      	break;
	
	case 'p':
		pos = flight->getPosition();
		cout << " x = " << add_x << "; y = " << add_y << "; z = " << add_z << "; latitude = " << pos.latitude <<
			"; longitude =  " << pos.longitude << endl;
		flight_position.x = add_x;
		flight_position.y = add_y;
		flight_position.z = add_z;
		flight_position.rotate = local_rotate;
		flight_position.latitude = pos.latitude;
		flight_position.longitude = pos.longitude;
		sem_post(&ss);
	break;
	

	case 'r':
		cout << " rotate control, run with clockwise rotation, input the angle  " << endl;
		cout << " the angle is form -180 - 180 , angle :" << endl;
		sem_wait(&s);
		cout << flight_position.rotate << endl;
		positionControlStatus = moveByPositionOffset(api, flight, 0, 0, 0, flight_position.rotate);
		local_rotate =  flight_position.rotate;
		sem_post(&ss);
		/*
		while(1){
		 		if( (cin  >>  rotate) && (rotate <=180))
					break;
				else{
					cin.clear();
					cout << "x,y,z limit form 0 to 20" << endl;
					cout << " Input error, example: 10,10,10 "  << endl;
					cout << " Input again :" << endl;
					getchar();
				}
		 }
		 positionControlStatus = moveByPositionOffset(api, flight, 0, 0, 0, rotate);
		 */
	break;

	case 'x':
		while(1){
			cout << "xcontrol mode" << endl;
			usleep(1000000);
			sem_post(&ss);
			sem_wait(&s);
			cout << xcmd << endl;
			if( xcmd == '8'){ //go ahead
				if(local_rotate<90 && local_rotate>=0){
					positionControlStatus = moveByPositionOffset(api, flight, 2*cos(local_rotate*3.141592/180), 2*sin(local_rotate*3.141592/180), 0, local_rotate,1500);
					add_x = add_x + 2*cos(local_rotate*3.141592/180);
					add_y = add_y + 2*sin(local_rotate*3.141592/180);
				}
				else if(local_rotate>=90 && local_rotate<=180){
					positionControlStatus = moveByPositionOffset(api, flight, -2*sin(local_rotate*3.141592/180), -2*cos(local_rotate*3.141592/180), 0, local_rotate,15000);
					add_x = add_x -2*sin(local_rotate*3.141592/180);
					add_y = add_y -2*cos(local_rotate*3.141592/180);
				}
				else if(local_rotate<0 && local_rotate>=-90){
					positionControlStatus = moveByPositionOffset(api, flight, -2*cos((local_rotate+180)*3.141592/180), -2*sin((local_rotate+180)*3.141592/180), 0, local_rotate,1500);
					add_x = add_x - 2*cos((local_rotate+180)*3.141592/180);
					add_y = add_y  - 2*sin((local_rotate+180)*3.141592/180);
				}
				else{
					positionControlStatus = moveByPositionOffset(api, flight, -2*sin((local_rotate+180)*3.141592/180), -2*cos((local_rotate+180)*3.141592/180), 0, local_rotate,1500);
					add_x = add_x - 2*sin((local_rotate+180)*3.141592/180);
					add_y = add_y - 2*cos((local_rotate+180)*3.141592/180);
				}
			}

			if( xcmd == '2'){ //go back
				if(local_rotate<90 && local_rotate>=0){
					positionControlStatus = moveByPositionOffset(api, flight, -2*cos(local_rotate*3.141592/180), -2*sin(local_rotate*3.141592/180), 0, local_rotate,1500);
					add_x = add_x - 2*cos(local_rotate*3.141592/180);
					add_y = add_y - 2*sin(local_rotate*3.141592/180);
				}
				else if(local_rotate>=90 && local_rotate<=180){
					positionControlStatus = moveByPositionOffset(api, flight, 2*sin(local_rotate*3.141592/180), 2*cos(local_rotate*3.141592/180), 0, local_rotate,1500);
					add_x = add_x + 2*sin(local_rotate*3.141592/180);
					add_y = add_y + 2*cos(local_rotate*3.141592/180);
				}
				else if(local_rotate<0 && local_rotate>=-90){
					positionControlStatus = moveByPositionOffset(api, flight, 2*cos((local_rotate+180)*3.141592/180), 2*sin((local_rotate+180)*3.141592/180), 0, local_rotate,1500);
					add_x = add_x + 2*cos((local_rotate+180)*3.141592/180);
					add_y = add_y + 2*sin((local_rotate+180)*3.141592/180);
				}
				else{
					positionControlStatus = moveByPositionOffset(api, flight, 2*sin((local_rotate+180)*3.141592/180), +2*cos((local_rotate+180)*3.141592/180), 0, local_rotate,1500);
					add_x = add_x + 2*sin((local_rotate+180)*3.141592/180);
					add_y = add_y + 2*cos((local_rotate+180)*3.141592/180);
				}
			}

			if( xcmd == '6'){ //go right
				if(local_rotate<90 && local_rotate>=0){
					positionControlStatus = moveByPositionOffset(api, flight, -2*sin(local_rotate*3.141592/180), 2*cos(local_rotate*3.141592/180), 0, local_rotate,1500);
					add_x = add_x  -2*sin(local_rotate*3.141592/180);
					add_y = add_y + 2*cos(local_rotate*3.141592/180);
				}
				else if(local_rotate>=90 && local_rotate<=180){
					positionControlStatus = moveByPositionOffset(api, flight, 2*cos(local_rotate*3.141592/180), -2*sin(local_rotate*3.141592/180), 0, local_rotate,1500);
					add_x = add_x + 2*cos(local_rotate*3.141592/180);
					add_y = add_y - 2*sin(local_rotate*3.141592/180);
				}
				else if(local_rotate<0 && local_rotate>=-90){
					positionControlStatus = moveByPositionOffset(api, flight, 2*sin((local_rotate+180)*3.141592/180), -2*cos((local_rotate+180)*3.141592/180), 0, local_rotate,1500);
					add_x = add_x + 2*sin((local_rotate+180)*3.141592/180);
					add_y = add_y - 2*cos((local_rotate+180)*3.141592/180);
				}
				else{
					positionControlStatus = moveByPositionOffset(api, flight, 2*cos((local_rotate+180)*3.141592/180), -2*sin((local_rotate+180)*3.141592/180), 0, local_rotate,1500);
					add_x = add_x + 2*cos((local_rotate+180)*3.141592/180);
					add_y = add_y - 2*sin((local_rotate+180)*3.141592/180);
				}
			}

			if( xcmd == '4'){ //go left
				if(local_rotate<90 && local_rotate>=0){
					positionControlStatus = moveByPositionOffset(api, flight, 2*sin(local_rotate*3.141592/180), -2*cos(local_rotate*3.141592/180), 0, local_rotate,1500);
					add_x = add_x  +2*sin(local_rotate*3.141592/180);
					add_y = add_y - 2*cos(local_rotate*3.141592/180);
				}
				else if(local_rotate>=90 && local_rotate<=180){
					positionControlStatus = moveByPositionOffset(api, flight, -2*cos(local_rotate*3.141592/180), 2*sin(local_rotate*3.141592/180), 0, local_rotate,1500);
					add_x = add_x - 2*cos(local_rotate*3.141592/180);
					add_y = add_y + 2*sin(local_rotate*3.141592/180);
				}
				else if(local_rotate<0 && local_rotate>=-90){
					positionControlStatus = moveByPositionOffset(api, flight, -2*sin((local_rotate+180)*3.141592/180), 2*cos((local_rotate+180)*3.141592/180), 0, local_rotate,1500);
					add_x = add_x - 2*sin((local_rotate+180)*3.141592/180);
					add_y = add_y + 2*cos((local_rotate+180)*3.141592/180);
				}
				else{
					positionControlStatus = moveByPositionOffset(api, flight, -2*cos((local_rotate+180)*3.141592/180), +2*sin((local_rotate+180)*3.141592/180), 0, local_rotate,1500);
					add_x = add_x - 2*cos((local_rotate+180)*3.141592/180);
					add_y = add_y + 2*sin((local_rotate+180)*3.141592/180);
				}
			}

			if( xcmd =='1'){ // shun shi zhen 10 du
				local_rotate = local_rotate + 10;
				if(local_rotate > 180)
					local_rotate = local_rotate  - 10;
				positionControlStatus = moveByPositionOffset(api, flight, 0, 0, 0, local_rotate,1500);
			}

			if( xcmd == '3'){ //ni shi zhen 10 du 
				local_rotate = local_rotate - 10;
				if(local_rotate < -180)
					local_rotate = local_rotate + 10;
				positionControlStatus = moveByPositionOffset(api, flight, 0, 0, 0, local_rotate,1500);
			}

			if( xcmd == '5'){ //up
				positionControlStatus = moveByPositionOffset(api, flight, 0, 0, 1, local_rotate,1500);
				add_z = add_z + 1;
			}

			if( xcmd == '0'){ //down
				positionControlStatus = moveByPositionOffset(api, flight, 0, 0,-1, local_rotate,1500);
				add_z = add_z - 1;
			}
			
			if( xcmd == '7'){ //quit
				sem_post(&ss);
				break;
			}
		}	
	break;

	case 'z':
		sem_post(&ss);
		while(1){
			cout << "zcontrol mode" << endl;
			sem_wait(&s);
			cout << zcmd << endl;
			if( zcmd =='1'){ // shun shi zhen 10 du
				local_rotate = local_rotate + 10;
				if(local_rotate > 180)
					local_rotate = local_rotate  - 10;
				positionControlStatus = moveByPositionOffset(api, flight, 0, 0, 0, local_rotate,1500);
			}

			if( zcmd =='9'){ 
				time_break_flag = 1;
			}

			if( zcmd == '3'){ //ni shi zhen 10 du 
				local_rotate = local_rotate - 10;
				if(local_rotate < -180)
					local_rotate = local_rotate + 10;
				positionControlStatus = moveByPositionOffset(api, flight, 0, 0, 0, local_rotate,1500);
			}

			if( zcmd == '8'){ //go ahead
				if(local_rotate<90 && local_rotate>=0){
					positionControlStatus = moveByPositionOffset(api, flight, 50*cos(local_rotate*3.141592/180), 50*sin(local_rotate*3.141592/180), 0, local_rotate,250000);
					add_x = add_x + 2*cos(local_rotate*3.141592/180);
					add_y = add_y + 2*sin(local_rotate*3.141592/180);
				}
				else if(local_rotate>=90 && local_rotate<=180){
					positionControlStatus = moveByPositionOffset(api, flight, -50*sin(local_rotate*3.141592/180), -50*cos(local_rotate*3.141592/180), 0, local_rotate,250000);
					add_x = add_x -2*sin(local_rotate*3.141592/180);
					add_y = add_y -2*cos(local_rotate*3.141592/180);
				}
				else if(local_rotate<0 && local_rotate>=-90){
					positionControlStatus = moveByPositionOffset(api, flight, -50*cos((local_rotate+180)*3.141592/180), -50*sin((local_rotate+180)*3.141592/180), 0, local_rotate,250000);
					add_x = add_x - 2*cos((local_rotate+180)*3.141592/180);
					add_y = add_y  - 2*sin((local_rotate+180)*3.141592/180);
				}
				else{
					positionControlStatus = moveByPositionOffset(api, flight, -50*sin((local_rotate+180)*3.141592/180), -50*cos((local_rotate+180)*3.141592/180), 0, local_rotate,250000);
					add_x = add_x - 2*sin((local_rotate+180)*3.141592/180);
					add_y = add_y - 2*cos((local_rotate+180)*3.141592/180);
				}
			}

			if( zcmd == '2'){ //go back
				if(local_rotate<90 && local_rotate>=0){
					positionControlStatus = moveByPositionOffset(api, flight, -50*cos(local_rotate*3.141592/180), -50*sin(local_rotate*3.141592/180), 0, local_rotate,250000);
					add_x = add_x - 2*cos(local_rotate*3.141592/180);
					add_y = add_y - 2*sin(local_rotate*3.141592/180);
				}
				else if(local_rotate>=90 && local_rotate<=180){
					positionControlStatus = moveByPositionOffset(api, flight, 50*sin(local_rotate*3.141592/180), 50*cos(local_rotate*3.141592/180), 0, local_rotate,250000);
					add_x = add_x + 2*sin(local_rotate*3.141592/180);
					add_y = add_y + 2*cos(local_rotate*3.141592/180);
				}
				else if(local_rotate<0 && local_rotate>=-90){
					positionControlStatus = moveByPositionOffset(api, flight, 50*cos((local_rotate+180)*3.141592/180), 50*sin((local_rotate+180)*3.141592/180), 0, local_rotate,250000);
					add_x = add_x + 2*cos((local_rotate+180)*3.141592/180);
					add_y = add_y + 2*sin((local_rotate+180)*3.141592/180);
				}
				else{
					positionControlStatus = moveByPositionOffset(api, flight, 50*sin((local_rotate+180)*3.141592/180), +50*cos((local_rotate+180)*3.141592/180), 0, local_rotate,250000);
					add_x = add_x + 2*sin((local_rotate+180)*3.141592/180);
					add_y = add_y + 2*cos((local_rotate+180)*3.141592/180);
				}
			}

			if( zcmd == '6'){ //go right
				if(local_rotate<90 && local_rotate>=0){
					positionControlStatus = moveByPositionOffset(api, flight, -50*sin(local_rotate*3.141592/180), 50*cos(local_rotate*3.141592/180), 0, local_rotate,250000);
					add_x = add_x  -2*sin(local_rotate*3.141592/180);
					add_y = add_y + 2*cos(local_rotate*3.141592/180);
				}
				else if(local_rotate>=90 && local_rotate<=180){
					positionControlStatus = moveByPositionOffset(api, flight, 50*cos(local_rotate*3.141592/180), -50*sin(local_rotate*3.141592/180), 0, local_rotate,250000);
					add_x = add_x + 2*cos(local_rotate*3.141592/180);
					add_y = add_y - 2*sin(local_rotate*3.141592/180);
				}
				else if(local_rotate<0 && local_rotate>=-90){
					positionControlStatus = moveByPositionOffset(api, flight, 50*sin((local_rotate+180)*3.141592/180), -50*cos((local_rotate+180)*3.141592/180), 0, local_rotate,250000);
					add_x = add_x + 2*sin((local_rotate+180)*3.141592/180);
					add_y = add_y - 2*cos((local_rotate+180)*3.141592/180);
				}
				else{
					positionControlStatus = moveByPositionOffset(api, flight, 50*cos((local_rotate+180)*3.141592/180), -50*sin((local_rotate+180)*3.141592/180), 0, local_rotate,250000);
					add_x = add_x + 2*cos((local_rotate+180)*3.141592/180);
					add_y = add_y - 2*sin((local_rotate+180)*3.141592/180);
				}
			}

			if( zcmd == '4'){ //go lef
				
				
				if(local_rotate<90 && local_rotate>=0){
					positionControlStatus = moveByPositionOffset(api, flight, 50*sin(local_rotate*3.141592/180), -50*cos(local_rotate*3.141592/180), 0, local_rotate,250000);
					add_x = add_x  +2*sin(local_rotate*3.141592/180);
					add_y = add_y - 2*cos(local_rotate*3.141592/180);
				}
				else if(local_rotate>=90 && local_rotate<=180){
					positionControlStatus = moveByPositionOffset(api, flight, -50*cos(local_rotate*3.141592/180), 50*sin(local_rotate*3.141592/180), 0, local_rotate,250000);
					add_x = add_x - 2*cos(local_rotate*3.141592/180);
					add_y = add_y + 2*sin(local_rotate*3.141592/180);
				}
				else if(local_rotate<0 && local_rotate>=-90){
					positionControlStatus = moveByPositionOffset(api, flight, -50*sin((local_rotate+180)*3.141592/180), 50*cos((local_rotate+180)*3.141592/180), 0, local_rotate,250000);
					add_x = add_x - 2*sin((local_rotate+180)*3.141592/180);
					add_y = add_y + 2*cos((local_rotate+180)*3.141592/180);
				}
				else{
					positionControlStatus = moveByPositionOffset(api, flight, -50*cos((local_rotate+180)*3.141592/180), +50*sin((local_rotate+180)*3.141592/180), 0, local_rotate,250000);
					add_x = add_x - 2*cos((local_rotate+180)*3.141592/180);
					add_y = add_y + 2*sin((local_rotate+180)*3.141592/180);
				}
				
			}

			if( zcmd == '5'){ //up
				positionControlStatus = moveByPositionOffset(api, flight, 0, 0, 1, local_rotate,1500);
				add_z = add_z + 1;
			}

			if( zcmd == '0'){ //down
				positionControlStatus = moveByPositionOffset(api, flight, 0, 0,-1, local_rotate,1500);
				add_z = add_z - 1;
			}


			if( zcmd == '7'){ //quit
				break;
			}
		}
	break;
	
	
/***************************************************************/
			
    }
    usleep(1000000);
  }
  return;
}
