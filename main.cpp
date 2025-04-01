#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <numeric>
#include "Stock.h"
#include "DataFetcher.h"
#include "Group.h"
#include "Matrix.h"

using namespace std;
using namespace fre;

int main() {
    int N = 40;
    //Initialise our Group object with our stocks
    Group group = Group("IWV", "2024-01-01", "2024-11-30", N, 10);
    group.performBootstraping();

    // Print the matrix for the first group
    // cout << group.getMetrics()[0] << endl;


    return 0;
}