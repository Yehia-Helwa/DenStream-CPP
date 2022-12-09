#include <vector>
#include <iostream>

class MC {

private:
    int32_t creation_ts;
    float lambda;
    
    std::vector< std::vector<uint16_t> > point_array;
   
    
    

   

public:
    std::vector<int32_t> ts_array;

    double weight;
    std::vector<double> center = {0,0};


    MC(int32_t creation_ts, float lambda);
   
  
    void append(std::vector<uint16_t> input_point, int32_t input_ts);
    void pop();

    std::tuple<float,double,std::vector<double>> calculateRadius(int32_t input_ts );

    std::vector<double> fadingFunction(int32_t input_ts);

    std::vector<double> cf1Function(std::vector<double> input_fading_array);
    std::vector<double> cf2Function(std::vector<double> input_fading_array);

    void updateParameters(int mode,int32_t input_ts, std::vector<double> input_cf1, double input_weight);  

};


