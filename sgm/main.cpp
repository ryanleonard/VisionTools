#include <iostream>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace std;
using namespace cv;

vector<vector<long>> CensusTransform(Mat &image, int window_width=7, int window_height=7){
    // Declare some variables early
    unsigned long census = 0;
    int bit;
    Mat CensusImage = Mat::zeros(image.size(), CV_8UC1);
    vector<vector<long>> census_array(image.rows, (vector<long>(image.cols, 0)));

    // For each pixel in the image, get relationship from center to neighbors.
    // Adjust bounds for a border that wont fit with window size
    for(int row = window_height/2; row < image.rows - window_height/2; row++){
        for(int col = window_height/2; col < image.cols - window_height/2; col++){
            unsigned long census_left = 0;
            int bit_idx = 0;
//            int census_array[window_width * window_height - 1];
            census = 0;

            // Iterate through pixels in window
            for(int x = row - window_width/2; x < row + window_width/2; x++){
                for (int y = col - window_height/2; y < col + window_height/2; y++){
                    // Dont compare the center pixel to itself.
                    if (x + y == window_height * window_width / 2)
                        continue;
                    // TODO: why are we left shifting the census value?
                    census <<= 1;

                    if (image.at<uchar>(x,y) > image.at<uchar>(row, col))
                        bit = 1;
                    else
                        bit = 0;
                    // TODO: Why are we specifically doing bitwise or? is there no other opeation that could have the same effect?
                    // Is this essentially storing a bit array?
                    census = census | bit;
//                    census_array[bit_idx] = bit;
                    bit_idx += 1;
                }
            }
            // Populate our new matrix with our census values.
            if (row == 8 && col == 8)
                cout << census << endl;
            census_array[row][col] = census;
//            CensusImage.ptr<uchar>(row)[col] = (short) census;
        }
    }

    return census_array;
}


void computeHammingDistance(vector<vector<long>> &left_census,
                            vector<vector<long>> &right_census,
                            vector<vector<vector<unsigned long>>> &pixel_cost,
                            int window_width = 7, int window_height=7, int disparity_range=64){
    // Now that we have performed the concensus transofrm on each of the input images, we need to compute the
    // Hamming distance along multple offsets.



    assert(("Input Images are not the same size", left_census.size() == right_census.size()));
    for(int row = window_height/2; row < left_census.size() - window_height/2; row++){
        for (int col = window_width/2; col < left_census[0].size() - window_width/2; col++){
            for (int d=0; d<disparity_range; d++){
                int census_right = 0;
                int census_left = left_census[row][col];
                // Each element of left/right census is essentially an array of bits that can be used for hamming dist
                // if the image width - some radius is greater than the current column + disparity
                // TODO: Figure out exactly what this if statement is doing.
                // Answer: it is comparing the value of each pixel in the left image to the value in the right image
                // and each disparityrange pixel to the right
                if (col + d < left_census[0].size() - window_width/2){
                    census_right = right_census[row][col+d];
                }else{
                    census_right = right_census[row][col - disparity_range + d];
                }
                // compute bitwise exclusive or on the left and right census values
                // TODO: Why is this performing an exclusive or with a value that is set to 0?
                long answer = (long)(census_left ^ census_right); // This line computes hamming distance
                short distance = 0;
                while(answer){
                    distance += 1;
                    answer &= answer - 1;
                }
                pixel_cost[row][col][d] = distance;
            }
        }
    }
}


int main() {
    vector<vector<vector<unsigned long>>> pixel_cost; // Width x Height x Disp
    vector<vector<vector<unsigned long>>> aggregated_cost; // Width x Height x Disp
    vector<vector<vector<vector<int>>>> single_path_cost; // Path_Nos x Width x Height x Disp


    Mat left_image = imread("/home/rleonard/CLionProjects/sgm/cones/im2.png", CV_LOAD_IMAGE_ANYCOLOR);
    Mat right_image = imread("/home/rleonard/CLionProjects/sgm/cones/im6.png", CV_LOAD_IMAGE_ANYCOLOR);

    // Census Transform
    Mat gray_left, gray_right;
    cvtColor(left_image, gray_left, COLOR_RGB2GRAY);
    blur(gray_left, gray_left, Size(3, 3));
    cvtColor(right_image, gray_right, COLOR_RGB2GRAY);
    blur(gray_right, gray_right, Size(3, 3));
    vector<vector<long>> census_left = CensusTransform(gray_left);
    vector<vector<long>> census_right = CensusTransform(gray_right);
    cout << census_left[8][8] << endl;
    cout << census_right[8][8] << endl;
    computeHammingDistance(census_left, census_right, pixel_cost);
    return 0;

    // Show some results
//    imshow("Image", left_image);
//    imshow("Census7", census_left);
//    waitKey(0);


    // Compute hamming distances on resulting images


}
