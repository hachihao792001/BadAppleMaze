#include "BMP.h"
#include "Wilson.h"

#define BLACK Color{0,0,0}
#define WHITE Color{255,255,255}
#define SCALE 3
#define OGWIDTH 80
#define OGHEIGHT 60

string intToString(int n) {
	stringstream ss;
	ss << n;
	return ss.str();
}

int main() {
	srand(time(NULL));
	for (int frame = 2001; frame <= 2191; frame++) {
		cout << "Frame " << frame << " ";
		Wilson wilson;
		WilsonCell*** maze = wilson.generateMaze(OGHEIGHT, OGWIDTH);
		BMPFile bmp;
		string fileName = "frames\\badApple (" + intToString(frame) + ").bmp";
		ReadBMP((char*)fileName.c_str(), bmp);
		cout << "read";
		BMPFile bmp2;
		bmp2.header = bmp.header;
		bmp2.dib = bmp.dib;
		bmp2.dib.width = bmp.dib.width * SCALE;
		bmp2.dib.height = bmp.dib.height * SCALE;

		int widthSize = bmp2.dib.width * (bmp2.dib.pixelDepth / 8);
		PixelArray pa;
		pa.paddingSize = (4 - (widthSize % 4)) % 4;
		pa.lineSize = widthSize + pa.paddingSize;
		pa.rawByteSize = pa.lineSize * bmp2.dib.height;
		bmp2.header.fileSize = 54 + bmp2.dib.width * bmp2.dib.height * 3 + pa.paddingSize * bmp2.dib.height;
		pa.rawBytes = new unsigned char[pa.rawByteSize];
		pa.rows = bmp2.dib.height;
		pa.cols = bmp2.dib.width;

		pa.pixels = new Color * [pa.rows];
		for (int i = 0; i < (int)pa.rows; i++) {
			pa.pixels[i] = new Color[pa.cols];
			for (int j = 0; j < (int)pa.cols; j++) {
				pa.pixels[i][j] = bmp.pixelArray.pixels[i / SCALE][j / SCALE];
			}
		}

		int b = 0;
		for (int i = bmp2.dib.height - 1; i >= 0; i--) {
			for (int j = 0; j < bmp2.dib.width; j++) {
				Color thisColor;

				if (bmp.pixelArray.pixels[i / SCALE][j / SCALE] != WHITE) {
					if (maze[i / SCALE][j / SCALE]->topWall && i % SCALE == 0) {
						thisColor = BLACK;
					}
					else if (maze[i / SCALE][j / SCALE]->downWall && (i + 1) % SCALE == 0) {
						thisColor = BLACK;
					}
					else if (maze[i / SCALE][j / SCALE]->leftWall && j % SCALE == 0) {
						thisColor = BLACK;
					}
					else if (maze[i / SCALE][j / SCALE]->rightWall && (j + 1) % SCALE == 0) {
						thisColor = BLACK;
					}
					else {
						thisColor = WHITE;
					}
				}
				else
					thisColor = bmp.pixelArray.pixels[i / SCALE][j / SCALE];

				if ((i < 5 && j < 5) || (i > bmp2.dib.height - 5 && j > bmp2.dib.width - 5))
					thisColor = WHITE;

				*(pa.rawBytes + (b++)) = thisColor.blue;
				*(pa.rawBytes + (b++)) = thisColor.green;
				*(pa.rawBytes + (b++)) = thisColor.red;
			}
			for (int j = 0; j < pa.paddingSize; j++)
				*(pa.rawBytes + (b++)) = (unsigned char)0;
		}
		bmp2.pixelArray = pa;
		fileName = "mazes\\maze (" + intToString(frame) + ").bmp";
		WriteBMP((char*)fileName.c_str(), bmp2);
		delete[] pa.rawBytes;
		for (int i = 0; i < (int)pa.rows; i++)
			delete[] pa.pixels[i];
		delete[] pa.pixels;

		ReadBMP((char*)fileName.c_str(), bmp2);
		int** mazeMatrix;
		int imgw, imgh;
		Pos S = { 0,0 }, E = { OGHEIGHT * SCALE - 1,OGWIDTH * SCALE - 1 };
		cout << "b";
		TurnPixelArrayToMazeMatrix(bmp2, mazeMatrix, imgw, imgh, S, E);
		cout << "a";
		int** result;
		if (SolveMaze(mazeMatrix, result, imgh, imgw, S, E)) {
			/*
			for (int i = 0; i < imgh; i++) {
				for (int j = 0; j < imgw; j++) {
					cout << result[i][j] << " ";
				}
				cout << endl;
			}
			*/
			fileName = "finals\\final (" + intToString(frame) + ").bmp";
			MakeResultBMP(fileName, result, bmp2, S, E);

			for (int i = 0; i < imgh; i++)
				delete[] result[i];
			delete[] result;

			cout << " PATH FOUND!" << endl;
		}
		else
			cout << " IMPOSSIBLE!" << endl;
		for (int i = 0; i < imgh; i++)
			delete[] mazeMatrix[i];
		delete[] mazeMatrix;
	}
}