#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>

#include "DenStream.hpp"
#include "nanobench.h"
 
// #define LAMBDA 0.25E-2
//  #define LAMBDA 0
//  #define EPSILON 7
//  #define BETA 0.5
//  #define MU 30




int main()
{
    int chunk_size = 1000;
    int plot_wait_ms=1;
    // chunk_size=1;
    // event_array_length=100000;

  
    // std::string fname = "wall_dots_static_nf_time"; //8.54 seconds long
    // #define LAMBDA 100000
    // #define EPSILON 7
    // #define BETA 0.5
    // #define MU 30


    // std::string fname = "mill_nf_time";
    // #define LAMBDA 100000
    // #define EPSILON 7
    // #define BETA 0.5
    // #define MU 30

    std::string fname = "fan_max_knf_time";
    #define LAMBDA 10000
    #define EPSILON 7
    #define BETA 0.5
    #define MU 30

    //std::string fname = "lunar_calendar_static_nf_time";
  
    std::string path= "data/"+fname+".csv";

    std::cout<<"Opening file: "<<path<<std::endl;

    std::vector<uint16_t> x_array;
    std::vector<uint16_t> y_array;
    std::vector<int32_t> ts_array;

    int event_array_length=0;

    std::vector<std::string> row;
    std::string line, word;
    
    std::fstream file (path, std::ios::in);
    if(file.is_open()){
        while(getline(file, line)){
            row.clear();
            
            std::stringstream str(line);
            
            while(getline(str, word, ','))
            row.push_back(word);
            x_array.push_back(stoi(row[0]));
            y_array.push_back(stoi(row[1]));
            ts_array.push_back(stoi(row[2]));
            
            event_array_length++;
        }
    }
    else
        std::cout<<"Could not open the file\n";
    


   
    DenStream den=DenStream(LAMBDA,EPSILON,BETA,MU); //mill



   
     
   
  


    int thickness = 2;//thickens of the line
    cv::Point center;//Declaring the center point
    int radius;
    int rng_color_seed=12345;

    cv::RNG rng(rng_color_seed);

    int count=0;
    for(int i=0;i<event_array_length/chunk_size;i++)
    {
        
        cv::Mat cvEvents(240, 320, CV_8UC3, cv::Vec3b{127, 127, 127});
        for(int j=0;j<chunk_size;j++){
            
        //   cout<<i<<" "<<j<< " " << event_array_length/chunk_size<<"\n";
            uint16_t current_x = x_array[(i*chunk_size)+j];
            uint16_t current_y = y_array[(i*chunk_size)+j];
            std::vector<uint16_t> current_point = {current_x,current_y};
            int32_t current_ts = ts_array[(i*chunk_size)+j];
           

            cvEvents.at<cv::Vec3b>(current_y, current_x) = -1 ? cv::Vec3b{255, 255, 255} : cv::Vec3b{0, 0, 0};
           
            //std::cout<<"point: "<<current_point[0]<<"  "<<current_point[1]<<std::endl;
            
            cv::Scalar rng_line_color(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));

            ankerl::nanobench::Bench().run("updating", [&]() {

                den.updateDenstream(current_point,current_ts,rng_line_color);

            });
            // den.updateDenstream(current_point,current_ts,rng_line_color);
        }

        std::vector<MC> plot_clusters = den.getClusters();
        for (MC cluster : plot_clusters){

            center=cv::Point(cluster.center[0],cluster.center[1]);
            radius=2;
            cv::circle(cvEvents, center,radius, cluster.line_color, thickness);
            // cv::circle(cvEvents,cv::Point(10,10),50, line_Color, thickness);
        
        }
        

        

        cv::imshow("PLOT_EVENTS", cvEvents);
        //cv::imwrite("output/"+fname+std::to_string(count)+".jpg", cvEvents); // A JPG FILE IS BEING SAVED
        cv::waitKey(plot_wait_ms);   

        count++;
    
    }
    cv::destroyAllWindows();
    return 0;
}
 