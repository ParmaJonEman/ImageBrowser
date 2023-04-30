# Browser
### Setup

- [Install OpenCV](https://opencv.org/releases/), and replace the path in the CMAKE file on the find_package line with your new path
- If you're on Windows, [download dirent.h](https://github.com/tronkko/dirent) and put it in your linker's include folder
- Build the project with the standard CMAKE build commands

### Run

- Run the application via the command line with the following usage patten:
  - browser.exe [params] directory or bBrowser.exe directory [params]
  - The only required parameter is an unnamed parameter for the directory you wish to browse
  - Optional Params are: rows and columns, which are positive integers specifying the maximum rows and columns in the viewing window
  - If rows and columns aren't specified, the defaults are: rows=720, columns=1080
- Examples:
  - browser.exe --rows=256 --columns=256 C:\images
  - browser.exe C:\images --rows=256 --columns=256
  - browser.exe C:\images
- Iterate through the images with:
  - n, N, or Space bar for the next image
  - p or P for the previous image
  - q or Q to quit the application
