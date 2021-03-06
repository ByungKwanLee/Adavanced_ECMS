// C++ library 
#include <iostream>
#include <math.h>
#include <typeinfo>  // typeid::name operator

// Advanced ECMS library
#include <VehicleInfo.hpp>
#include <ICEMG.hpp>
#include <interp_tool.hpp>
#include <performance.hpp>

// ROS library
#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int32.h>
#include <geometry_msgs/Twist.h>

// getcwd
// #include <unistd.h>
// std::string GetCurrentWorkingDir( void ) {
//   char buff[FILENAME_MAX];
//   getcwd( buff, FILENAME_MAX );
//   std::string current_working_dir(buff);
//   return current_working_dir;
// }

using namespace std;



// void accel_callback(const std_msgs::Float32::ConstPtr & accel_data)
// {	
// 	VehicleInfo::velocity_update(accel_data->data);
// 	// cout << "velocity and accel : "<< VehicleInfo::velocity_rt <<", "<< VehicleInfo::accel_rt << endl;
// } 

void accel_callback(const geometry_msgs::Twist::ConstPtr & accel_data)
{	
	VehicleInfo::velocity_update((accel_data->linear).x);
	// cout << "velocity and accel : "<< VehicleInfo::velocity_rt <<", "<< VehicleInfo::accel_rt << endl;
}


