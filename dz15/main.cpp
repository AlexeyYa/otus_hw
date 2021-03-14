// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*
    This is an example illustrating the use of the kkmeans object
    and spectral_cluster() routine from the dlib C++ Library.

    The kkmeans object is an implementation of a kernelized k-means clustering
    algorithm.  It is implemented by using the kcentroid object to represent
    each center found by the usual k-means clustering algorithm.

    So this object allows you to perform non-linear clustering in the same way
    a svm classifier finds non-linear decision surfaces.

    This example will make points from 3 classes and perform kernelized k-means
    clustering on those points.  It will also do the same thing using spectral
    clustering.

    The classes are as follows:
        - points very close to the origin
        - points on the circle of radius 10 around the origin
        - points that are on a circle of radius 4 but not around the origin at all
*/


#include <dlib/clustering.h>
#include <dlib/rand.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "export.h"

using namespace std;
using namespace dlib;

int main(int argc, char* argv[])
{
    int nclusters;

    std::string filename;
    int scale = 0;
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: kkmeans <nclusters>\n";
            return 1;
        }
        nclusters = std::atoi(argv[1]);

        if (argc > 2)
            filename = argv[2];
        if (argc == 4)
            scale = std::atoi(argv[3]);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
        return 1;
    }

    // Here we declare that our samples will be 2 dimensional column vectors.
    // (Note that if you don't know the dimensionality of your vectors at compile time
    // you can change the 2 to a 0 and then set the size at runtime)
    typedef matrix<double,2,1> sample_type;

    // Now we are making a typedef for the kind of kernel we want to use.  I picked the
    // radial basis kernel because it only has one parameter and generally gives good
    // results without much fiddling.
    typedef radial_basis_kernel<sample_type> kernel_type;


    // Here we declare an instance of the kcentroid object.  It is the object used to
    // represent each of the centers used for clustering.  The kcentroid has 3 parameters
    // you need to set.  The first argument to the constructor is the kernel we wish to
    // use.  The second is a parameter that determines the numerical accuracy with which
    // the object will perform part of the learning algorithm.  Generally, smaller values
    // give better results but cause the algorithm to attempt to use more dictionary vectors
    // (and thus run slower and use more memory).  The third argument, however, is the
    // maximum number of dictionary vectors a kcentroid is allowed to use.  So you can use
    // it to control the runtime complexity.
    kcentroid<kernel_type> kc(kernel_type(0.1),0.01, 8);

    // Now we make an instance of the kkmeans object and tell it to use kcentroid objects
    // that are configured with the parameters from the kc object we defined above.
    kkmeans<kernel_type> test(kc);

    std::vector<sample_type> samples;
    std::vector<sample_type> initial_centers;

    sample_type m;

    dlib::rand rnd;



    // make some samples near the origin
    std::string input;
    char buf[64];

    while (getline(std::cin, input))
    {
        if (input[input.length() - 1] != '\n')
            input = input + '\n';
        if (input == "q\n")
            break;
        int counter = 0, idx = 0;
        for (auto c : input)
        {
            if (c != ';' && c != '\n')
            {
                buf[counter] = c;
                counter++;
            }
            else
            {
                buf[counter] = '\0';
                m(idx) = std::atof(buf);
                counter = 0;
                idx++;
            }
        }

        // add this sample to our set of samples we will run k-means
        samples.push_back(m);
    }

    // tell the kkmeans object we made that we want to run k-means with k set to 3.
    // (i.e. we want 3 clusters)
    test.set_number_of_centers(nclusters);

    // You need to pick some initial centers for the k-means algorithm.  So here
    // we will use the dlib::pick_initial_centers() function which tries to find
    // n points that are far apart (basically).
    pick_initial_centers(nclusters, initial_centers, samples, test.get_kernel());

    // now run the k-means algorithm on our set of samples.
    test.train(samples,initial_centers);

    // now loop over all our samples and print out their predicted class.  In this example
    // all points are correctly identified.
    for (unsigned long i = 0; i < samples.size(); ++i)
    {
        cout << samples[i](0) << ';' << samples[i](1) << ';';
        cout << test(samples[i]) << "\n";
    }

    if (scale != 0)
    {
        //std::shared_ptr<ImageRGB> img = std::make_shared(new ImageRGB(100, 100));
        auto img = CreateImage(samples, nclusters, test, scale);
        write_JPEG_file(filename.c_str(), 90, img);
    }
}
