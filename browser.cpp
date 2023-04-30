// browser.cpp:	This program allows the user to select a directory, number of rows, and number of columns, and
//						then traverses the directory recursively, adding all files found into a vector. Then it displays
//						all of the files that are images, one by one, scaled to the row and column values provided, while
//						maintaining	aspect ratio. Users can use 'Spacebar', 'N', or 'n' to see the next image, 'P' or 'p'
//						to see the previous, and 'Q' or 'q'	to quit the program.
// Author:				Jon Eman
// Date:				9/3/2022


#include "browser.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    String directory;
    int rows;
    int columns;
    if(parseParameters(argc, argv, &directory, &rows, &columns))
    {
        cout << "Selected resolution: " << columns << "x" << rows << "\n" << endl;
        vector<string> vectorOfFiles = list_directory(directory.c_str());
        if (showImages(vectorOfFiles, rows, columns))
        {
            cerr << "A fatal error occurred, please review the error message, make changes, and try again" << endl;
            return(1);
        }
        cout << "\n" << "Thanks for using Image Browser! " << endl;
    }
    return(0);
}

static int showImages(vector<string> vectorOfFiles, int rows, int columns)
{
    for (int i = 0; i < vectorOfFiles.size(); i++)
    {
        try
        {
            String imageName;
            String imageDirectory;
            String imageFileExtention;
            uint imageChannels;
            float memorySize;
            float fileSize;

            // Try to read the file, if the file cannot be read, throw an exception which deletes the file from the vector and continues
            Mat image = imread(vectorOfFiles[i]);
            if (image.empty())
                throw (string("Cannot open input image ") + vectorOfFiles[i]);

            // Gather and print some image meta data
            imageName = vectorOfFiles[i].substr(vectorOfFiles[i].find_last_of("/\\") + 1);
            imageDirectory = vectorOfFiles[i].substr(0, vectorOfFiles[i].find_last_of("/\\") + 1);
            imageFileExtention = vectorOfFiles[i].substr(vectorOfFiles[i].find_last_of(".\\") + 1);
            imageChannels = image.channels();
            memorySize = (image.step[0] * image.rows)/1048576.f;

            ifstream in(vectorOfFiles[i], ios::binary);
            in.seekg(0, ios::end);
            fileSize = in.tellg()/1048576.f;

            cout.setf(ios::fixed);
            cout << setprecision(2) << endl;
            cout << "Image number: " << i << endl;
            cout << "Image name: " << imageName << endl;
            cout << "Directory: " << imageDirectory << endl;;
            cout << "File extention: " << imageFileExtention << endl;
            cout << "File size: " << fileSize << " MB" << endl;
            cout << "Size in memory (uncompressed): " << memorySize << " MB" << endl;
            cout << "Channels: " << imageChannels << endl;
            cout << "Original image size: " << image.cols << "x" << image.rows << endl;

            // Scale image and print new scaled size
            Mat scaledImage = scaleImage(image, rows, columns);
            cout << "Scaled image size : " << scaledImage.cols << "x" << scaledImage.rows << "\n" << endl;

            // Show image in window
            imshow("Image Browser", scaledImage);

            // Wait for key command
            int keyCommand = waitForKeyCommand(&i);
            if (keyCommand)
            {
                return (0);
            }

        }
        catch (string& str)
        {
            // Alerts user that file is not valid, deletes file from vector, decrements the index, and continues gracefully
            cerr << "Error: " << vectorOfFiles[i] << ": " << str << "\n" << endl;
            vectorOfFiles.erase(vectorOfFiles.begin() + i);
            i--;
        }
        catch (Exception& e)
        {
            // Alerts user of an exception that cannot be handled gracefully, and terminates program
            cerr << "Error: " << vectorOfFiles[i] << ": " << e.msg << endl;
            return (1);
        }
    }
    return (0);
}

static int waitForKeyCommand(int* index)
{
    //	Waiting for one of the following keys:
    // Keycode	Keyboard Key	Functionality
    //	81			q				Quit
    //	113			Q				Quit
    //	32			n				Next
    //	78			N				Next
    //	110			Spacebar		Next
    //	80			p				Previous
    //	112			P				Previous

    int key = 0;
    while (key != 81 && key != 113 && key != 32 && key != 78 && key != 110 && key != 80 && key != 112)
    {
        key = waitKey();
    }
    switch (key)
    {
        case 81:
        case 113:
            return(1); // Quitting application gracefully by returning 1
        case 32:
        case 78:
        case 110:
            break; // Do nothing, let for-loop continue
        case 80:
        case 112:
            if (*index > 0)
            {
                *index = *index - 2; // If we're not on the first image, move the index back two spaces so that the for loop will send us to the previous image
            }
            else
            {
                cout << "You can't go the previous image, you're on the first one" << endl;
                *index = *index - 1; // If we are on the first image, move the index back one space, so that the for loop will send us to the image we're on
            }
            break;
        default:
            cerr << "There should not be a way to access this case, please report scenario to developer."; //The while loop should prevent this, but just in case, we quit gracefully
            return(1);
    }
    return 0;
}

