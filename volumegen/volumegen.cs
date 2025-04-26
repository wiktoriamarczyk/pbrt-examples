/* This program converts a stack of medical .tif images (e.g., CT or MRI scans) into volumetric data compatible with PBRT v4. It computes a 3D density grid by mapping the brightness of each pixel to a density value, applies intensity scaling and thresholding, and writes the result into a volume.pbrt file using a uniform grid representation for realistic volumetric rendering. */

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.IO;

namespace volumegen
{
    class Program
    {
        static void Main(string[] args)
        {
            const string CT_directory = "CT_Head_TIF";
            const string MR_directory = "MR_Brain_TIF";

            string chosenDirectory = CT_directory;

            // current directory with input images
            string inputDir = Path.Combine(Directory.GetCurrentDirectory(), chosenDirectory);

            const int resolution = 256;
            const int numOfPictures = 99;

            int nx = resolution;
            int ny = resolution;
            int nz = numOfPictures;

            string outFileName = "volume.pbrt";
            Console.WriteLine("Volumegen for PBRTv4");

            using (StreamWriter writer = new StreamWriter(outFileName))
            {
                writer.WriteLine("MakeNamedMedium \"smoke\"");
                writer.WriteLine("    \"integer nx\" [{0}]", nx);
                writer.WriteLine("    \"integer ny\" [{0}]", ny);
                writer.WriteLine("    \"integer nz\" [{0}]", nz);

                writer.WriteLine("    \"point3 p0\" [  -1.000000 -1.000000 -1.000000 ] \"point3 p1\" [ 1.000000 1.000000 1.000000 ]");
                writer.WriteLine("    \"string type\" [ \"uniformgrid\" ]");
                writer.WriteLine("    \"spectrum sigma_a\" [300 20 830 20] \"spectrum sigma_s\" [300 160 830 160]");
                writer.WriteLine("    \"float density\" [");

                // Load images
                List<Bitmap> images = new List<Bitmap>();
                var files = Directory.GetFiles(inputDir, "*.tif");
                Array.Sort(files);

                foreach (string file in files)
                {
                    images.Add(new Bitmap(file));
                }

                float intensityScale = 8f;
                float minThreshold = 0.1f;
                float maxThreshold = 0.8f;

                if (chosenDirectory == MR_directory)
                    intensityScale = 6f;

                for (int z = 0; z < nz; z++)
                {
                    Bitmap image = images[z];
                    for (int y = 0; y < ny; y++)
                    {
                        for (int x = 0; x < nx; x++)
                        {
                            Color pixel = image.GetPixel(x, y);
                            float brightness = pixel.GetBrightness();
                            float density = brightness;

                            // scale density
                            density *= intensityScale;
                            // apply brightness thresholds
                            density = Clamp(density, minThreshold, maxThreshold);
                            // normalize density to range 0-1
                            density = (density - minThreshold) / (maxThreshold - minThreshold);

                            writer.Write(density.ToString(CultureInfo.InvariantCulture) + " ");
                        }
                    }
                }

                writer.WriteLine("]");
            }
        }

        static float Clamp(float value, float min, float max)
        {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }
    }
}
