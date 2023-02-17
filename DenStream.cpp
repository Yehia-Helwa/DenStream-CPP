

#include "DenStream.hpp"
#include "nanobench.h"

DenStream::DenStream(float lambda, float epsilon, float beta, float mu){
    this->lambda=lambda;
    this->epsilon=epsilon;
    this->beta=beta;
    this->mu=mu;    
}
void DenStream::calcTp() {
    // this->Tp = (1.0 / lambda) * log((beta * mu)) / (beta * mu - 1);
    this->Tp =  log((beta * mu) / ((beta * mu) - 1));
    
}
    

void DenStream::updateDenstream(std::vector<uint16_t> input_point ,int32_t input_ts,  cv::Scalar rng_line_color ) {
    
    this->point=input_point;
    this->ts=input_ts;

    
    mergingPoint(rng_line_color);
    // std::cout<<"ts: "<<ts<<" Tp: "<<Tp<<" ts % Tp: "<<fmod(ts,Tp)<<std::endl;

}   


void DenStream::mergingPoint( cv::Scalar rng_line_color) {
   
   

    // std::cout<<"color: "<<rng_line_color[0]<<std::endl;
    // std::cout<<"rng: "<<rng.uniform(0,255)<<std::endl;
    

    // std::cout<<"Merging point: "<<this->point[0]<<"  "<<this->point[1]<<std::endl;
    // std::cout<<"Start O clusters: "<<o_micro_clusters.size()<<std::endl;
    // std::cout<<"Start P clusters: "<<p_micro_clusters.size()<<std::endl;
    
    bool merged=false;
    if (p_micro_clusters.size()>0){
      //  std::cout<<"p micro size is nonzero "<<std::endl;
        //find closest cluster index
       // std::cout<<"Finding closest p micro cluster"<<std::endl;
        int closest_p_index = findClosestCluster(this->p_micro_clusters,this->point);
        MC& closest_p_cluster = this->p_micro_clusters[closest_p_index];
       // std::cout<<"p cluster size "<<closest_p_cluster.ts_array.size()<<std::endl;
       // std::cout<<"Closest p index is: "<<closest_p_index<<std::endl;

       // std::cout<<"Appending point to closest p cluster"<<std::endl;
        closest_p_cluster.append(this->point,this->ts); 

        
        // std::cout<<"p cluster size "<<closest_p_cluster.ts_array.size()<<std::endl;
        
        // if (p_micro_clusters.size()>0){
        //     for (MC i : p_micro_clusters){
        //         i.append(this->point,this->ts); 
        //         for (int32_t t : i.ts_array){
        //             std::cout<<"P cluster array element: "<<t<<std::endl;
        //         }	
        //     }
        // }
        float r_p;
        double weight;
        std::vector<double> cf1;
        std::tie(r_p,weight,cf1)= closest_p_cluster.calculateRadius(this->ts);
        //std::cout<<"RP: "<<r_p<<" epsilon: "<<this->epsilon<<std::endl;

        if (r_p<=this->epsilon){
           //std::cout<<"rp<= epsilon, keeping p in p cluster (merged), updating parameters: "<<closest_p_index<<std::endl;
            closest_p_cluster.updateParameters();
            merged=true;
            
        }
        else{
           //  std::cout<<"NOT merging p, popping from p cluster: "<<closest_p_index<<std::endl;
            closest_p_cluster.pop();
        }
       
    }

    if (o_micro_clusters.size()>0 && (merged==false)) {
       // std::cout<<"o micro size is nonzero" <<std::endl;
        //std::cout<<"Finding closest o micro cluster"<<std::endl;
        int closest_o_index = findClosestCluster(this->o_micro_clusters,this->point);
        MC& closest_o_cluster = this->o_micro_clusters[closest_o_index];
       // std::cout<<"Closest o index is: "<<closest_o_index<<std::endl;

       // std::cout<<"Appending point to closest o cluster"<<std::endl;
        closest_o_cluster.append(this->point,this->ts); 
    
        

        float r_o;
        double weight;
        std::vector<double> cf1;
        std::tie(r_o,weight,cf1)= closest_o_cluster.calculateRadius(this->ts);
       //std::cout<<"RO: "<<r_o<<" epsilon: "<<epsilon<<std::endl;

        if (r_o<=this->epsilon){
           //std::cout<<"ro<= epsilon, keeping p in o cluster (merged), updating parameters: "<<closest_o_index<<std::endl;
            merged=true;
           //std::cout<<"Weight: "<<closest_o_cluster.weight<<"   Beta*Mu: "<<this->beta*this->mu<<std::endl;
            closest_o_cluster.updateParameters();
            
            if (closest_o_cluster.weight > (this->beta*this->mu)){
               //std::cout<<"Weight greater than beta*mu, upgrading o cluster to p cluster"<<std::endl;
                p_micro_clusters.push_back(closest_o_cluster);
                o_micro_clusters.erase(o_micro_clusters.begin()+closest_o_index);
            }
        }
        else{
            //std::cout<<"NOT merging p, popping from o cluster: "<<closest_o_index<<std::endl;
     
            closest_o_cluster.pop();
        } 
    }

    if  (merged==false){
        //std::cout<<"No merging done, creating new outlier cluster"<<std::endl;
        MC new_o_cluster(this->ts,this->lambda,rng_line_color);
        //std::cout<<"New o cluster ts: "<<new_o_cluster.creation_ts<<" current ts: "<<this->ts<<std::endl;
        new_o_cluster.append(this->point,this->ts);
        new_o_cluster.updateParameters();
        this->o_micro_clusters.push_back(new_o_cluster);
    }
    

    // decayClusters();
    pruneClusters();

    // ankerl::nanobench::Bench().run("pruning", [&]() {

    //     pruneClusters();

    // });
    
    // std::cout<<"End O clusters: "<<o_micro_clusters.size()<<std::endl;
    // std::cout<<"End P clusters: "<<p_micro_clusters.size()<<std::endl;

    
    //std::cout<<std::endl<<std::endl;
    
    this->last_ts=this->ts;


    // if (p_micro_clusters.size()>0){
    //     for (MC i : p_micro_clusters){
    //         for (int32_t t : i.ts_array){
    //             std::cout<<"P cluster array element: "<<t<<std::endl;
	//         }	
	//     }
    // }

    // if (o_micro_clusters.size()>0) {
    //     for (MC i : o_micro_clusters){
    //         for (int32_t t : i.ts_array){
    //             std::cout<<"O cluster array element: "<<t<<std::endl;
	//         }	
	//     }
     
    // }


    // if (p_micro_clusters.size()>0){
    //     for (MC i : p_micro_clusters){
            
    //         std::cout<<"P cluster center: "<<i.center[0]<<"  "<<i.center[1]<<std::endl;
	        	
	//     }
    // }
  
}

