#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <numeric>
#include "Matrix.h"
#include "Stock.h"
#include "DataFetcher.h"

const int BOOTSTRAP_SIZE = 30;  // Number of stocks to select for each group
const int BOOTSTRAP_ITERATIONS = 40; //Number of iterations


namespace fre {
    class Group {
        private:
            vector<Stock> missGroup, meetGroup, beatGroup;
            Stock benchmark;
            string start_time, end_time;
            vector<Matrix> metrics;
            DataFetcher benchmarkfetcher;
            int N;
            size_t thread_num=4;
        public:
            Group(string benchmark_, string start_time_, string end_time_, int N_);
            Group(string benchmark_, string start_time_, string end_time_, int N_, size_t thread_num_);
            vector<Stock> selectRandomStocks(vector<Stock> stocks, int count);
            pair<Vector, Vector> calculateMetrics(vector<Stock> stocks);
            Vector calculateAverageVector(vector<Matrix>& matrices, int index);
            Vector calculateStandardDeviation(vector<Matrix>& matrices, int index);
            void performBootstraping();

            vector<Matrix> getMetrics() {return metrics;}
            Stock getBenchmark() {return benchmark; }
            vector<vector<Stock>> divide_stocks_into_groups();
            vector<vector<Stock>> divide_stocks_into_groups_multi(size_t numThreads);
    };

}