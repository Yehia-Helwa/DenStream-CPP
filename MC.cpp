#include <math.h>
#include "MC.hpp"
#include <numeric>
#include <tuple>

MC::MC(int32_t creation_ts, float lambda) {
    this->creation_ts=creation_ts;
	this->lambda=lambda;
	// std::cout<<"Creating new microcluster at time: "<<creation_ts <<std::endl;
  
}

void MC::append(std::vector<uint16_t> input_point, int32_t input_ts) {
	std::cout<<"Appending point and ts "<< input_ts<<std::endl;

	std::cout<<"ts size before "<<this->ts_array.size()<<std::endl;

	this->point_array.push_back(input_point);
	this->ts_array.push_back(input_ts);

	std::cout<<"ts size after "<<this->ts_array.size()<<std::endl;

}

void MC::pop() {
	//std::cout<<"Popping point and ts ":endl;

	this->point_array.pop_back();
	this->ts_array.pop_back();
}

std::tuple<float,double,std::vector<double>> MC::calculateRadius(int32_t input_ts ){
	//std::cout<<"Calculating radius "<<input_ts<<std::endl;

	std::vector<double> fading_array = fadingFunction(input_ts );
	this->weight = std::accumulate(fading_array.begin(), fading_array.end(), 0.0); //Dot product sum of fading_array
	std::vector<double> cf1 = cf1Function(fading_array);
	std::vector<double> cf2 = cf2Function(fading_array);

	float radius=sqrt(abs( (sqrt(cf2[0]*cf2[0]+cf2[1]*cf2[1])/this->weight) - pow((sqrt(cf1[0]*cf1[0]+cf1[1]*cf1[1])/this->weight),2) )); 
	
	//std::cout<<"Radius: "<<radius<<std::endl;
	std::cout<<"Weight: "<<this->weight<<std::endl;
	
	return std::make_tuple(radius,this->weight,cf1);
}

std::vector<double> MC::fadingFunction(int32_t input_ts){

	//std::cout<<"Fading ts array "<<input_ts<<std::endl;
	std::vector<double> fading_array;

	
	for (int32_t t : this->ts_array){
		fading_array.push_back(pow(2,-this->lambda*(input_ts-t)));
		//std::cout<<ts_array.size()<<std::endl;
		//std::cout<<pow(2,-this->lambda*(input_ts-t))<<std::endl;
		std::cout<<t<<std::endl;
	}

	std::cout<<std::endl<<std::endl;

	return fading_array;

}

std::vector<double> MC::cf1Function(std::vector<double> input_fading_array){

	std::vector<double> cf1_array = {0,0};

	for (int i = 0; i < ts_array.size(); i++){
		cf1_array[0] += point_array[i][0]*input_fading_array[i];
		cf1_array[1] += point_array[i][1]*input_fading_array[i];
	}


	return cf1_array;
	
}

std::vector<double> MC::cf2Function(std::vector<double> input_fading_array){
	std::vector<double> cf2_array = {0,0};

	for (int i = 0; i < ts_array.size(); i++){
		cf2_array[0] += point_array[i][0]*point_array[i][0]*input_fading_array[i];
		cf2_array[1] += point_array[i][0]*point_array[i][1]*input_fading_array[i];
	}


	return cf2_array;
}

void MC::updateParameters(int mode,int32_t input_ts, std::vector<double> input_cf1 = {0,0}, double input_weight = 0){
	/*
	Updating the weight and center parameter for the micro-cluster.
        There is two modes:
            - Mode 0 - Uses only "input_ts" and calculates the weight and cf1 from scratch.
            - Mode 1 - Simply use input_weight and input_cf1, done to avoid recomputing them.

	*/

	if (mode==0){
		std::vector<double> fading_array = fadingFunction(input_ts );
		this->weight = std::accumulate(fading_array.begin(), fading_array.end(), 0); //Dot product sum of fading_array

		std::vector<double> cf1 = cf1Function(fading_array);
		this->center[0]=cf1[0]/this->weight;
		this->center[1]=cf1[1]/this->weight;

	}

	else if(mode==1){

		this->weight=input_weight;
		this->center[0]=input_cf1[0]/this->weight;
		this->center[1]=input_cf1[1]/this->weight;

	}
	else{
		std::cout<<"Invalid mode input in updateParameters"<<std::endl;
	}
}