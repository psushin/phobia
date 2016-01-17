#include <SoftwareSerial.h>

#define WIDTH 28
#define HEIGHT 7

/////////////////////////////////////////////////////////////

void cleanImage(bool data[WIDTH][HEIGHT])
{
  for (int i = 0; i < WIDTH; ++i) {
    for (int j = 0; j < HEIGHT; ++j) {
      data[i][j] = false;
    }
  }
}

void createImage(bool data[WIDTH][HEIGHT], TImage images[], int imageCount, int distance = 1)
{
  cleanImage(data);
  
  int sumWidth = (imageCount - 1) * distance;
  for (int i = 0; i < imageCount; ++i) {
    sumWidth += images[i].Width;
  }

  int margin = (WIDTH - sumWidth) / 2;

  int column = margin;
  for (int i = 0; i < imageCount; ++i) {
    if (i > 0) {
      column += distance;
    }
   
    for (int c = 0; c < images[i].Width; ++c) {
      for (int r = 0; r < HEIGHT; ++r) {
        data[column + c][r] = images[i].Data[c][r];
      }
    }
    column += images[i].Width;
  }
}

/////////////////////////////////////////////////////////////

struct TDot
{
  int Column = 0;
  int Row = 0;

  TDot()
  { }

  TDot(int column, int row)
    : Column(column)
    , Row(row)
  { }
};

/////////////////////////////////////////////////////////////

class TFlipDotDisplay
{
public:
  TFlipDotDisplay(int txPin)
    : Serial_(30, txPin)
  {
    Serial_.begin(9600); 
  }

  void SetImage(bool image[WIDTH][HEIGHT])
  {
    for (int i = 0; i < WIDTH; ++i) {
      for (int j = 0; j < HEIGHT; ++j) {
        Data_[i][j] = image[i][j];
      }
    }
    DoUpdate();
  }

  void CleanImage()
  {
    cleanImage(Data_);
    DoUpdate();
  }

  void Shuffle(int shuffleTime = 5000, int stepTime = 100, int flipsPerStep = 3)
  {
    for (int step = 0; step < shuffleTime / stepTime + 1; ++step) {
      for (int flip = 0; flip < flipsPerStep; ++flip) {
        int row = random(HEIGHT);
        int column = random(WIDTH);
        Data_[column][row] = !Data_[column][row];
      }
  
      DoUpdate();
      delay(stepTime);
    } 
  }

  void ChaoticMorph(bool image[WIDTH][HEIGHT], int stepTime = 100, int flipsPerStep = 3)
  {
    int badDotCount = 0;
    TDot badDots[WIDTH * HEIGHT];
  
    for (int i = 0; i < WIDTH; ++i) {
      for (int j = 0; j < HEIGHT; ++j) {
        if (Data_[i][j] != image[i][j]) {
          badDots[badDotCount] = TDot(i, j);
          ++badDotCount;
        }
      }
    }
  
    Serial.print("Bad dot count ");
    Serial.println(badDotCount);
  
    while (badDotCount > 0) {
      int stepDotCount = 0;
      while (stepDotCount < flipsPerStep && badDotCount > 0) {
        int dotIndex = random(badDotCount);
        TDot dot = badDots[dotIndex];
        Data_[dot.Column][dot.Row] = !Data_[dot.Column][dot.Row];
        
        badDots[dotIndex] = badDots[badDotCount - 1];
        --badDotCount;
        ++stepDotCount; 
      }
  
      Serial.print("Bad dot count ");
      Serial.println(badDotCount);
  
      DoUpdate();
      delay(stepTime);
    }
  }

  void LeftShutter(bool image[WIDTH][HEIGHT], int stepTime = 50)
  {
    for (int i = 0; i < WIDTH; ++i) {
      SetColumnToWhite(i);
      if (i > 0) {
        CopyColumn(image, i - 1);
      }

      DoUpdate();
      delay(stepTime);
    }

    CopyColumn(image, WIDTH - 1);
    DoUpdate();
    delay(stepTime);
  }

  void TopShutter(bool image[WIDTH][HEIGHT], int stepTime = 100)
  {
    for (int i = 0; i < HEIGHT; ++i) {
      SetRowToWhite(i);
      if (i > 0) {
        CopyRow(image, i - 1);
      }

      DoUpdate();
      delay(stepTime);
    }

    CopyRow(image, HEIGHT - 1);
    DoUpdate();
    delay(stepTime);
  }

  void RightShutter(bool image[WIDTH][HEIGHT], int stepTime = 50)
  {
    for (int i = WIDTH - 1; i >= 0; --i) {
      SetColumnToWhite(i);
      if (i < WIDTH - 1) {
        CopyColumn(image, i + 1);
      }

      DoUpdate();
      delay(stepTime);
    }

    CopyColumn(image, 0);
    DoUpdate();
    delay(stepTime);
  }

