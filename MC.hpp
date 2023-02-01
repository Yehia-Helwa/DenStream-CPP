#include <vector>
#include <iostream>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class MC {

private:
    
    float lambda;
    
    // std::vector< std::vector<uint16_t> > point_array;
   
    
    

   

public:
    std::vector<int32_t> ts_array;

    double weight=0;
    std::vector<double> center = {0,0};
   

    std::vector<double> CF1 = {0,0};
    std::vector<double> CF2 = {0,0};

    std::vector<double> last_CF1 = {0,0};
    std::vector<double> last_CF2 = {0,0};

    cv::Scalar line_color;

    int32_t refreshed_ts;
    int32_t previous_refreshed_ts;

    int32_t creation_ts;
    


    MC(int32_t creation_ts, float lambda,cv::Scalar line_color);
   
  
    void append(std::vector<uint16_t> input_point, int32_t input_ts);
    void pop();

    std::tuple<float,double,std::vector<double>> calculateRadius(int32_t input_ts );

    std::vector<double> fadingFunction(int32_t input_ts);

    std::vector<double> cf1Function(std::vector<double> input_fading_array);
    std::vector<double> cf2Function(std::vector<double> input_fading_array);

    void updateParameters();  

};