int DenStream::findClosestCluster(std::vector<MC> input_clusters, std::vector<uint16_t> input_point ) {
    // std::cout<<"finding closest cluser \n";
    int lowest_index =0;
    float lowest_distance=std::numeric_limits<float>::max();
    
    for (int index = 0; index < input_clusters.size(); ++index){
        
        //std::cout<<"Input cluster size: "<<input_clusters.size()<<std::endl;
        float distance= sqrt(pow(input_clusters[index].center[0] - input_point[0], 2) + pow(input_clusters[index].center[1] - input_point[1], 2) * 1.0);
        //std::cout<<"Calculated distance: "<<distance<<" between cluster index: "<<index<<"w/ center: "<<input_clusters[index].center[0]<<"  "<<input_clusters[index].center[1]<< " and point: "<<input_point[0]<<"  "<<input_point[1]<<std::endl;
        if (distance<lowest_distance){
        
            lowest_distance=distance;
            lowest_index=index;
            //std::cout<<"Current index is: "<<index<<std::endl;
            //std::cout<<"Lowest distance is: "<<lowest_distance<<std::endl;
            //std::cout<<"Lowest index is: "<<lowest_index<<std::endl;

        }
    }

    return lowest_index;
    
}


std::vector<MC> DenStream::getClusters(){

    return p_micro_clusters;
}

void DenStream::decayClusters(){
    

   std::cout<<"Decaying p clusters"<<std::endl;
    for (int index = 0; index < p_micro_clusters.size(); ++index){
            MC cluster = p_micro_clusters[index];
            if(this->ts>cluster.refreshed_ts){
              //  std::cout<<"Weight: "<<cluster.weight<<" will have decaying factor applied: "<<pow(2,-this->lambda*(this->ts-cluster.refreshed_ts))<<std::endl;
                cluster.weight=cluster.weight*pow(2,-this->lambda*(this->ts-cluster.refreshed_ts));
                int32_t Tp=this->ts-this->last_ts;
                float xi1 = pow(2,-this->lambda*(this->ts - cluster.creation_ts + Tp))-1;
                float xi2 = pow(2,-this->lambda*Tp)-1;
                float xi = 0.1;
             
               // std::cout<<"Weight: "<<cluster.weight<<" xi: "<<xi<<std::endl;
                if (cluster.weight<xi){
                    //std::cout<<"weight<xi, removing p cluster"<<std::endl;
                    p_micro_clusters.erase(p_micro_clusters.begin()+index);
                }
                
            }
        
        }
    std::cout<<"Decaying o clusters"<<std::endl;
    for (int index = 0; index < o_micro_clusters.size(); ++index){
            MC cluster = o_micro_clusters[index];
            if(this->ts>cluster.refreshed_ts){
                //std::cout<<"Weight: "<<cluster.weight<<" will have decaying factor applied: "<<pow(2,-this->lambda*(this->ts-cluster.refreshed_ts))<<std::endl;
                cluster.weight=cluster.weight*pow(2,-this->lambda*(this->ts-cluster.refreshed_ts));
                int32_t Tp=this->ts-this->last_ts;
                float xi1 = pow(2,-this->lambda*(this->ts - cluster.creation_ts + Tp))-1;
                float xi2 = pow(2,-this->lambda*Tp)-1;
                float xi = 0.1;

                //std::cout<<"Weight: "<<cluster.weight<<" xi: "<<xi<<std::endl;
                if (cluster.weight<xi){
                     //std::cout<<"weight<xi, removing o cluster"<<std::endl;
                    o_micro_clusters.erase(o_micro_clusters.begin()+index);
                }
                
            }
        
        }

}



void DenStream::pruneClusters(){
    

    bool completed_p_cluster=false;
    for (int index = 0; index < p_micro_clusters.size() && (completed_p_cluster==false); ++index){
            MC cluster = p_micro_clusters[index];
            if(this->ts-cluster.creation_ts>this->lambda){
              
                p_micro_clusters.erase(p_micro_clusters.begin()+index);
                completed_p_cluster=true;
                
                
            }
        
        }
    
    bool completed_o_cluster=false;
    for (int index = 0; index < o_micro_clusters.size() && (completed_o_cluster==false); ++index){
            MC cluster = o_micro_clusters[index];
            if(this->ts-cluster.creation_ts>this->lambda){
             
                o_micro_clusters.erase(o_micro_clusters.begin()+index);
                completed_o_cluster=true;
                
                
            }
        
        }

}