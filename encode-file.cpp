#include <Magick++.h>
#include <iostream>
#include <cassert>
#include <fstream>
#include <bitset>
#include <cstdlib>
#include <ctime>

using std::cout;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::endl;
using namespace Magick;

char byte;          // Extracting data in the form of bytes.
unsigned bitsleft;  // Data bits that is to be encoded.

/* Returns number of lines of input file */
int countLines(ifstream& in) {
  int numLines = 0;
  string unused;
  while (std::getline(in, unused)) ++numLines;
  return numLines;
}

int countStrength(ifstream& in) {
  char character;
  int count = 0;
  while (in.get(character)) ++count;
  return count;
}

Quantum encodeBitPair(ifstream& in, char& byte, unsigned& bitsleft, Quantum q) {
  Quantum result;
  if (bitsleft == 0) {
    if (in.get(byte)) {
      // cout << "Read byte: " << bitset<8>(byte) << '\n';
      bitsleft = 8;
    } else {
      result = rand() % 4;
    }
  }
  if (bitsleft > 0) {
    bitsleft -= 2;
    result = (byte & (3 << bitsleft)) >> bitsleft;
  }
  result = (result << 8) | result;
  Quantum merged = (q & ~0x0303) | result;
  // cout << "Next two: " << bitset<2>(result) << " merged into "
  //      << hex << q << " is " << merged << '\n';
  return merged;
}

int main(int argc, char** argv) {
  // Initializing Library
  InitializeMagick(*argv);
  srand(time(nullptr));

  // Checking for correct command line arguments
  if (argc != 4) {
    cout << "Usage: " << argv[0]
         << " inputimage.png secret.data outputimage.png\n";
    exit(-1);
  }

  // Extracting arguments
  string inputPath = argv[1];
  string secretPath = argv[2];
  string outputPath = argv[3];

  cout << "Loading image ..." << '\n';
  Image im(inputPath);
  Geometry g = im.size();

  // LSB Steganography works only on uncompressed images so we have to make
  // sure image is of correct format i.e 24 bit color representation and type
  // true color.
  if (!(im.depth() <= 8)) {
    cout << "Input Image depth is of improper format\n";
    exit(-1);
  }
  if (im.type() != TrueColorType) {
    cout << "Input Image should be of TrueColourType\n";
    exit(-1);
  }

  ifstream in2(secretPath.c_str(), ios::binary);

  cout << "Opening File : " << secretPath << " ......\n";

  if (!(in2.good())) {
    cout << "File " << secretPath.c_str() << " does not exist. Aborting...\n";
    exit(-1);
  }

  int nline = countLines(in2);
  in2.close();

  ofstream fout;
  fout.open("lines.txt");  // stores number of lines so that we can extract
                            // exact data later.
  fout << nline << endl;
  fout.close();

  ifstream in3(secretPath.c_str(), ios::binary);
  int strength = countStrength(in3);
  in3.close();
  ifstream in(secretPath.c_str(), ios::binary);

  ofstream fout2;
  fout2.open("strength.txt");  // stores strength so that we can extract exact
                                // data later.
  fout2 << strength << endl;
  fout2.close();

  byte = 0;
  bitsleft = 0;

  // Pixel Processing of Source Image.
  for (unsigned y = 0; y < g.height(); y++) {
    for (unsigned x = 0; x < g.width(); x++) {
      Color c = im.pixelColor(x, y);
      c.redQuantum(encodeBitPair(in, byte, bitsleft, c.redQuantum()));
      c.greenQuantum(encodeBitPair(in, byte, bitsleft, c.greenQuantum()));
      c.blueQuantum(encodeBitPair(in, byte, bitsleft, c.blueQuantum()));
      im.pixelColor(x, y, c);
    }
  }

  if (in) {
    cout << "Source file was too large, All data couldn't be encoded"
         << "\n";
  }
  in.close();
  cout << "Writing " << outputPath << '\n';
  im.write(outputPath);

  return 0;
}
