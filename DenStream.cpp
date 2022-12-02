

#include "DenStream.hpp"


DenStream::DenStream(float lambda, float epsilon, float beta, float mu){
    this->lambda=lambda;
    this->epsilon=epsilon;
    this->beta=beta;
    this->mu=mu;    
}
float DenStream::calcTp() {
    Tp = (1.0 / lambda) * log((beta * mu)) / (beta * mu - 1);
}
    

void DenStream::updateDenstream(std::vector<uint16_t> input_point ,int32_t input_ts ) {
    
    this->point=input_point;
    this->ts=input_ts;

    
    mergingPoint();

    // if ( (ts % Tp) == 0){
    //     std::cout<<"Pruning clusters";
    // }
}   


void DenStream::mergingPoint() {
    
    if (p_micro_clusters.size()>0){
        std::cout<<"p micro size is nonzero "<<p_micro_clusters.size()<<"\n";
        //find closest cluster index
        int closest_p_index = findClosestCluster(this->p_micro_clusters,this->point);
        MC closest_p_cluster = this->p_micro_clusters[closest_p_index];

        closest_p_cluster.append(this->point,this->ts); 
        float r_p;
        double weight;
        std::vector<double> cf1;
        std::tie(r_p,weight,cf1)= closest_p_cluster.calculateRadius(this->ts);

        if (r_p<=this->epsilon){
            closest_p_cluster.updateParameters(1,0,cf1,weight);
        }
        else{
            closest_p_cluster.pop();
        }
    }

    else if (o_micro_clusters.size()>0) {
        std::cout<<"o micro size is nonzero with size: "<<o_micro_clusters.size()<<"\n";

        int closest_o_index = findClosestCluster(this->o_micro_clusters,this->point);
        MC closest_o_cluster = this->o_micro_clusters[closest_o_index];

        closest_o_cluster.append(this->point,this->ts); 
        std::cout<<"Closest o index is: "<<closest_o_index<<std::endl;

        float r_o;
        double weight;
        std::vector<double> cf1;
        std::tie(r_o,weight,cf1)= closest_o_cluster.calculateRadius(this->ts);
        // std::cout<<r_o<<std::endl;

        if (r_o<=this->epsilon){
            //std::cout<<closest_o_cluster.weight<<std::endl;
            closest_o_cluster.updateParameters(1,0,cf1,weight);
            if (closest_o_cluster.weight > this->beta*this->mu){
                p_micro_clusters.push_back(closest_o_cluster);
                o_micro_clusters.erase(o_micro_clusters.begin()+closest_o_index);
            }
        }
        else{
            closest_o_cluster.pop();
        } 
    }

    

    MC new_o_cluster(this->ts,this->lambda);
    new_o_cluster.append(this->point,this->ts);
    this->o_micro_clusters.push_back(new_o_cluster);
  
}

int DenStream::findClosestCluster(std::vector<MC> input_clusters, std::vector<uint16_t> input_point ) {
    // std::cout<<"finding closest cluser \n";
    int lowest_index =0;
    float lowest_distance=std::numeric_limits<float>::max();
    for (size_t index = 0; index < input_clusters.size(); ++index){
        float distance= sqrt(pow(input_clusters[index].center[0] - input_point[0], 2) + pow(input_clusters[index].center[1] - input_point[1], 2) * 1.0);
        if (distance<lowest_distance){
            lowest_distance=distance;
            lowest_index=index;

        }
    }

    return lowest_index;
    
}