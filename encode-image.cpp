#include <Magick++.h>
#include <iostream>
#include <cstdlib>

using std::cout;
using std::string;
using namespace Magick;

void verification(Image im) {
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
}

Quantum mergeQuanta(Quantum input, Quantum secret) {
  // reduce private color to just 2 bits
  secret = (secret & 0xFF) >> 6;
  // zero out lowest 2 bits of public color
  input &= 0xFC;
  // merge
  Quantum r = input | secret;
  if (QuantumDepth == 16) {
    // for some reason, 2-byte quanta are doubled-up, like 0x7a7a
    r = (r << 8) | r;
  }
  return r;
}

Color mergeColors(Color input, Color secret) {
  input.redQuantum(mergeQuanta(input.redQuantum(), secret.redQuantum()));
  input.greenQuantum(mergeQuanta(input.greenQuantum(), secret.greenQuantum()));
  input.blueQuantum(mergeQuanta(input.blueQuantum(), secret.blueQuantum()));
  return input;
}

int main(int argc, char** argv) {
  // Initializing Library
  InitializeMagick(*argv);

  // Checking for correct command line arguments
  if (argc != 4) {
    cout << "Usage: " << argv[0]
         << " publicimage.png secretimage.png outputimage.png\n";
    exit(1);
  }

  string inputImagePath = argv[1];
  string secretImagePath = argv[2];
  string outputImagePath = argv[3];

  cout << "Loading Image ... : " << inputImagePath << '\n';
  Image input(inputImagePath);
  Geometry g = input.size();

  verification(input);

  cout << "Loading " << secretImagePath << '\n';
  Image secret(secretImagePath);

  if (secret.size() != g) {
    cout << "Images are of different dimensions\n";
    exit(-1);
  }

  verification(secret);

  // Process each pixel to merge the secret image into the public image
  for (unsigned x = 0; x < g.width(); x++) {
    for (unsigned y = 0; y < g.height(); y++) {
      input.pixelColor(x, y,
                       mergeColors(input.pixelColor(x, y),
                                   secret.pixelColor(x, y)));
    }
  }

  cout << "Writing " << outputImagePath << '\n';
  input.write(outputImagePath);
  return 0;
}
