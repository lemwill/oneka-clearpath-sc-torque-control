//Required include files
#include <stdio.h>	
#include <string>
#include <iostream>
#include "pubSysCls.h"	
#include <unistd.h>
#include <signal.h>

using namespace sFnd;

#define MOTOR_TORQUE_TO_FORCE_KG_PER_PERCENT 0.9

// Control loop configuration
float VELOCITY_MAX_RPM = 400;
float VELOCITY_MAX_TIMER = 20;
float FORCE_MAX_KG = 25;
float FORCE_MIN_KG = 3;
float FORCE_AT_ORIGIN = 10;
float TRANSITION_VELOCITY_WINDOW_WIDTH_RPM = 50;
float WORKING_FORCE_KG = 20;
float WORKING_SLOPE_KG_PER_RPM = 0.05;
float RETURN_SLOPE_KG_PER_RPM = 0.05;

// Cable rewind configuration

#define ENCODER_TURNS_PER_REVOLUTION 32000
#define ACCELERATION_LIMIT_RPM_PER_SECOND	300
#define TIME_TILL_TIMEOUT	10000	//The timeout used for homing(ms)

INode* node;

void exitHandler(int value) {
	printf("Ctrl+C detected. Stopping motor");
	node->Motion.NodeStop(STOP_TYPE_DISABLE_ABRUPT);
	
	exit(0);
}

int verifyVelocity(float velocity, INode &theNode) {
	static int velocity_error_counter = 0;

	// If the velocity is above a threshold for too long, shutdown the motor
	if (abs(velocity) > VELOCITY_MAX_RPM) {

		// Increase the velocity_error_counter
		velocity_error_counter += 1;
		if (velocity_error_counter >= VELOCITY_MAX_TIMER) {
			theNode.Motion.NodeStop(STOP_TYPE_DISABLE_ABRUPT);
			printf("Stop the motor. The velocity was too high \n");
			return -1;
		}
	} else {
		velocity_error_counter = 0;
	}

	return 0;
}

void adjustTorque(INode &theNode, float velocity) {

	// Set the torque =====================================


	// torque_at_origin
	// torque_slope_at_negative_velocity
	// velocity

	//
	//
	//
	//
	//               |
	//  ____---------|
	//              0
	float slope = 0;
	float origin = 0;
	// Set the torque when the velocity is < 0

	float transition_slope = (WORKING_FORCE_KG-FORCE_AT_ORIGIN)/TRANSITION_VELOCITY_WINDOW_WIDTH_RPM;

	if (velocity < 0.0) {
		origin = FORCE_AT_ORIGIN;
		slope = RETURN_SLOPE_KG_PER_RPM;

		// Set the torque when the velocity is > 0.1
	} else if (velocity > TRANSITION_VELOCITY_WINDOW_WIDTH_RPM) {
		slope = WORKING_SLOPE_KG_PER_RPM;
		origin = FORCE_AT_ORIGIN+TRANSITION_VELOCITY_WINDOW_WIDTH_RPM*(transition_slope-WORKING_SLOPE_KG_PER_RPM);

		// Set the torque when the velocity is between 0 and 0.1
	} else {
		origin = FORCE_AT_ORIGIN;
		slope = transition_slope;
	}

	// Set the toque setpoint
	float torque_setpoint_percentage = velocity * slope + origin;

	float torque_setpoint_percentage_with_saturation =
			torque_setpoint_percentage;

	// Put limits on the torque setpoint
	if (torque_setpoint_percentage_with_saturation > FORCE_MAX_KG) {
		torque_setpoint_percentage_with_saturation = FORCE_MAX_KG;
	}
	if (torque_setpoint_percentage_with_saturation < FORCE_MIN_KG) {
		torque_setpoint_percentage_with_saturation = FORCE_MIN_KG;
	}
	printf("Velocity: %f Torque before saturation: %f Torque: %f\n", velocity,
			torque_setpoint_percentage,
			torque_setpoint_percentage_with_saturation);

	theNode.Limits.TrqGlobal.Value(torque_setpoint_percentage_with_saturation, false);
}


