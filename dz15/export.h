/*! @file matrix.h
 *  @brief Описание n-мерной разреженной матрицы
 */

#ifndef EXPORT_H
#define EXPORT_H

#define BITS_IN_JSAMPLE 8
#define HAVE_UNSIGNED_CHAR


#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>
#include <memory>
#include <vector>
#include <dlib/image_processing/generic_image.h>
#include <dlib/image_transforms.h>

struct ImageRGB
{
    ImageRGB(int width, int height) : image_width(width), image_height(height)
    {
        image_buffer = new JSAMPLE[image_width * image_height * 3];
    }

    ~ImageRGB()
    {
        delete [] image_buffer;
    }

    void Clear()
    {
        for (int i = 0; i < image_width * image_height * 3; i++)
        {
            image_buffer[i] = 255;
        }
    }
    JSAMPLE* image_buffer;
    int image_width;
    int image_height;
};

// Dlib image functions
long num_rows(const std::shared_ptr<ImageRGB>& img) { return img->image_height; }
long num_columns(const std::shared_ptr<ImageRGB>& img) { return img->image_width; }
void set_image_size(std::shared_ptr<ImageRGB>& img, long rows, long cols)
{
    img->image_height = rows;
    img->image_width = cols;
}
void* image_data(std::shared_ptr<ImageRGB>& img) { return img->image_buffer; }
const void* image_data(const std::shared_ptr<ImageRGB>& img) { return img->image_buffer; }
long width_step(const std::shared_ptr<ImageRGB>& img) { return img->image_width * 3 * sizeof(JSAMPLE); }
void swap(std::shared_ptr<ImageRGB>& a, std::shared_ptr<ImageRGB>& b) { std::shared_ptr<ImageRGB> tmp = a; a = b; b = tmp; }

namespace dlib
{
    template <>
    struct image_traits<std::shared_ptr<ImageRGB>>
    {
        typedef rgb_pixel pixel_type;
    };
}

template <typename sample_type, typename tester>
std::shared_ptr<ImageRGB> CreateImage(const std::vector<sample_type>& samples, int nclusters, const tester& test, const int scale)
{
    std::shared_ptr<ImageRGB> img = std::make_shared<ImageRGB>(200 * scale, 200 * scale);
    dlib::fill_rect(img, {static_cast<unsigned long>(200 * scale), static_cast<unsigned long>(200 * scale)}, 255);
    dlib::draw_line(100 * scale, 0, 100 * scale, 200 * scale, img, 127);
    dlib::draw_line(0, 100 * scale, 200 * scale, 100 * scale, img, 127);

    for (auto& point : samples)
    {
        dlib::draw_solid_circle(img, {(100 + point(0)) * scale, (100 - point(1)) * scale}, 2 * scale,
            dlib::hsi_pixel{static_cast<unsigned char>(255 /(nclusters + 1) * test(point)),
                            static_cast<unsigned char>(255),
                            static_cast<unsigned char>(127)});
    }

    return img;
}

void write_JPEG_file(const char *filename, const int quality, std::shared_ptr<ImageRGB> image)
{
  /* This struct contains the JPEG compression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   * It is possible to have several such structures, representing multiple
   * compression/decompression processes, in existence at once.  We refer
   * to any one struct (and its associated working data) as a "JPEG object".
   */
  struct jpeg_compress_struct cinfo;
  /* This struct represents a JPEG error handler.  It is declared separately
   * because applications often want to supply a specialized error handler
   * (see the second half of this file for an example).  But here we just
   * take the easy way out and use the standard error handler, which will
   * print a message on stderr and call exit() if compression fails.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct jpeg_error_mgr jerr;
  /* More stuff */
  FILE *outfile;                /* target file */
  JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */
  int row_stride;               /* physical row width in image buffer */

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
   * step fails.  (Unlikely, but it could happen if you are out of memory.)
   * This routine fills in the contents of struct jerr, and returns jerr's
   * address which we place into the link field in cinfo.
   */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */

  /* Here we use the library-supplied code to send compressed data to a
   * stdio stream.  You can also write your own code to do something else.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to write binary files.
   */
  if ((outfile = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    exit(1);
  }
  jpeg_stdio_dest(&cinfo, outfile);

  /* Step 3: set parameters for compression */

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
  cinfo.image_width = image->image_width;      /* image width and height, in pixels */
  cinfo.image_height = image->image_height;
  cinfo.input_components = 3;           /* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB;       /* colorspace of input image */
  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  row_stride = image->image_width * 3; /* JSAMPLEs per row in image_buffer */
  while (cinfo.next_scanline < cinfo.image_height) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
    row_pointer[0] = &image->image_buffer[cinfo.next_scanline * row_stride];
    (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  /* Step 6: Finish compression */
  jpeg_finish_compress(&cinfo);
  /* After finish_compress, we can close the output file. */
  fclose(outfile);

  /* Step 7: release JPEG compression object */
  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

  /* And we're done! */
}

#endif
