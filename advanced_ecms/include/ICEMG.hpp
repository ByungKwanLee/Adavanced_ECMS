#include <iostream>
#include <eigen3/Eigen/Dense>
#include <math.h>
#include <vector>

using namespace std;



class ICE
{

public:
	// TM, FD ratio, Eff
	static vector<float> TM_Ratio;
	static Eigen::VectorXf TMR; // type casting from std to eigen
	static float TM_Eff;
	static float FD_EFF;
	static float FD_Ratio;

	static vector<float> En_minRPM;
	static vector<float> En_minTrq;
	static vector<float> En_maxRPM;
	static vector<float> En_maxTrq;
	static vector<float> En_mapRPM;
	static vector<float> En_mapTrq;
	static vector<vector<float>> En_mapData;
	static vector<vector<float>> En_FC;

	// HEV
	static Eigen::VectorXf Wi_HEV;
	static Eigen::MatrixXf Wi_HEV_rep; // special
	static Eigen::VectorXf Ti_max;
	static Eigen::MatrixXf Ti_HEV;

	// FC
	static Eigen::MatrixXf FC_HEV;

	// constraints
	static float Wi_HEV_min; //constraints
	static float Wi_HEV_max; //constraints

};

class MG
{
public:

	// battery configuration
	static float Voc;
	static float Rint;
	static float P_bat;
	static float Bat_NumCell;
	static float P_aux;

	static float Bat_Quantity;
	static float SOC;
	static vector<float> Bat_indexRint;
	static vector<float> Bat_indexSoc;
	static vector<float> Bat_indexVoc;

	// Motor map
	static vector<float> MG_mapTrq;
	static vector<float> MG_mapRPM;
	static vector<vector<float>> MG_mapData;
	static vector<float> MG_maxTrq;
	static vector<float> MG_maxRPM;

	// EV 
	static Eigen::VectorXf W1_EV;	static Eigen::VectorXf W1_EV_constr;
		static Eigen::VectorXf T1_EV; static Eigen::VectorXf T1_EV_constr;
	static Eigen::VectorXf Eta1_EV;
	static Eigen::VectorXf P1elec_EV;
	static Eigen::VectorXf PbatD_EV;
	static Eigen::VectorXf dSOC_EV;

	// HEV
	static Eigen::MatrixXf W1_HEV;
	static Eigen::MatrixXf T1_HEV;

	static Eigen::MatrixXf Eta1_HEV; // efficiency of motor power
	static Eigen::MatrixXf P1elec_HEV;
	static Eigen::MatrixXf PbatD_HEV;
	static Eigen::MatrixXf dSOC_HEV;

	// contraints
	static float W1_HEV_max; //constraints
	static float W1_EV_max; //constraints
	static Eigen::VectorXf T1_EV_max; //constraints

};

class Tool
{
public :
	// fundamental tool
	static vector<float> rpm2rs(vector<float> rpm);

	// sampling point number in FC map
	static const int NumGrid;

	// ICEMG_update
	static bool ICEMG_parameter_update();


	// Voc, Rint
	static float Voc(bool update = true);
	static float Rint(bool update = true );

	// EV of W1, T1, Eta1 estimation
	static Eigen::VectorXf W1_EV(bool update = true);
	static Eigen::VectorXf T1_EV(bool update = true);
	static Eigen::VectorXf Eta1_EV(bool update = true);

	// EV of P estimation
	static Eigen::VectorXf P1elec_EV(bool update = true);
	static Eigen::VectorXf PbatD_EV(bool update = true);
	static Eigen::VectorXf dSOC_EV(bool update = true);

	// HEV of Wi, Ti
	static Eigen::VectorXf Wi_HEV(bool update = true);
	static Eigen::MatrixXf Wi_HEV_rep(bool update = true);
	static Eigen::VectorXf Ti_max(bool update = true);
	static Eigen::MatrixXf Ti_HEV(bool update = true);
	static Eigen::MatrixXf FC_HEV(bool update = true);

	// HEV of W1 T1 Eta1
	static Eigen::MatrixXf W1_HEV(bool update = true);
	static Eigen::MatrixXf T1_HEV(bool update = true);
	static Eigen::MatrixXf Eta1_HEV(bool update = true);

	// HEV of P estimation
	static Eigen::MatrixXf P1elec_HEV(bool update = true);
	static Eigen::MatrixXf PbatD_HEV(bool update = true);
	static Eigen::MatrixXf dSOC_HEV(bool update = true);

	// constraints fucntion
	static float W1_EV_max(); //constraints
	static Eigen::VectorXf W1_EV_constr(bool update = true);
	static Eigen::VectorXf T1_EV_constr(bool update = true);
	static Eigen::VectorXf T1_EV_max(bool update = true); //constraints
	static float W1_HEV_max(); //constraints
	static float Wi_HEV_min(); //constraints
	static float Wi_HEV_max(); //constraints
	
	
};
