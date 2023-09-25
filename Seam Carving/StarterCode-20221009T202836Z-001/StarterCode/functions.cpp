#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

Pixel** createImage(int width, int height) {
  cout << "Start createImage... " << endl;
  
  // Create a one dimensional array on the heap of pointers to Pixels 
  //    that has width elements (i.e. the number of columns)
  Pixel** image = new Pixel*[width];
  
  bool fail = false;
  
  for (int i=0; i < width; ++i) { // loop through each column
    // assign that column to a one dimensional array on the heap of Pixels
    //  that has height elements (i.e. the number of rows)
    image[i] = new Pixel[height];
    
    if (image[i] == nullptr) { // failed to allocate
      fail = true;
    }
  }
  
  if (fail) { // if any allocation fails, clean up and avoid memory leak
    // deallocate any arrays created in for loop
    for (int i=0; i < width; ++i) {
      delete [] image[i]; // deleting nullptr is not a problem
    }
    delete [] image; // delete array of pointers
    return nullptr;
  }
  
  // initialize cells
  //cout << "Initializing cells..." << endl;
  for (int row=0; row<height; ++row) {
    for (int col=0; col<width; ++col) {
      //cout << "(" << col << ", " << row << ")" << endl;
      image[col][row] = { 0, 0, 0 };
    }
  }
  cout << "End createImage... " << endl;
  return image;
}

void deleteImage(Pixel** image, int width) {
  cout << "Start deleteImage..." << endl;
  // avoid memory leak by deleting the array
  for (int i=0; i<width; ++i) {
    delete [] image[i]; // delete each individual array placed on the heap
  }
  delete [] image;
  image = nullptr;
}

// implement for part 1

int* createSeam(int length) {

  int *seam = new int[length];

  for (int i = 0; i < length; i++)
  {
    seam[i] = 0;
  }

  return seam;
}

void deleteSeam(int* seam) {

  delete[]seam;

}

bool loadImage(string filename, Pixel** image, int width, int height) 
{
  ifstream myFile(filename);

  if (!myFile.is_open())
  {
    cout << "Error: failed to open input file - " << filename << endl;
    return false;
  }

  string firstLine;

  myFile >> firstLine;

  if (firstLine != "P3")
  {
    cout << "Error: type is " << firstLine << " instead of P3" << endl;
    return false;
  }

  int newWidth = 0;

  myFile >> newWidth;

  if (myFile.fail())
  {
    cout << "Error: read non-integer value" << endl;
    return false;
  }

  if (newWidth != width)
  {
    cout << "Error: input width (" << width << ") does not match value in file ("  << newWidth << ")" << endl;
    return false;
  }

  int newHeight;

  myFile >> newHeight;

  if (myFile.fail())
  {
    cout << "Error: read non-integer value" << endl;
    return false;
  }

  if (newHeight != height)
  {
    cout << "Error: input height (" << height << ") does not match value in file (" << newHeight << ")" << endl;
    return false;
  }

  int maxValue;

  myFile >> maxValue;

  for (int row = 0; row < height; row++)
  {
    for (int collin = 0; collin < width; collin++)
    {
      myFile >> image[collin][row].r;

        if (myFile.eof())
        {
          cout << "Error: not enough color values" << endl;
          return false;
        }

        if(myFile.fail())
        {
          cout << "Error: read non-integer value" << endl;
          return false;
        }

        if (image[collin][row].r < 0 || image[collin][row].r > maxValue)
        {
          cout << "Error: invalid color value " << image[collin][row].r << endl;
          return false;
        }

      myFile >> image[collin][row].g;

        if (myFile.eof())
        {
          cout << "Error: not enough color values" << endl;
          return false;
        }

        if(myFile.fail())
        {
          cout << "Error: read non-integer value" << endl;
          return false;
        }

        if (image[collin][row].g < 0 || image[collin][row].g > maxValue)
        {
          cout << "Error: invalid color value " << image[collin][row].g << endl;
          return false;
        }

      myFile >> image[collin][row].b;

        if(myFile.fail() && !myFile.eof())
        {
          cout << "Error: read non-integer value" << endl;
          return false;
        }

        if (image[collin][row].b < 0 || image[collin][row].b > maxValue)
        {
          cout << "Error: invalid color value " << image[collin][row].b << endl;
          return false;
        }

    }
  }

  string peen;

  myFile >> peen;

  if (!myFile.fail())
  {
    cout << "Error: too many color values" << endl;
    return false;
  }

  myFile.close();

  return true;
}

