//Required include files
#include <stdio.h>	
#include <string>
#include <iostream>
#include "pubSysCls.h"	

using namespace sFnd;

// Send message and wait for newline
void msgUser(const char *msg) {
	std::cout << msg;
	getchar();
}

//*********************************************************************************
//This program will load configuration files onto each node connected to the port, then executes
//sequential repeated moves on each axis.
//*********************************************************************************

#define ACC_LIM_RPM_PER_SEC	1000
#define VEL_LIM_RPM			100
#define MOVE_DISTANCE_CNTS	100000
#define NUM_MOVES			5
#define TIME_TILL_TIMEOUT	10000	//The timeout used for homing(ms)

void adjustTorque(INode &theNode) {

	// Read the velocity =====================================
	theNode.Motion.VelMeasured.Refresh();
	float velocity = theNode.Motion.VelMeasured.Value();

	// Set the torque =====================================
	float torque_setpoint;
	float torque_at_origin = 0;
	float slope = 0;

	// Set the torque when the velocity is < 0
	if(velocity < 0.0){
		torque_at_origin = 0.3;
		slope = 0.05;

	// Set the torque when the velocity is > 0.1
	} else if (velocity > 0.1){
		slope = 0.10;
		torque_at_origin = 0.59;

	// Set the torque when the velocity is between 0 and 0.1
	} else {
		torque_at_origin = 0.3;
		slope = 3.00;
	}

	// Set the toque setpoint
	torque_setpoint = velocity * slope + torque_at_origin;

	// Put limits on the torque setpoint
	if (torque_setpoint > 10.0) {
		torque_setpoint = 10.0;
	}
	if (torque_setpoint < 2.0) {
		torque_setpoint = 2.0;
	}
	printf("Velocity: %f Torque: %f\n", velocity, torque_setpoint);

	theNode.Limits.TrqGlobal.Value(velocity, false);
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

		msgUser("Press any key to continue."); //pause so the user can see the error message; waits for user to press a key

		return -1;  //This terminates the main program
	}
	//printf("\n I will now open port \t%i \n \n", portnum);
	myMgr.PortsOpen(portCount);				//Open the port

	return portCount;

}