  void OuterArrows(bool image[WIDTH][HEIGHT], int stepTime = 100)
  {
    const int arrowWidth = HEIGHT / 2 + 1;
    const int stepCount = WIDTH / 2 + arrowWidth;
    
    const int leftArrowStart = WIDTH / 2 - 1;
    const int rightArrowStart = WIDTH / 2;
    
    for (int i = 0; i < stepCount; ++i) {
      // Left arrow.
      int leftPointer = leftArrowStart - i;
      for (int arrowColumn = 0; arrowColumn < arrowWidth; ++arrowColumn) {
        // Counting from pointer to wings, left to right.
        int column = leftPointer + arrowColumn;
        if (column >= 0 && column <= leftArrowStart) {
          Data_[column][HEIGHT / 2 - arrowColumn] = true;
          Data_[column][HEIGHT / 2 + arrowColumn] = true;
        }
      }

      for (int copyColumn = 0; copyColumn < arrowWidth; ++copyColumn) {
        int column = leftPointer + copyColumn + 1;
        if (column >= 0 && column <= leftArrowStart) {
          Data_[column][HEIGHT / 2 - copyColumn] = image[column][HEIGHT / 2 - copyColumn];
          Data_[column][HEIGHT / 2 + copyColumn] = image[column][HEIGHT / 2 + copyColumn];
        }
      }

      // Right arrow.
      int rightPointer = rightArrowStart + i;
      for (int arrowColumn = 0; arrowColumn < arrowWidth; ++arrowColumn) {
        // Counting from pointer to wings, right to left.
        int column = rightPointer - arrowColumn;
        if (column >= rightArrowStart && column < WIDTH) {
          Data_[column][HEIGHT / 2 - arrowColumn] = true;
          Data_[column][HEIGHT / 2 + arrowColumn] = true;
        }
      }

      for (int copyColumn = 0; copyColumn < arrowWidth; ++copyColumn) {
        int column = rightPointer - copyColumn - 1;
        if (column >= rightArrowStart && column < WIDTH) {
          Data_[column][HEIGHT / 2 - copyColumn] = image[column][HEIGHT / 2 - copyColumn];
          Data_[column][HEIGHT / 2 + copyColumn] = image[column][HEIGHT / 2 + copyColumn];
        }
      }

      DoUpdate();
      delay(stepTime);
    }
  }

  void InnerArrows(bool image[WIDTH][HEIGHT], int stepTime = 100)
  {
    const int arrowWidth = HEIGHT / 2 + 1;
    const int stepCount = WIDTH / 2 + arrowWidth;
    
    const int leftArrowEnd = WIDTH / 2 - 1;
    const int rightArrowEnd = WIDTH / 2;
    
    for (int i = 0; i < stepCount; ++i) {
      // Left arrow.
      int leftPointer = i;
      for (int arrowColumn = 0; arrowColumn < arrowWidth; ++arrowColumn) {
        // Counting from pointer to wings, left to right.
        int column = leftPointer - arrowColumn;
        if (column >= 0 && column <= leftArrowEnd) {
          Data_[column][HEIGHT / 2 - arrowColumn] = true;
          Data_[column][HEIGHT / 2 + arrowColumn] = true;
        }
      }

      for (int copyColumn = 0; copyColumn < arrowWidth; ++copyColumn) {
        int column = leftPointer - copyColumn - 1;
        if (column >= 0 && column <= leftArrowEnd) {
          Data_[column][HEIGHT / 2 - copyColumn] = image[column][HEIGHT / 2 - copyColumn];
          Data_[column][HEIGHT / 2 + copyColumn] = image[column][HEIGHT / 2 + copyColumn];
        }
      }

      // Right arrow.
      int rightPointer = WIDTH - i - 1;
      for (int arrowColumn = 0; arrowColumn < arrowWidth; ++arrowColumn) {
        // Counting from pointer to wings, right to left.
        int column = rightPointer + arrowColumn;
        if (column >= rightArrowEnd && column < WIDTH) {
          Data_[column][HEIGHT / 2 - arrowColumn] = true;
          Data_[column][HEIGHT / 2 + arrowColumn] = true;
        }
      }

      for (int copyColumn = 0; copyColumn < arrowWidth; ++copyColumn) {
        int column = rightPointer + copyColumn + 1;
        if (column >= rightArrowEnd && column < WIDTH) {
          Data_[column][HEIGHT / 2 - copyColumn] = image[column][HEIGHT / 2 - copyColumn];
          Data_[column][HEIGHT / 2 + copyColumn] = image[column][HEIGHT / 2 + copyColumn];
        }
      }

      DoUpdate();
      delay(stepTime);
    }
  }
  
private:  
  bool Data_[WIDTH][HEIGHT];
  SoftwareSerial Serial_;

  void CopyColumn(bool image[WIDTH][HEIGHT], int column)
  {
    for (int row = 0; row < HEIGHT; ++row) {
      Data_[column][row] = image[column][row];
    }
  }

  void CopyRow(bool image[WIDTH][HEIGHT], int row)
  {
    for (int column = 0; column < WIDTH; ++column) {
      Data_[column][row] = image[column][row];
    }
  }

  void SetColumnToWhite(int column)
  {
    for (int row = 0; row < HEIGHT; ++row) {
      Data_[column][row] = true;
    }
  }

  void SetRowToWhite(int row)
  {
    for (int column = 0; column < WIDTH; ++column) {
      Data_[column][row] = true;
    }
  }
  
  void DoUpdate()
  {
    byte header[] = {0x80, 0x83, 0x00};
    Serial_.write(header, 3);
  
    for (int i = 0; i < WIDTH; ++i) {
      byte data = 0;
      for (int j = 0; j < HEIGHT; ++j) {
        data |= byte(Data_[i][j]) << j;
      }
      Serial_.write(data);
    }
  
    // End of message.
    Serial_.write(0x8F);  
  }
};

/////////////////////////////////////////////////////////////
