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
 

 




int main()
{
    std::string fname = "data/shapes.csv";

    std::vector<uint16_t> x_array;
    std::vector<uint16_t> y_array;
    std::vector<int32_t> ts_array;

    int event_array_length=0;

    std::vector<std::string> row;
    std::string line, word;
    
    std::fstream file (fname, std::ios::in);
    if(file.is_open()){
        while(getline(file, line)){
            row.clear();
            
            std::stringstream str(line);
            
            while(getline(str, word, ','))
            row.push_back(word);
            x_array.push_back(stoi(row[0]));
            y_array.push_back(stoi(row[1]));
            event_array_length++;
        }
    }
    else
        std::cout<<"Could not open the file\n";
    


    // DenStream den=DenStream(0.001,35,0.1,0); //shapes
    DenStream den=DenStream(0.001,35,0.2,6); //mill


    den.calcTp();
     
    int chunk_size = 500;

    cv::Scalar line_Color(255, 0, 0);//Color of the circle
    int thickness = 2;//thickens of the line
    cv::Point center;//Declaring the center point
    int radius;
    for(int i=0;i<event_array_length/chunk_size;i++)
    {
        
        cv::Mat cvEvents(240, 320, CV_8UC3, cv::Vec3b{127, 127, 127});
        for(int j=0;j<chunk_size;j++){
        //   cout<<i<<" "<<j<< " " << event_array_length/chunk_size<<"\n";
            uint16_t current_x = x_array[(i*chunk_size)+j];
            uint16_t current_y = y_array[(i*chunk_size)+j];
            std::vector<uint16_t> current_point = {current_x,current_y};
            // int32_t current_ts = ts_array[(i*chunk_size)+j]
            // int32_t current_ts = (i*chunk_size)+j; 
            int32_t current_ts = (i*chunk_size)+j; 

            cvEvents.at<cv::Vec3b>(current_y, current_x) = -1 ? cv::Vec3b{255, 255, 255} : cv::Vec3b{0, 0, 0};
            den.updateDenstream(current_point,current_ts+1);
        }

        std::vector<MC> plot_clusters = den.getClusters();
        for (MC cluster : plot_clusters){

            center=cv::Point(cluster.center[0],cluster.center[1]);
            radius=10;
            cv::circle(cvEvents, center,radius, line_Color, thickness);
        
        
        }
        

        

        cv::imshow("PLOT_EVENTS", cvEvents);
        cv::waitKey(1);   
    
    }
    cv::destroyAllWindows();
    return 0;
}
 