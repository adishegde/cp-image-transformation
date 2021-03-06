#include "image/brightness_histogram.h"
#include "image/image.h"
#include "image/pixel.h"
#include "image/color.h"
#include<algorithm>
#include<vector>

// Default Constructor
BrightnessHistogram::BrightnessHistogram(){

}

// Constructs brightness histogram from image
// Range of frequency vector is from 0 to maximum brightness
// of a. Thus the size of _frequency is a.getMaxColorVal()
BrightnessHistogram::BrightnessHistogram(const Image &a, int smoothFactor) :
    _frequency(a.getMaxColorVal())
{
    // Iterate through each pixel to find brightness value
    for(int i = 0; i < a.getHeight(); ++i){
        for(int j = 0; j < a.getWidth(); ++j){
            _frequency[a[i][j].brightness()]++;
        }
    }

    // Unnecessary computation if smoothFactor = 0
    if(smoothFactor != 0){
        // To maintain sum of elements in 2 * smoothFactor window
        int sumWindow = 0;
        for(int i = 0; i < 2 * smoothFactor; ++i)
            sumWindow += _frequency[i];

        // Index where iteration should end
        int endIndexAvg = _frequency.size() - smoothFactor - 1;
        for(int i = smoothFactor; i < endIndexAvg; ++i){
            _frequency[i] = sumWindow / smoothFactor;

            sumWindow -= _frequency[i - smoothFactor];
            sumWindow += _frequency[i + smoothFactor];
        }

        // Write value for end index
        _frequency[endIndexAvg] = sumWindow / smoothFactor;
    }
}

// Returns an image that is a visualisation of the histogram
Image BrightnessHistogram::outputHistogram(
        int scaleHeight,
        int window,
        int lowerBoundMinima,
        bool markThresholds
) const{
    // Height of image is the scaled maximum frequency
    int heightOfImage = *std::max_element(
        _frequency.begin(),
        _frequency.end()
    );
    heightOfImage /= scaleHeight;

    // Create image object for storing histogram
    Image histogram(_frequency.size(), heightOfImage);

    // Some color constants
    const Color BLACK(0, 0, 0), BLUE(0, 0, 255), WHITE(255, 255, 255), RED(255, 0, 0);

    // Iterate throught the pixels of the histogram
    // and color them appropriately
    for(int i = 0; i < heightOfImage; ++i){
        for(int j = 0; j < _frequency.size(); ++j){
            if(i <= heightOfImage - (_frequency[j] / scaleHeight)){
                histogram[i][j].setColor(WHITE);
            }
            else{
                histogram[i][j].setColor(BLACK);
            }
        }
    }

    // If markThesholds is true then
    // mark thresholds values with a red line
    if(markThresholds){
        std::vector<int> thresholds(getThresholds(window, lowerBoundMinima));
        for(int i = 0; i < heightOfImage; ++i){
            for(int j = 0; j < thresholds.size(); ++j){
                if(i <= heightOfImage - (_frequency[thresholds[j]] / scaleHeight)){
                    histogram[i][thresholds[j]].setColor(BLUE);
                }
                else{
                    histogram[i][thresholds[j]].setColor(RED);
                }
            }
        }
    }

    return histogram;
}

std::vector<int> BrightnessHistogram::getThresholds(int window, int lowerBoundMinima) const{
    // This will be populated with the threshold values
    std::vector<int> thresholds;

    // For each value of the brightness value
    // check window values on left and right of it
    // This value is threshold if it is minimum in this window
    // Frequencies smaller than or equal lowerBoundMinima ignored
    for(int i = 0, j; i < _frequency.size(); ++i){
        if(_frequency[i] <= lowerBoundMinima) continue;

        for(j = 1; j <= window; ++j){
            if(
                i-j >= 0 &&
                _frequency[i-j] > lowerBoundMinima &&
                _frequency[i-j] < _frequency[i]
            ){
                break;
            }

            if(
                i+j < _frequency.size() &&
                _frequency[i+j] > lowerBoundMinima &&
                _frequency[i+j] < _frequency[i]
            ){
                break;
            }
        }

        if(j > window){
            thresholds.push_back(i);
        }
    }

    return thresholds;
}


std::vector<int> BrightnessHistogram::getFrequencyList() const{
    return _frequency;
}

int BrightnessHistogram::operator[](int index) const{
    // For out of range values return 0
    if(index > _frequency.size()) return 0;

    return _frequency[index];
}