int main(int argc, char *argv[]) {

	size_t portCount = 0;

	//Create the SysManager object. This object will coordinate actions among various ports
	// and within nodes. In this example we use this object to setup and open our port.
	SysManager myMgr;							//Create System Manager myMgr
	myMgr.PortsClose();

	//This will try to open the port. If there is an error/exception during the port opening,
	//the code will jump to the catch loop where detailed information regarding the error will be displayed;
	//otherwise the catch loop is skipped over
	try {

		portCount = connect(myMgr);

		for (size_t i = 0; i < portCount; i++) {


			IPort &myPort = myMgr.Ports(i);

			printf(" Port[%d]: state=%d, nodes=%d\n", myPort.NetNumber(),
					myPort.OpenState(), myPort.NodeCount());

			//Once the code gets past this point, it can be assumed that the Port has been opened without issue
			//Now we can get a reference to our port object which we will use to access the node objects

			for (size_t iNode = 0; iNode < myPort.NodeCount(); iNode++) {
				// Create a shortcut reference for a node
				INode &theNode = myPort.Nodes(iNode);

				theNode.EnableReq(false);//Ensure Node is disabled before loading config file

				myMgr.Delay(200);

				//theNode.Setup.ConfigLoad("Config File path");

				printf("   Node[%d]: type=%d\n", int(iNode),
						theNode.Info.NodeType());
				printf("            userID: %s\n", theNode.Info.UserID.Value());
				printf("        FW version: %s\n",
						theNode.Info.FirmwareVersion.Value());
				printf("          Serial #: %d\n",
						theNode.Info.SerialNumber.Value());
				printf("             Model: %s\n", theNode.Info.Model.Value());

				//The following statements will attempt to enable the node.  First,
				// any shutdowns or NodeStops are cleared, finally the node is enabled
				theNode.Status.AlertsClear();			//Clear Alerts on node
				theNode.Motion.NodeStopClear();	//Clear Nodestops on Node  				
				theNode.EnableReq(true);					//Enable node 
				//At this point the node is enabled
				printf("Node \t%zi enabled\n", iNode);
				double timeout = myMgr.TimeStampMsec() + TIME_TILL_TIMEOUT;	//define a timeout in case the node is unable to enable
																			//This will loop checking on the Real time values of the node's Ready status
				while (!theNode.Motion.IsReady()) {
					if (myMgr.TimeStampMsec() > timeout) {
						printf(
								"Error: Timed out waiting for Node %d to enable\n",
								iNode);
						msgUser("Press any key to continue."); //pause so the user can see the error message; waits for user to press a key
						return -2;
					}
				}
				//At this point the Node is enabled, and we will now check to see if the Node has been homed
				//Check the Node to see if it has already been homed, 
				if (theNode.Motion.Homing.HomingValid()) {
					if (theNode.Motion.Homing.WasHomed()) {
						printf(
								"Node %d has already been homed, current position is: \t%8.0f \n",
								iNode, theNode.Motion.PosnMeasured.Value());
						printf("Rehoming Node... \n");
					} else {
						printf(
								"Node [%d] has not been homed.  Homing Node now...\n",
								iNode);
					}
					//Now we will home the Node
					theNode.Motion.Homing.Initiate();

					timeout = myMgr.TimeStampMsec() + TIME_TILL_TIMEOUT;//define a timeout in case the node is unable to enable
																		// Basic mode - Poll until disabled
					while (!theNode.Motion.Homing.WasHomed()) {
						if (myMgr.TimeStampMsec() > timeout) {
							printf(
									"Node did not complete homing:  \n\t -Ensure Homing settings have been defined through ClearView. \n\t -Check for alerts/Shutdowns \n\t -Ensure timeout is longer than the longest possible homing move.\n");
							msgUser("Press any key to continue."); //pause so the user can see the error message; waits for user to press a key
							return -2;
						}
					}
					printf("Node completed homing\n");
				} else {
					printf(
							"Node[%d] has not had homing setup through ClearView.  The node will not be homed.\n",
							iNode);
				}

			}

			///////////////////////////////////////////////////////////////////////////////////////
			//At this point we will execute 10 rev moves sequentially on each axis
			//////////////////////////////////////////////////////////////////////////////////////
			for (size_t i = 0; i < NUM_MOVES; i++) {
				for (size_t iNode = 0; iNode < myPort.NodeCount(); iNode++) {
					// Create a shortcut reference for a node
					INode &theNode = myPort.Nodes(iNode);
					printf("Position tracking limit: %d \n",
							theNode.Limits.PosnTrackingLimit.Value());
					theNode.Limits.PosnTrackingLimit.Value(128000, false);

					printf("MoveWentDone");
					theNode.Motion.MoveWentDone();//Clear the rising edge Move done register
					printf("Done \n");

					theNode.AccUnit(INode::RPM_PER_SEC);//Set the units for Acceleration to RPM/SEC
					theNode.VelUnit(INode::RPM);//Set the units for Velocity to RPM
					theNode.Motion.AccLimit = ACC_LIM_RPM_PER_SEC;//Set Acceleration Limit (RPM/Sec)
					theNode.Motion.VelLimit = VEL_LIM_RPM;//Set Velocity Limit (RPM)
					theNode.Limits.TrqGlobal = 0.5;

					printf("Moving Node \t%zi \n", iNode);
					theNode.Motion.MovePosnStart(MOVE_DISTANCE_CNTS);//Execute 10000 encoder count move
					printf("%f estimated time.\n",
							theNode.Motion.MovePosnDurationMsec(
									MOVE_DISTANCE_CNTS));
					double timeout = myMgr.TimeStampMsec()
							+ theNode.Motion.MovePosnDurationMsec(
									MOVE_DISTANCE_CNTS) + 5000;	//define a timeout in case the node is unable to enable

					while (!theNode.Motion.MoveIsDone()) {
						if (myMgr.TimeStampMsec() > timeout) {
							//theNode.Limits.TrqGlobal.Value(1.1, false);

							//printf("Error: Timed out waiting for move to complete\n");
							//msgUser("Press any key to continue."); //pause so the user can see the error message; waits for user to press a key
							// Close down the ports
							//myMgr.PortsClose();

							//return -2;
						}

						adjustTorque(theNode);
						//printf("%f ", theNode.Motion.TrqMeasured);
						//theNode.Motion.TrqMeasured.Refresh();
						//printf("Torque: %f \n", theNode.Motion.TrqMeasured.Value());

						//printf("Torque: %f \n", theNode.Motion.VelMeasured.Value());

						//printf("%f ", theNode.Motion.VelMeasured);

					}
					printf("\nNode \t%zi Move Done\n", iNode);

				} // for each node
			} // for each move

			//////////////////////////////////////////////////////////////////////////////////////////////
			//After moves have completed Disable node, and close ports
			//////////////////////////////////////////////////////////////////////////////////////////////
			printf("Disabling nodes, and closing port\n");
			//Disable Nodes

			for (size_t iNode = 0; iNode < myPort.NodeCount(); iNode++) {
				// Create a shortcut reference for a node
				myPort.Nodes(iNode).EnableReq(false);
			}
		}
	} catch (mnErr &theErr) {
		printf("Failed to disable Nodes n\n");
		//This statement will print the address of the error, the error code (defined by the mnErr class), 
		//as well as the corresponding error message.
		printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr,
				theErr.ErrorCode, theErr.ErrorMsg);

		//msgUser("Press any key to continue."); //pause so the user can see the error message; waits for user to press a key
		//return 0;  //This terminates the main program
	}

	// Close down the ports
	myMgr.PortsClose();

	//msgUser("Press any key to continue."); //pause so the user can see the error message; waits for user to press a key
	return 0;			//End program
}

