#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"
//#include "functions.cpp"

using namespace std;

int main() {
  string filename;
  int width = 0;
  int height = 0;
  int targetWidth = 0;
  int targetHeight = 0;
  
  // Add code to validate input (Do in part 1)
  cout << "Input filename: ";
  cin >> filename;
  cout << "Input width: ";
  cin >> width;

  if (cin.fail())
  {
    cout << "Error: width is a non-integer value" << endl;
    return 1;
  }

  if (width <= 0)
  {
    cout << "Error: width must be greater than 0. You entered " << width << endl;
    return 1;
  }

  cout << "Input height: ";
  cin >> height;

  if (cin.fail())
  {
    cout << "Error: height is a non-integer value" << endl;
    return 1;
  }

  if (height <= 0)
  {
    cout << "Error: height must be greater than 0. You entered " << height << endl;
    return 1;
  }

  cout << "Input target width: ";
  cin >> targetWidth;

  if (cin.fail())
  {
    cout << "Error: target width is a non-integer value" << endl;
    return 1;
  }

  if (targetWidth <= 0)
  {
    cout << "Error: target width must be greater than 0. You entered " << targetWidth << endl;
    return 1;
  }

  cout << "Input target height: ";
  cin >> targetHeight;

  if (cin.fail())
  {
    cout << "Error: target height is a non-integer value" << endl;
    return 1;
  }

  if (targetHeight <= 0)
  {
    cout << "Error: target height must be greater than 0. You entered " << targetHeight << endl;
    return 1;
  }

  if (targetWidth > width)
  {
    cout << "Error: target width must be less than width, " << targetWidth << " is greater than " << width;
    return 1;
  }

  if (targetHeight > height)
  {
    cout << "Error: target height must be less than height, " << targetHeight << " is greater than " << height;
    return 1;
  }
  
  int originalWidth = width; // need to delete image array at end of program

  Pixel** image = createImage(width, height); // create array of size that we need
  if (image != nullptr) {
    if (loadImage(filename, image, width, height)) {
      cout << "Start carving..." << endl;
      
      // Add code to remove seams from imagcout << lowestEnergy << endl;e (Do in part 2)

      // set up output filename
      stringstream ss;
      ss << "carved" << width << "X" << height << "." << filename;
      outputImage(ss.str().c_str(), image, width, height);
    }
  
    // call last to remove the memory from the heap
    deleteImage(image, originalWidth);
  }
  // else 
  
}