static Mat scaleImage(Mat sourceImage, int rows, int columns)
{
    // First we get the ratio between the source rows and the goal rows, and then the same for the columns
    float rowRatio = static_cast<float>(rows) / static_cast<float>(sourceImage.rows);
    float colRatio = static_cast<float>(columns) / static_cast<float>(sourceImage.cols);
    // Then we pick the ratio that is the smallest, because we want to scale the image as little as possible
    // And we want the rows and columns to be scaled by the same amount
    float ratio = min(rowRatio, colRatio);
    cout << "Scale ratio is: " << ratio << endl;

    // The first triangle of the affine transformation is (0,0), (last column, 0), (last row, 0)
    Point2f srcTri[3];
    srcTri[0] = Point2f(0.f, 0.f);
    srcTri[1] = Point2f(sourceImage.cols - 1.f, 0.f);
    srcTri[2] = Point2f(0.f, sourceImage.rows - 1.f);


    // The second triangle of the affine transformation is (0,0), (last column * ratio, 0), (last row * ratio, 0)
    Point2f dstTri[3];
    dstTri[0] = Point2f(0.f, 0.f);
    dstTri[1] = Point2f((sourceImage.cols * ratio) - 1.f, 0.f);
    dstTri[2] = Point2f(0.f, (sourceImage.rows * ratio) - 1.f);

    // Create the transformation matrix and a blank destination matrix
    Mat warp_mat = getAffineTransform(srcTri, dstTri);
    Mat warp_dst = Mat::zeros((sourceImage.rows) * ratio, (sourceImage.cols) * ratio, sourceImage.type());

    // Execute affine transformation
    warpAffine(sourceImage, warp_dst, warp_mat, warp_dst.size());
    return warp_dst;
}

static int parseParameters(int argc, char** argv, String* directory, int* rows, int* cols) {
    String keys =
            {
                    "{help h usage ? |                            | print this message   }"
                    "{@directory      | | directory you want to use	}"
                    "{rows           |720| number of rows	}"
                    "{columns        |1080| number of columns	}"
            };

    // Get required parameters. If any are left blank, defaults are set based on the above table
    // If no directory is passed in, or if the user passes in a help param, usage info is printed
    CommandLineParser parser(argc, argv, keys);
    parser.about("Image Browser v1.0");

    if (!parser.has("@directory") || parser.has("help")) {
        parser.printMessage();
        return (0);
    }

    if (parser.get<int>("rows") < 1 || parser.get<int>("columns") < 1)
    {
        cerr << "Values for rows and columns parameters must both be positive integers" << endl;
        return(0);
    }

    *directory = parser.get<String>("@directory");
    *rows = parser.get<int>("rows");
    *cols = parser.get<int>("columns");
    return(1);
}

static vector<string> list_directory(const char* dirname)
{

    // Scan files in directory
    struct dirent** files;
    vector<string> vectorOfFiles;
    int n = scandir(dirname, &files, NULL, alphasort);

    // Empty folders return an empty vector. Application is not terminated, because this could be a child folder in a folder that does have images.
    if (n < 0)
    {
        fprintf(stderr,
                "Cannot open %s (%s)\n", dirname, strerror(errno));
        return vectorOfFiles;
    }

    // Loop through file names
    for (int i = 0; i < n; i++)
    {
        // Get pointer to file entry and create full file path
        struct dirent* ent = files[i];
        string dirnameRoot;
        if (dirname != nullptr)
        {
            dirnameRoot = dirname + string("/");
        }
        const string filePath = dirnameRoot + string(ent->d_name);

        switch (ent->d_type)
        {
            // If the entry is a file, push it onto the vectorOfFiles
            case DT_REG:
                vectorOfFiles.push_back(filePath);
                break;

                // If the entry is a directory, execute method recursively and insert results into vectorOfFiles
            case DT_DIR:
                if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                    vector<string> vectorOfFilesChildDir = list_directory(filePath.c_str());
                    vectorOfFiles.insert(vectorOfFiles.end(), vectorOfFilesChildDir.begin(), vectorOfFilesChildDir.end());
                }
                break;

            default:
                cout << "Unsupported file or folder type";
        }

    }

    // Release file names
    for (int i = 0; i < n; i++)
    {
        free(files[i]);
    }
    free(files);

    return vectorOfFiles;
}
