#include <math.h>
#include "MC.hpp"
#include <numeric>
#include <tuple>

MC::MC(int32_t creation_ts, float lambda, cv::Scalar line_color) {
    this->refreshed_ts=creation_ts;

	this->creation_ts=creation_ts;
	this->lambda=lambda;
	this->line_color=line_color;

	// std::cout<<"Creating new microcluster at time: "<<creation_ts <<std::endl;
  
}

void MC::append(std::vector<uint16_t> input_point, int32_t input_ts) {
	
	// for (int32_t t : this->ts_array){
	
		
	
	// }

	//std::cout<<"ts size before "<<this->ts_array.size()<<std::endl;

	this->last_CF1=this->CF1;
	this->last_CF2=this->CF2;

	this->previous_refreshed_ts=refreshed_ts;

	this->refreshed_ts=input_ts;

	// std::cout<<"CF1[0]: "<<CF1[0]<< " CF1[1]: "<<CF1[1]<<std::endl;
	// std::cout<<"CF2[0]: "<<CF1[0]<< " CF2[1]: "<<CF1[1]<<std::endl;

	this->CF1[0]=this->CF1[0]+input_point[0];
	this->CF1[1]=this->CF1[1]+input_point[1];

	this->CF2[0]=this->CF2[0]+(input_point[0]*input_point[0]);
	this->CF2[1]=this->CF2[1]+(input_point[1]*input_point[1]);

	

	// std::cout<<"Pre append weight: "<<this->weight<<std::endl;
	this->weight=this->weight+1;
	// std::cout<<"Post append weight: "<<this->weight<<std::endl;

	// std::cout<<"CF1[0]: "<<CF1[0]<< " CF1[1]: "<<CF1[1]<<std::endl;
	// std::cout<<"CF2[0]: "<<CF1[0]<< " CF2[1]: "<<CF1[1]<<std::endl;


	// this->point_array.push_back(input_point); 
	// this->ts_array.push_back(input_ts);

	//std::cout<<"ts size after "<<this->ts_array.size()<<std::endl;

	
	//std::cout<<"Appended point and ts: "<< input_ts<<std::endl;

	// for (int32_t t : this->ts_array){
	// 	std::cout<<"Post append array element: "<<t<<std::endl;
		
	
	// }

}

void MC::pop() {
	//std::cout<<"Popping point and ts: "<<this->ts_array.back()<<std::endl;

	this->CF1=this->last_CF1;
	this->CF2=this->last_CF2;

	// std::cout<<"Pre pop weight: "<<this->weight<<std::endl;
	this->weight=this->weight-1;
	// std::cout<<"Post pop weight: "<<this->weight<<std::endl;

	this->refreshed_ts=this->previous_refreshed_ts;
}

std::tuple<float,double,std::vector<double>> MC::calculateRadius(int32_t input_ts){
	//std::cout<<"Calculating radius "<<input_ts<<std::endl;

	// std::vector<double> fading_array = fadingFunction(input_ts );
	// this->weight = std::accumulate(fading_array.begin(), fading_array.end(), 0.0); //Dot product sum of fading_array

	// std::cout<<"CF1[0]: "<<CF1[0]<< " CF1[1]: "<<CF1[1]<<" Weight: "<<this->weight<<std::endl;
	// std::cout<<"CF2[0]: "<<CF2[0]<< " CF2[1]: "<<CF2[1]<<" Weight: "<<this->weight<<std::endl;

	std::vector<double> R1= {this->CF2[0]/this->weight, this->CF2[1]/this->weight};
	std::vector<double> R2= {(this->CF1[0]/this->weight)*(this->CF1[0]/this->weight),(this->CF1[1]/this->weight)*(this->CF1[1]/this->weight)};
	float radius=sqrt(abs(R1[0]-R2[0]+R1[1]-R2[1])); 
	
	// std::cout<<"R1[0]: "<<R1[0]<< " R1[1]: "<<R1[1]<<std::endl;
	// std::cout<<"R2[0]: "<<R2[0]<< " R2[1]: "<<R2[1]<<std::endl;
	// std::cout<<"Radius: "<<radius<<std::endl;
	
	// std::cout<<"Fading array size: "<<fading_array.size()<<std::endl;
	// for (int i = 0; i < fading_array.size(); i++){
	// 	std::cout<<"Fading array element: "<<fading_array[i]<<std::endl;
	// }
	// std::cout<<"Weight: "<<this->weight<<std::endl;
	
	return std::make_tuple(radius,this->weight,this->CF1);
}

std::vector<double> MC::fadingFunction(int32_t input_ts){

	//std::cout<<"Fading ts array "<<input_ts<<std::endl;
	std::vector<double> fading_array;

	
	for (int32_t t : this->ts_array){
		//std::cout<<"Fading input ts array element: "<<t<<std::endl;
		fading_array.push_back(pow(2,-this->lambda*(input_ts-t)));
		//std::cout<<ts_array.size()<<std::endl;
		//std::cout<<pow(2,-this->lambda*(input_ts-t))<<std::endl;
	
	}

	

	return fading_array;

}


//NEED TO REMOVE cf1 and cf2functions, deperecated
std::vector<double> MC::cf1Function(std::vector<double> input_fading_array){

	std::vector<double> cf1_array = {0,0};

	// for (int i = 0; i < ts_array.size(); i++){
	// 	cf1_array[0] += point_array[i][0]*input_fading_array[i];
	// 	cf1_array[1] += point_array[i][1]*input_fading_array[i];
	// }


	return cf1_array;
	
}

std::vector<double> MC::cf2Function(std::vector<double> input_fading_array){
	std::vector<double> cf2_array = {0,0};

	// for (int i = 0; i < ts_array.size(); i++){
	// 	cf2_array[0] += point_array[i][0]*point_array[i][0]*input_fading_array[i];
	// 	cf2_array[1] += point_array[i][0]*point_array[i][1]*input_fading_array[i];
	// }


	return cf2_array;
}

void MC::updateParameters(){
	/*
	Updating the weight and center parameter for the micro-cluster.
        There is two modes:
            - Mode 0 - Uses only "input_ts" and calculates the weight and cf1 from scratch.
            - Mode 1 - Simply use input_weight and input_cf1, done to avoid recomputing them.

	*/

	// if (mode==0){
	// 	std::vector<double> fading_array = fadingFunction(input_ts );
	// 	this->weight = std::accumulate(fading_array.begin(), fading_array.end(), 0); //Dot product sum of fading_array

	// 	std::vector<double> cf1 = cf1Function(fading_array);
	// 	this->center[0]=cf1[0]/this->weight;
	// 	this->center[1]=cf1[1]/this->weight;

	// }


	this->center[0]=this->CF1[0]/this->weight;
	this->center[1]=this->CF1[1]/this->weight;

}
	
	