#include <iostream>
#include <vector>
#include <random>
#include "MC.hpp"
#include <tuple>

class DenStream {
  
  private:
    // Private attribute
    float lambda;
    float epsilon;
    float beta;
    float mu;
    int32_t Tp;

    std::vector<uint16_t> point;
    int32_t ts;
    
    std::vector<MC> o_micro_clusters;
    std::vector<MC> p_micro_clusters;

    std::vector<MC> completed_o_clusters;
    std::vector<MC> completed_p_clusters;

  public:
   
    //Methods
    DenStream(float lambda, float epsilon, float beta, float mu);
    
    void updateDenstream(std::vector<uint16_t> input_point ,int32_t input_ts);
    void mergingPoint();

    int findClosestCluster(std::vector<MC> input_cluster, std::vector<uint16_t> input_point );


    float calcTp();
    void calcRp();
    
};