void rewindWire(){


}


size_t connect(SysManager &myMgr) {

	size_t portCount = 0;

	std::vector<std::string> comHubPorts;

	SysManager::FindComHubPorts(comHubPorts);
	printf("Found %d SC Hubs\n", comHubPorts.size());

	for (portCount = 0;
			portCount < comHubPorts.size() && portCount < NET_CONTROLLER_MAX;
			portCount++) {

		myMgr.ComHubPort(portCount, comHubPorts[portCount].c_str()); //define the first SC Hub port (port 0) to be associated
		// with COM portnum (as seen in device manager)
	}

	if (portCount < 0) {

		printf("Unable to locate SC hub port\n");

		return -1;  //This terminates the main program
	}
	//printf("\n I will now open port \t%i \n \n", portnum);
	myMgr.PortsOpen(portCount);				//Open the port

	return portCount;
}

int initialize_motor(INode &theNode, SysManager &myMgr) {

	theNode.EnableReq(false);//Ensure Node is disabled before loading config file

	myMgr.Delay(200);

	//theNode.Setup.ConfigLoad("Config File path");

	printf("   Node[%d]: type=%d\n", int(0), theNode.Info.NodeType());
	printf("            userID: %s\n", theNode.Info.UserID.Value());
	printf("        FW version: %s\n", theNode.Info.FirmwareVersion.Value());
	printf("          Serial #: %d\n", theNode.Info.SerialNumber.Value());
	printf("             Model: %s\n", theNode.Info.Model.Value());

	//The following statements will attempt to enable the node.  First,
	// any shutdowns or NodeStops are cleared, finally the node is enabled
	theNode.Status.AlertsClear();			//Clear Alerts on node
	theNode.Motion.NodeStopClear();	//Clear Nodestops on Node
	theNode.EnableReq(true);					//Enable node
	//At this point the node is enabled
	printf("Node \t%zi enabled\n", 0);
	double timeout = myMgr.TimeStampMsec() + TIME_TILL_TIMEOUT;	//define a timeout in case the node is unable to enable
																//This will loop checking on the Real time values of the node's Ready status
	while (!theNode.Motion.IsReady()) {
		if (myMgr.TimeStampMsec() > timeout) {
			printf("Error: Timed out waiting for Node %d to enable\n", 0);
			return -2;
		}
	}
}

int returnMotorToHome(INode &theNode, SysManager &myMgr) {
	double timeout;
	//At this point the Node is enabled, and we will now check to see if the Node has been homed
	//Check the Node to see if it has already been homed,
	if (theNode.Motion.Homing.HomingValid()) {
		if (theNode.Motion.Homing.WasHomed()) {
			printf(
					"Node %d has already been homed, current position is: \t%8.0f \n",
					0, theNode.Motion.PosnMeasured.Value());
			printf("Rehoming Node... \n");
		} else {
			printf("Node [%d] has not been homed.  Homing Node now...\n",
					0);
		}
		//Now we will home the Node
		theNode.Motion.Homing.Initiate();

		timeout = myMgr.TimeStampMsec() + TIME_TILL_TIMEOUT;//define a timeout in case the node is unable to enable
															// Basic mode - Poll until disabled
		while (!theNode.Motion.Homing.WasHomed()) {
			if (myMgr.TimeStampMsec() > timeout) {
				printf(
						"Node did not complete homing:  \n\t -Ensure Homing settings have been defined through ClearView. \n\t -Check for alerts/Shutdowns \n\t -Ensure timeout is longer than the longest possible homing move.\n");
				return -2;
			}
		}
		printf("Node completed homing\n");
	} else {
		printf(
				"Node[%d] has not had homing setup through ClearView.  The node will not be homed.\n",
				0);
	}
}


