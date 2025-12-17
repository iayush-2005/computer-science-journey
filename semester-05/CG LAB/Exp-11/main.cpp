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
    
    // Apply translation
    void translate(int tx, int ty) {
        x += tx;
        y += ty;
    }
    
    // Display coordinates
    void display(int index) {
        cout << "P" << index+1 << " (" << x << ", " << y << ")" << endl;
    }
};

int main() {
    int n, tx, ty;

    cout << "Enter number of vertices of the shape: ";
    cin >> n;

    Point shape[50];   // Array for storing points

    // Input vertices
    for(int i=0; i<n; i++) {
        shape[i].input(i);
    }

    cout << "\nEnter translation vector (tx ty): ";
    cin >> tx >> ty;

    cout << "\nOriginal Coordinates:\n";
    for(int i=0; i<n; i++) {
        shape[i].display(i);
    }

    // Apply translation
    for(int i=0; i<n; i++) {
        shape[i].translate(tx, ty);
    }

    cout << "\nAfter Translation:\n";
    for(int i=0; i<n; i++) {
        shape[i].display(i);
    }

    return 0;
}