bool outputImage(string filename, const Pixel*const* image, int width, int height) {

  ofstream myFile(filename);
  string type = "P3";

  if (myFile.is_open() == false)
  {
    cout << "Error: failed to open output file " << filename << endl;
    return false;
  }

  myFile << type << endl;

  myFile << width << " " << height;

  int maxValue = 255;
  myFile << maxValue << endl;

  for (int rome = 0; rome < height; rome++)
  {
    for (int colosseum = 0; colosseum < width; colosseum++)
    {
      myFile << image[colosseum][rome].r << " ";
      myFile << image[colosseum][rome].g << " ";
      myFile << image[colosseum][rome].b << endl;
    }
  }

  myFile.close();

  return true;
}

int energy(const Pixel*const* image, int x, int y, int width, int height) {

  int Rx, Ry, Gx, Gy, Bx, By, energydX, energydY, totalEnergy;

  if (width < 3)
  {
    energydX = 0;

    if (y == 0)
    {
      Ry = abs(image[x][height - 1].r - image[x][y + 1].r);
      Gy = abs(image[x][height - 1].g - image[x][y + 1].g);
      By = abs(image[x][height - 1].b - image[x][y + 1].b);
      energydY = pow(Ry, 2) + pow(Gy, 2) + pow(By, 2);
    }
    else if (y == (height - 1))
    {
      Ry = abs(image[x][0].r - image[x][y - 1].r);
      Gy = abs(image[x][0].g - image[x][y - 1].g);
      By = abs(image[x][0].b - image[x][y - 1].b);
      energydY = pow(Ry, 2) + pow(Gy, 2) + pow(By, 2);
    }
    else
    {
        Ry = abs(image[x][y + 1].r - image[x][y - 1].r);
        Gy = abs(image[x][y + 1].g - image[x][y - 1].g);
        By = abs(image[x][y + 1].b - image[x][y - 1].b);
        energydY = pow(Ry, 2) + pow(Gy, 2) + pow(By, 2);
    }

    totalEnergy = energydX + energydY;

    return totalEnergy;
  }

  if (height < 3)
  {
    energydY = 0;

    if (x == 0)
    {
      Rx = abs(image[width - 1][y].r - image[x + 1][y].r);
      Gx = abs(image[width - 1][y].g - image[x + 1][y].g);
      Bx = abs(image[width - 1][y].b - image[x + 1][y].b);
      energydX = pow(Rx, 2) + pow(Gx, 2) + pow(Bx, 2);
    }
    else if (x == (width - 1))
    {
      Rx = abs(image[0][y].r - image[x - 1][y].r);
      Gx = abs(image[0][y].g - image[x - 1][y].g);
      Bx = abs(image[0][y].b - image[x - 1][y].b);
      energydX = pow(Rx, 2) + pow(Gx, 2) + pow(Bx, 2);
    }
    else
    {
        Rx = abs(image[x + 1][y].r - image[x - 1][y].r);
        Gx = abs(image[x + 1][y].g - image[x - 1][y].g);
        Bx = abs(image[x + 1][y].b - image[x - 1][y].b);
        energydX = pow(Rx, 2) + pow(Gx, 2) + pow(Bx, 2);
    }

  totalEnergy = energydX + energydY;

  return totalEnergy;

  }

  if (x == 0)
  {
    Rx = abs(image[width - 1][y].r - image[x + 1][y].r);
    Gx = abs(image[width - 1][y].g - image[x + 1][y].g);
    Bx = abs(image[width - 1][y].b - image[x + 1][y].b);
    energydX = pow(Rx, 2) + pow(Gx, 2) + pow(Bx, 2);
  }
  else if (x == (width - 1))
  {
    Rx = abs(image[0][y].r - image[x - 1][y].r);
    Gx = abs(image[0][y].g - image[x - 1][y].g);
    Bx = abs(image[0][y].b - image[x - 1][y].b);
    energydX = pow(Rx, 2) + pow(Gx, 2) + pow(Bx, 2);
  }
  else
  {
      Rx = abs(image[x + 1][y].r - image[x - 1][y].r);
      Gx = abs(image[x + 1][y].g - image[x - 1][y].g);
      Bx = abs(image[x + 1][y].b - image[x - 1][y].b);
      energydX = pow(Rx, 2) + pow(Gx, 2) + pow(Bx, 2);
  }

  if (y == 0)
  {
    Ry = abs(image[x][height - 1].r - image[x][y + 1].r);
    Gy = abs(image[x][height - 1].g - image[x][y + 1].g);
    By = abs(image[x][height - 1].b - image[x][y + 1].b);
    energydY = pow(Ry, 2) + pow(Gy, 2) + pow(By, 2);
  }
  else if (y == (height - 1))
  {
    Ry = abs(image[x][0].r - image[x][y - 1].r);
    Gy = abs(image[x][0].g - image[x][y - 1].g);
    By = abs(image[x][0].b - image[x][y - 1].b);
    energydY = pow(Ry, 2) + pow(Gy, 2) + pow(By, 2);
  }
  else
  {
      Ry = abs(image[x][y + 1].r - image[x][y - 1].r);
      Gy = abs(image[x][y + 1].g - image[x][y - 1].g);
      By = abs(image[x][y + 1].b - image[x][y - 1].b);
      energydY = pow(Ry, 2) + pow(Gy, 2) + pow(By, 2);
  }

  totalEnergy = energydX + energydY;

  return totalEnergy;
}

