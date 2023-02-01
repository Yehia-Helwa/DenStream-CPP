#include <iostream>
#include <vector>
#include <random>
#include "MC.hpp"
#include <tuple>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class DenStream {
  
  private:
    // Private attribute
    float lambda;
    float epsilon;
    float beta;
    float mu;
    float Tp;

    std::vector<uint16_t> point;
    int32_t ts;
    
    std::vector<MC> o_micro_clusters;
    std::vector<MC> p_micro_clusters;

    std::vector<MC> completed_o_clusters;
    std::vector<MC> completed_p_clusters;

  public:

   int32_t last_ts;
   
    //Methods
    DenStream(float lambda, float epsilon, float beta, float mu);
    
    void updateDenstream(std::vector<uint16_t> input_point ,int32_t input_ts, cv::Scalar rng_line_color);
    void mergingPoint( cv::Scalar rng_line_color);

    int findClosestCluster(std::vector<MC> input_cluster, std::vector<uint16_t> input_point );


    void calcTp();
    void calcRp();

    std::vector<MC> getClusters();
    

    void decayClusters();

    void pruneClusters();
};