int main(int argc, char ** argv){


// ros setting for making node
ros::init(argc, argv, "advanced_ecms");
ros::NodeHandle nh;

string ECMS_mode;
string method;
int iter;

nh.getParam("/ecms/ECMS_mode", ECMS_mode);
nh.getParam("/ecms/method", method);
nh.getParam("/ecms/iter", iter);

cout << (ECMS_mode)<< method<<iter<<endl;

// ros::Subscriber sub = nh.subscribe("/accel", 1, accel_callback);
ros::Subscriber sub = nh.subscribe("/joy_teleop/cmd_vel", 1, accel_callback);
ros::Publisher pub_vel = nh.advertise<std_msgs::Float32>("/velocity", 1);
ros::Publisher pub_soc = nh.advertise<std_msgs::Float32>("/soc", 1);
ros::Publisher pub_mass = nh.advertise<std_msgs::Float32>("/mass", 1);
ros::Publisher pub_mode = nh.advertise<std_msgs::Int32>("/mode", 1);
ros::Publisher pub_gear = nh.advertise<std_msgs::Int32>("/gear", 1);
ros::Publisher pub_lambda = nh.advertise<std_msgs::Float32>("/lambda", 1);
ros::Publisher pub_mu = nh.advertise<std_msgs::Float32>("/mu",1);
ros::Publisher pub_rho = nh.advertise<std_msgs::Float32>("/rho",1);


assert( (ECMS_mode == "performance" || ECMS_mode == "realtime") 
	&& "ECMS_mode is only performance or realtime ");
assert( (method == "L" || method == "ADMM") 
	&& "method is only L or ADMM ");

if(ECMS_mode == "performance")
{

// finding parameter for performance check by each method
std::vector<float> lambda;
std::vector<float> mu;
std::vector<float> raw;
std::vector<float> SOC_constraint;
std::vector<float> Fuel_Consumption;


if(method == "ADMM")
{
	for (register int ind = 1; ind < iter + 1; ind ++)
	{	
		Optimizer  obj_optimizer(0,0,ind);
		ECMS_performance obj_per(0.6);
		obj_per.do_performance(obj_optimizer, method, 1);

		mu.push_back(obj_optimizer.mu);
		raw.push_back(obj_optimizer.raw);
		SOC_constraint.push_back(abs(MG::SOC - 0.6));
		Fuel_Consumption.push_back(obj_optimizer.En_FC_sum);
	}
}


else{

	for (register int ind = 1; ind < iter + 1 ; ind ++)
	{
		Optimizer obj_optimizer(0,0,ind);
		ECMS_performance obj_per(0.6);
		obj_per.do_performance(obj_optimizer, method, 1);

		lambda.push_back(obj_optimizer.lambda);
		SOC_constraint.push_back(abs(MG::SOC - 0.6));
		Fuel_Consumption.push_back(obj_optimizer.En_FC_sum);
	}
}

// csv file save
ofstream myFile;
if (method == "L")
{
	myFile.open("src/advanced_ecms/output/iter/lambda.csv");
	for(register int ind = 0; ind < lambda.size(); ind ++)
	{
	    myFile << ind * VehicleInfo::time_rt <<
	    ","<<lambda[ind]<< endl;
	}
	myFile.close();	
}
else
{
	myFile.open("src/advanced_ecms/output/iter/mu.csv");
	for(register int ind = 0; ind < mu.size(); ind ++)
	{
	    myFile << ind * VehicleInfo::time_rt <<
	    ","<<mu[ind]<< endl;
	}
	myFile.close();
	myFile.open("src/advanced_ecms/output/iter/raw.csv");
	for(register int ind = 0; ind < raw.size(); ind ++)
	{
	    myFile << ind * VehicleInfo::time_rt <<
	    ","<<raw[ind]<< endl;
	}
	myFile.close();


}

myFile.open("src/advanced_ecms/output/iter/SOC_constraint.csv");

for(register int ind = 0; ind < SOC_constraint.size(); ind ++)
{
    myFile << ind * VehicleInfo::time_rt <<
    ","<<SOC_constraint[ind]<< endl;
}

myFile.close();
myFile.open("src/advanced_ecms/output/iter/Fuel_Consumption.csv");

for(register int ind = 0; ind < Fuel_Consumption.size(); ind ++)
{
    myFile << ind * VehicleInfo::time_rt <<
    ","<<Fuel_Consumption[ind]<< endl;
}
myFile.close();


// performance check by each method after finding parameter

Optimizer  obj_optimizer(0,0,iter);
ECMS_performance obj_per(0.6);
obj_per.do_performance(obj_optimizer, method, 1);
obj_per.csvwrite(method);

}


else
{


// real time processing part
float HZ = 20;
VehicleInfo::time_rt = 1./HZ;
ros::Rate rate(HZ); // HZ
Optimizer obj_optimizer(0,0,10);
while(ros::ok())
{	
	ros::Time start = ros::Time::now();

	// ros spin for subscribing rt var : accel
	ros::spinOnce();

	// update from rt var -> optimizer var
	VehicleInfo::for_optimizer();

	int num = 0;
	bool update = false;
	while(!update)
	{
		update = Tool::ICEMG_parameter_update();
		num += 1;
	}

	obj_optimizer.optimizer(method);
	obj_optimizer.En_FC_rt(false);

	ros::Time end = ros::Time::now();

	cout << "Update num : "<< num <<", Processing Time : "<< (end-start).toSec() <<endl;

	// publish velocity
	std_msgs::Float32 msg_vel; 
	msg_vel.data = VehicleInfo::velocity*3.6;
	pub_vel.publish(msg_vel);

	// publish mass
	std_msgs::Float32 msg_mass; 
	msg_mass.data = obj_optimizer.En_FC_sum;
	pub_mass.publish(msg_mass);

	// publish SOC
	std_msgs::Float32 msg_SOC; 
	msg_SOC.data = MG::SOC;
	pub_soc.publish(msg_SOC);

	// publish gear
	std_msgs::Int32 msg_gear; 
	msg_gear.data = std::get<0>(obj_optimizer.optimal_inform)=="EV_Stop" ?
			0 : std::get<1>(obj_optimizer.optimal_inform);
	pub_gear.publish(msg_gear);

	// publish mode
	std_msgs::Int32 msg_mode; 
	msg_mode.data = int((std::get<0>(obj_optimizer.optimal_inform) == "HEV" ? 2 : 1) )
      + int((std::get<0>(obj_optimizer.optimal_inform) == "EV_Stop" ?  -1 : 0))
      + int((std::get<0>(obj_optimizer.optimal_inform) == "EV_Regen" ?  -2 : 0));
	pub_mode.publish(msg_mode);

	// publish lambda
	std_msgs::Float32 msg_lambda;
	msg_lambda.data = obj_optimizer.lambda;
	pub_lambda.publish(msg_lambda);

	// publish mu
	std_msgs::Float32 msg_mu;
	msg_mu.data = obj_optimizer.mu;
	pub_mu.publish(msg_mu);

	// publish rho
	std_msgs::Float32 msg_raw;
	msg_raw.data = obj_optimizer.raw;
	pub_rho.publish(msg_raw);

	// sleep thread
	rate.sleep();

} // for end
} // if end

}