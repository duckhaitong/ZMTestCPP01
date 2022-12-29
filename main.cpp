#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

#define ELAPSED_TIME(MSG, ...) auto start = std::chrono::steady_clock::now(); __VA_ARGS__ ; auto end = std::chrono::steady_clock::now(); std::cout << "Elapsed time in milliseconds [" << MSG << "] : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";

/**
 * Perform DFS algorithm to get bounding box for each connected component
 *
 * @param image 2d array contains pixel values
 * @param visited 2d array is used to mark whether a pixel is visited or not
 * @param i, j the position of current pixel
 * @param up, down, left, right indicates (up, left) and (down, right) points of bounding box
 */
void dfs(std::vector<std::vector<int> > &image, std::vector<std::vector<bool> > &visited, int i, int j, int &up, int &down, int &left, int &right) {
    if (i < 0 || i >= image.size() || j < 0 || j >= image[0].size() || image[i][j] == 0 || visited[i][j]) return;
    visited[i][j] = true;
    up = std::min(i, up);
    down = std::max(i, down);
    left = std::min(j, left);
    right = std::max(j, right);
    dfs(image, visited, i - 1, j, up, down, left, right);
    dfs(image, visited, i + 1, j, up, down, left, right);
    dfs(image, visited, i, j - 1, up, down, left, right);
    dfs(image, visited, i, j + 1, up, down, left, right);
}

/**
 * Perform DFS algorithm to find all connected components and get bounding box for each of them
 *
 * @param image 2d array contains pixel values
 * @param visited 2d array is used to mark whether a pixel is visited or not
 * @param n the n-th image
 * @return bounding_boxes bounding boxes of objects in image
 */
std::vector<std::vector<int> > findBoundingBox(std::vector<std::vector<int> > &image, std::vector<std::vector<bool> > &visited, int n) {
    std::vector<std::vector<int> > bounding_boxes;

    for (int i = 0; i < image.size(); i++) {
        for (int j = 0; j < image[0].size(); j++) {
            if (image[i][j] > 0 && visited[i][j] == false) {
                int up = i, down = i, left = j, right = j;
                dfs(image, visited, i, j, up, down, left, right);
                std::vector<int> bounding_box = {up, left, down - up, right - left};
                bounding_boxes.push_back(bounding_box);
            }
        }
    }

    return bounding_boxes;
}

int main() {
    auto start = std::chrono::steady_clock::now();
    const size_t n_images = 10;
    const size_t width = 800, height = 600;

    //open file
    std::ifstream infile("data.bin");

    // get length of file
    infile.seekg(0, std::ios::end);
    std::streamsize length = infile.tellg();
    infile.seekg(0, std::ios::beg);

    auto* buffer = new char[n_images * height * width];

    // read whole file
    infile.read(buffer, length);
    std::vector<int> pixel_values;
    for (int i = 0; i < length; ++i) {
        pixel_values.push_back(static_cast<int>(static_cast<unsigned char>(buffer[i]))); // cast char to unsigned char numbers(0 - 255)
    }

    std::vector<std::vector<int> > image(height, std::vector<int>(width, 0));
    std::vector<std::vector<bool> > visited(height, std::vector<bool>(width, false));

    // identify the bounding boxes and write to output.txt
    std::ofstream outfile("output.txt");
    for (int i = 0; i < length; i++) {
        int row = i / width;
        row %= height;
        int col = i % width;
        // std::cout << i << " " << row << " " << col << "\n";
        image[row][col] = pixel_values[i]; // convert 1d array [4800000] to 2d array [10 x 600 x800]

        if ((i + 1) % (width * height) == 0) { // check if image is fully updated
            int n = (i + 1) / (width * height);
            std::vector<std::vector<int> > bounding_boxes = findBoundingBox(image, visited, n);

            outfile << "Bounding boxes (top, left, height, width) of " << n << "-th image is: ";
            std::cout << "Bounding boxes (top, left, height, width) of " << n << "-th image is: ";

            for (const auto & bounding_box : bounding_boxes) {
                outfile << "(" << bounding_box[0] << ", " << bounding_box[1] << ", " << bounding_box[2] << ", " << bounding_box[3] << ") ";
                std::cout << "(" << bounding_box[0] << ", " << bounding_box[1] << ", " << bounding_box[2] << ", " << bounding_box[3] << ") ";
            }
            outfile << "\n";
            std::cout << "\n";
            
        }
    }

    outfile.close();
    delete[] buffer;
    auto end = std::chrono::steady_clock::now(); 
    std::cout << "Elapsed time in milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";
    return 0;
}
