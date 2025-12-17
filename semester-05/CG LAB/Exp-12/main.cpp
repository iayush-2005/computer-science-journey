#include <iostream>
using namespace std;

// Class to represent a point
class Point {
    int x, y;
public:
    Point(int xv=0, int yv=0) {
        x = xv;
        y = yv;
    }
    
    // Input coordinates
    void input(int index) {
        cout << "Enter coordinates of Point " << index+1 << " (x y): ";
        cin >> x >> y;
    }
    
    // Apply scaling
    void scale(int sx, int sy) {
        x *= sx;
        y *= sy;
    }
    
    // Display coordinates
    void display(int index) {
        cout << "P" << index+1 << " (" << x << ", " << y << ")" << endl;
    }
};

int main() {
    int n, sx, sy;

    cout << "Enter number of vertices of the shape: ";
    cin >> n;

    Point shape[50];   // Array for storing points

    // Input vertices
    for(int i=0; i<n; i++) {
        shape[i].input(i);
    }

    cout << "\nEnter scaling factors (sx sy): ";
    cin >> sx >> sy;

    cout << "\nOriginal Coordinates:\n";
    for(int i=0; i<n; i++) {
        shape[i].display(i);
    }

    // Apply scaling
    for(int i=0; i<n; i++) {
        shape[i].scale(sx, sy);
    }

    cout << "\nAfter Scaling:\n";
    for(int i=0; i<n; i++) {
        shape[i].display(i);
    }

    return 0;
}