// implement for part 2

// uncomment for part 2
 

int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int *seam) {

  seam[0] = start_col;

  int totalEnergy = energy(image, start_col, 0, width, height);

  int rightEnergy, leftEnergy, middleEnergy;

  int newColumn = start_col;

  int minEnergy = 0;

  for (int rome = 1; rome < height; rome++)
  {
    leftEnergy = INT32_MAX;
    rightEnergy = INT32_MAX;
    middleEnergy = energy(image, newColumn, rome, width, height);

    if (newColumn - 1 >= 0)
    {
      rightEnergy = energy(image, (newColumn - 1), rome, width, height);
    }

    if (width > (newColumn + 1))
    {
      leftEnergy = energy(image, (newColumn + 1), rome, width, height);
    }

    if ((middleEnergy <= rightEnergy) && (middleEnergy <= leftEnergy))
    {
      minEnergy = middleEnergy;
    }
    else if (rightEnergy >= leftEnergy)
    {
      minEnergy = leftEnergy;
      newColumn = (newColumn + 1);
    }
    else
    {
      minEnergy = rightEnergy;
      newColumn = (newColumn - 1);
    }

    seam[rome] = newColumn;

    totalEnergy += minEnergy;
  }

  return totalEnergy;
}

int* findMinVerticalSeam(Pixel** image, int width, int height) {

  int *minimum = createSeam(height);

  int distance = loadVerticalSeam(image, 0, width, height, minimum);

  int *newMinimum = createSeam(height);

  int newDistance = -1;

  int *count;

  for (int collin = 0; collin < width; collin++)
  {
    newDistance = loadVerticalSeam(image, collin, width, height, newMinimum);

    if (distance > newDistance)
    {
      distance = newDistance;

      count = newMinimum;

      newMinimum = minimum;

      minimum = count;
    }
  }

  deleteSeam(newMinimum); 

  return minimum;
}

void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam) {

  width = width - 1;

  for (int rome = 0; rome < height; rome++)
  {
    int vS = verticalSeam[rome];

    for (int collin = vS; collin < width; collin++)
    {
      image[collin][rome] = image[collin + 1][rome];
    }
  }

}

/*int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam) {

  seam[0] = start_row;

  int totalEnergy = energy(image, start_row, 0, width, height);

  int rightEnergy, leftEnergy, middleEnergy;

  int newRow = start_row;

  int minEnergy = 0;

  for (int rome = 1; rome < height; rome++)
  {
    leftEnergy = INT32_MAX;
    rightEnergy = INT32_MAX;
    middleEnergy = energy(image, newRow, rome, width, height);

    if (newRow - 1 >= 0)
    {
      rightEnergy = energy(image, (newRow - 1), rome, width, height);
    }

    if (width > (newRow + 1))
    {
      leftEnergy = energy(image, (newRow + 1), rome, width, height);
    }

    if ((middleEnergy <= rightEnergy) && (middleEnergy <= leftEnergy))
    {
      minEnergy = middleEnergy;
    }
    else if (rightEnergy >= leftEnergy)
    {
      minEnergy = leftEnergy;
      newRow = (newRow + 1);
    }
    else
    {
      minEnergy = rightEnergy;
      newRow = (newRow - 1);
    }

    seam[rome] = newRow;

    totalEnergy += minEnergy;
  }

  return totalEnergy;
}

int* findMinHorizontalSeam(Pixel** image, int width, int height) {
  return nullptr;
}

void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam) {
}*/