int moveMotor(int distanceCount, INode &theNode, SysManager &myMgr, float maxVelocity = 60, bool torqueControlOn = true, bool rewindMotor = false){

	// Change the position tracking error limit. This allows to control the motor in torque
	theNode.Limits.PosnTrackingLimit.Value(128000000, false);
	//theNode.Limits.PosnTrackingLimit.Value(1600, false);

	printf("Position tracking limit: %d \n", theNode.Limits.PosnTrackingLimit.Value());

	printf("MoveWentDone");
	theNode.Motion.MoveWentDone();//Clear the rising edge Move done register
	printf("Done \n");

	theNode.AccUnit(INode::RPM_PER_SEC);//Set the units for Acceleration to RPM/SEC
	theNode.VelUnit(INode::RPM);//Set the units for Velocity to RPM
	theNode.Motion.AccLimit = ACCELERATION_LIMIT_RPM_PER_SECOND;//Set Acceleration Limit (RPM/Sec)
	theNode.Motion.VelLimit = maxVelocity;//Set Velocity Limit (RPM)

	printf("Moving Node \t%zi \n", 0);
	theNode.Motion.MovePosnStart(distanceCount);//Execute 10000 encoder count move
	printf("%f estimated time.\n",
			theNode.Motion.MovePosnDurationMsec(
					distanceCount));
	double timeout = myMgr.TimeStampMsec()
			+ theNode.Motion.MovePosnDurationMsec(
					distanceCount) + 5000;	//define a timeout in case the node is unable to enable

	while (!theNode.Motion.MoveIsDone()) {
		if (myMgr.TimeStampMsec() > timeout) {
			//printf("Error: Timed out waiting for move to complete\n");
			//theNode.Motion.NodeStop(STOP_TYPE_DISABLE_ABRUPT);

		}



		// Read the velocity =====================================
		theNode.Motion.VelMeasured.Refresh();

		float velocity = theNode.Motion.VelMeasured.Value();

		if (torqueControlOn == true){
			adjustTorque(theNode, velocity);
		}

		if (rewindMotor == true){

			theNode.Limits.TrqGlobal.Value(25, false);

			static int velocity_error_counter = 0;

			theNode.Motion.TrqMeasured.Refresh();
			float torque = theNode.Motion.TrqMeasured.Value();
			printf("Rewinding motor, torque: %f\n", torque);

			if(abs(torque) > 20){
				velocity_error_counter = velocity_error_counter + 1;

				if (velocity_error_counter > 40){
					//theNode.Motion.NodeStop(STOP_TYPE_DISABLE_ABRUPT);
					return 0;
					printf("Done rewinding motor\n");
				}
			} else {
				velocity_error_counter = 0;
			}
		}

		verifyVelocity(velocity, theNode);


	}
	if (rewindMotor = true){
		printf("reached max number of turns. Stopping");
		theNode.Motion.NodeStop(STOP_TYPE_DISABLE_ABRUPT);
	}
	printf("\nNode \t%zi Move Done\n", 0);
}


int main(int argc, char *argv[]) {
	
	signal(SIGINT, exitHandler);
	SysManager myMgr;

	try {

		connect(myMgr);

		IPort &myPort = myMgr.Ports(0);

		node = &(myPort.Nodes(0));

		initialize_motor(*node, myMgr);

		returnMotorToHome(*node, myMgr);

		// Move the motor
		//moveMotor(100000, node, myMgr, false);

		// Move the motor with torque control
		//for( int i = 0; i < 10; i++){
		//	theNode.Limits.TrqGlobal.Value(3, false);
		//}
		moveMotor(-1*ENCODER_TURNS_PER_REVOLUTION, *node, myMgr,60, false, true);

		moveMotor(-10*ENCODER_TURNS_PER_REVOLUTION, *node, myMgr, 1000, true, false);

		// Disable the node
		myPort.Nodes(0).EnableReq(false);

	} catch (mnErr &theErr) {

		printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
	}

	myMgr.PortsClose();

	return 0;

}

