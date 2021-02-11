// test

#include <iostream>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/objdetect.hpp>
#include <zbar.h>
#include <Magick++.h>

#define STR(s) #s

using namespace std;
using namespace cv;
using namespace zbar;

int main ()
{
   VideoCapture vcap(0);
    Mat frame;
    if (!vcap.isOpened()) {
      cout << "Can't open video stream" << endl;
      return -1;
    }
    else {
    vcap.set(CAP_PROP_FRAME_WIDTH,320);
    vcap.set(CAP_PROP_FRAME_HEIGHT,240);

      while(1) {
        if (!vcap.read(frame)) {
          cout << "No frames" << endl;
          return -1;
        }
      imwrite("some.png", frame);

#ifdef MAGICK_HOME
    // http://www.imagemagick.org/Magick++/
    //    under Windows it is necessary to initialize the ImageMagick
    //    library prior to using the Magick++ library
    Magick::InitializeMagick(MAGICK_HOME);
#endif

    // create a reader
    ImageScanner scanner;

    // configure the reader
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

    // obtain image data
    Magick::Image magick("some.png");  // read an image file
    int width = magick.columns();   // extract dimensions
    int height = magick.rows();
    Magick::Blob blob;              // extract the raw data
    magick.modifyImage();
    magick.write(&blob, "GRAY", 8);
    const void *raw = blob.data();

    // wrap image data
    Image image(width, height, "Y800", raw, width * height);

    // scan the image for barcodes
    int n = scanner.scan(image);

    // extract results
    for(Image::SymbolIterator symbol = image.symbol_begin();
        symbol != image.symbol_end();
        ++symbol) {
        // do something useful with results
        cout << "decoded " << symbol->get_type_name()
             << " symbol \"" << symbol->get_data() << '"' << endl;
   }

    imshow("Frame", frame);
    if (waitKey(25) == 27)
        break;

    // clean up
    image.set_data(NULL, 0);
      }
    }

    return(0);
}
