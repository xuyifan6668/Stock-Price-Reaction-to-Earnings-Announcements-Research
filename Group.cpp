#include "Group.h"
#include "Matrix.h"
#include <vector>
#include <string>
#include <iostream>
#include <chrono>  // for time counter
#include <thread>
#include <algorithm> // for std::shuffle
#include <random> 
#include <cmath>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace fre {
    Group::Group(string benchmark_, string start_time, string end_time, int N_){
        N=N_;
        benchmark.setSymbol(benchmark_);
        benchmarkfetcher.fetchBenchmarkData(benchmark, start_time, end_time);
        vector<vector<Stock>> tp;
        tp = divide_stocks_into_groups();
        missGroup = tp[0];
        meetGroup = tp[1];
        beatGroup = tp[2];
    }
    Group::Group(string benchmark_, string start_time, string end_time, int N_, size_t thread_num_){
        N=N_;
        thread_num = thread_num_;
        benchmark.setSymbol(benchmark_);
        benchmarkfetcher.fetchBenchmarkData(benchmark, start_time, end_time);
        vector<vector<Stock>> tp;
        tp = divide_stocks_into_groups_multi(thread_num);
        missGroup = tp[0];
        meetGroup = tp[1];
        beatGroup = tp[2];
    }
    // Function to randomly select stocks from a given range
    vector<Stock> Group::selectRandomStocks(vector<Stock> stocks, int count) {
        // Check for valid input
        if (count <= 0 || stocks.empty()) {
            return {};  // Return an empty vector if invalid input
        }

        // Limit count to the size of the stocks vector
        count = std::min(count, static_cast<int>(stocks.size()));

        // Create a random number generator
        std::random_device rd;
        std::mt19937 gen(rd());

        // Shuffle the stocks vector
        std::shuffle(stocks.begin(), stocks.end(), gen);

        // Select the first 'count' elements
        vector<Stock> selection(stocks.begin(), stocks.begin() + count);

        return selection;
    }


    void Group::performBootstraping() {

        cout<<"Starting Bootstrapping"<<endl;

        vector<Matrix> AARs(BOOTSTRAP_ITERATIONS, Matrix(3));
        vector<Matrix> CAARs(BOOTSTRAP_ITERATIONS, Matrix(3));

        for (int iteration = 0; iteration < BOOTSTRAP_ITERATIONS; ++iteration) {
            cout<<"Iteration: "<<iteration<<endl;
            vector<Stock> missGroupSelection = selectRandomStocks(missGroup, BOOTSTRAP_SIZE);
            vector<Stock> meetGroupSelection = selectRandomStocks(meetGroup, BOOTSTRAP_SIZE);
            vector<Stock> beatGroupSelection = selectRandomStocks(beatGroup, BOOTSTRAP_SIZE);


            // Calculate metrics for each group
            pair<Vector, Vector> missMetrics = calculateMetrics(missGroupSelection);
            pair<Vector, Vector> meetMetrics = calculateMetrics(meetGroupSelection);
            pair<Vector, Vector> beatMetrics = calculateMetrics(beatGroupSelection);


            AARs[iteration][0] = missMetrics.first;
            AARs[iteration][1] = meetMetrics.first;
            AARs[iteration][2] = beatMetrics.first;

            CAARs[iteration][0] = missMetrics.second;
            CAARs[iteration][1] = meetMetrics.second;
            CAARs[iteration][2] = beatMetrics.second;
        }


        for (int i = 0; i < 3; i++) {
            Matrix result(4, Vector(2 * N, 0.0));
            result[0] = calculateAverageVector(AARs, i);
            result[1] = calculateStandardDeviation(AARs, i);
            result[2] = calculateAverageVector(CAARs, i);
            result[3] = calculateStandardDeviation(CAARs, i);
            metrics.push_back(result);
        }
    }


    pair<Vector, Vector> Group::calculateMetrics(vector<Stock> stocks) {
        Vector avgAAR(2 * N); //specify size so we can add later on 
        Vector avgCAAR(2 * N);

        for (Stock stock: stocks) {
            if (stock.getAbnormalReturns().empty()) {

                stock.calculateAbnormalReturns(benchmark);

            }
            avgAAR = stock.getAbnormalReturns() + avgAAR;

        }

        // Calculate AAR and CAAR using partial_sum
        avgAAR = avgAAR / stocks.size();
        std::partial_sum(avgAAR.begin(), avgAAR.end(), avgCAAR.begin());
        return make_pair(avgAAR, avgCAAR);
    }

    //Here index refers to the group we are dealing with
    Vector Group::calculateAverageVector(vector<Matrix>& matrices, int index) {
        size_t vec_length = matrices[0][0].size();
        Vector avgVector(vec_length);
        for (Matrix matrix: matrices) {
            avgVector = avgVector +  matrix[index];
        }
        return avgVector / BOOTSTRAP_ITERATIONS;
    }   

    Vector Group::calculateStandardDeviation(vector<Matrix>& matrices, int index){
        Vector meanVector = calculateAverageVector(matrices, index);
        size_t vectorSize = matrices[0][0].size();
        Vector variance(vectorSize);

        //Calculate the difference of every vector and the mean and square it 
        for (Matrix matrix: matrices) {
            Vector diff = matrix[index] - meanVector;
            variance = variance + diff * diff;
        }
        
        return pow((variance / BOOTSTRAP_ITERATIONS), 0.5);
    }

    vector<vector<Stock>> Group::divide_stocks_into_groups() {
        auto start = chrono::high_resolution_clock::now(); // Start timing
        vector<Stock> stocks;
        vector<vector<Stock>> groups;

        // CSV file path
        const string RusselEarningFile = "test.csv";
        ifstream file(RusselEarningFile);

        if (!file.is_open()) {
            throw std::runtime_error("Error opening file: " + RusselEarningFile);
        }

        // Skip header line
        string line;
        getline(file, line);

        // Read all stocks
        while (getline(file, line)) {
            stringstream ss(line);
            string symbol, announce_date, period_end;
            string estimate_str, reported_str, surprise_str, surprise_percent_str;

            // Parse CSV line
            getline(ss, symbol, ',');
            getline(ss, announce_date, ',');
            getline(ss, period_end, ',');
            getline(ss, estimate_str, ',');
            getline(ss, reported_str, ',');
            getline(ss, surprise_str, ',');
            getline(ss, surprise_percent_str, ',');

            // Convert strings to doubles
            double estimate = stod(estimate_str);
            double reported = stod(reported_str);
            double surprise = stod(surprise_str);
            double surprise_percent = stod(surprise_percent_str);

            // Create and populate stock object
            Stock stock;
            stock.setEarningsInfo(announce_date, period_end, estimate, reported, surprise, surprise_percent);
            stock.setSymbol(symbol);
            stocks.push_back(stock);
        }

        // Fetch data for each stock and filter based on available price data
        vector<Stock> validStocks;
        int totalStocks = stocks.size();
        int processedStocks = 0;

        cout << "Fetching price data for stocks..." << endl;

        DataFetcher dataFetcher;

        for (Stock& stock : stocks) {
            processedStocks++;
            // Display a loading bar
            cout << "\rProcessing: " << processedStocks << "/" << totalStocks << flush;

            // Fetch price data for the stock
            int fetchResult = dataFetcher.fetchStockData(stock, benchmark, N);

            // Check if the stock has at least 2N+1 price data points
            if (fetchResult == 0){
                validStocks.push_back(stock);
            }
        }

        cout << "\nValid stocks after filtering: " << validStocks.size() << endl;

        // Sort stocks based on surprise percentage
        sort(validStocks.begin(), validStocks.end(),
            [](const Stock& a, const Stock& b) {
                return a.getSurprisePercent() < b.getSurprisePercent();
            });

        // Split into three groups
        size_t groupSize = validStocks.size() / 3;
        size_t remainder = validStocks.size() % 3;

        vector<Stock> missGroup(validStocks.begin(), validStocks.begin() + groupSize);
        vector<Stock> meetGroup(validStocks.begin() + groupSize, validStocks.begin() + 2 * groupSize);
        vector<Stock> beatGroup(validStocks.begin() + 2 * groupSize, validStocks.end());

        // Adjust for remainder
        if (remainder > 0) {
            beatGroup.insert(beatGroup.end(), validStocks.end() - remainder, validStocks.end());
        }

        // Print summary
        cout << "Total stocks processed: " << validStocks.size() << endl;
        cout << "Stocks per group: " << groupSize << endl;

        cout << "\nGroup thresholds (Surprise %):" << endl;
        cout << "Miss Group (lowest): " << missGroup.front().getSurprisePercent()
            << "% to " << missGroup.back().getSurprisePercent() << "%" << endl;
        cout << "Meet Group (middle): " << meetGroup.front().getSurprisePercent()
            << "% to " << meetGroup.back().getSurprisePercent() << "%" << endl;
        cout << "Beat Group (highest): " << beatGroup.front().getSurprisePercent()
            << "% to " << beatGroup.back().getSurprisePercent() << "%" << endl;

        groups.push_back(missGroup);
        groups.push_back(meetGroup);
        groups.push_back(beatGroup);

        auto end = chrono::high_resolution_clock::now(); // End timing
        chrono::duration<double> elapsed = end - start;
        std::cout << "Elapsed time for getting data: " << elapsed.count() << " seconds." << std::endl;
        
        return groups;
    }

vector<vector<Stock>> Group::divide_stocks_into_groups_multi(size_t numThreads) {
    auto start = chrono::high_resolution_clock::now(); // Start timing
    vector<Stock> stocks;
    vector<vector<Stock>> groups;

    // CSV file path
    const string RusselEarningFile = "Russell3000EarningsAnnouncements.csv";
    ifstream file(RusselEarningFile);

    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + RusselEarningFile);
    }

    // Skip header line
    string line;
    getline(file, line);

    // Read all stocks
    while (getline(file, line)) {
        stringstream ss(line);
        string symbol, announce_date, period_end;
        string estimate_str, reported_str, surprise_str, surprise_percent_str;

        // Parse CSV line
        getline(ss, symbol, ',');
        getline(ss, announce_date, ',');
        getline(ss, period_end, ',');
        getline(ss, estimate_str, ',');
        getline(ss, reported_str, ',');
        getline(ss, surprise_str, ',');
        getline(ss, surprise_percent_str, ',');

        // Convert strings to doubles
        double estimate = stod(estimate_str);
        double reported = stod(reported_str);
        double surprise = stod(surprise_str);
        double surprise_percent = stod(surprise_percent_str);

        // Create and populate stock object
        Stock stock;
        stock.setEarningsInfo(announce_date, period_end, estimate, reported, surprise, surprise_percent);
        stock.setSymbol(symbol);
        stocks.push_back(stock);
    }

    // Multithreading setup
    vector<thread> threads;
    mutex validStocksMutex;
    vector<Stock> validStocks;

    // Divide stocks into chunks for each thread
    size_t totalStocks = stocks.size();
    size_t chunkSize = (totalStocks + numThreads - 1) / numThreads;

    auto worker = [&](size_t startIdx, size_t endIdx) {
        DataFetcher dataFetcher; // Each thread gets its own DataFetcher
        cout << "Thread processing stocks from " << startIdx << " to " << endIdx << endl;
        for (size_t i = startIdx; i < endIdx; ++i) {
            try {
                Stock& stock = stocks[i];
                // Fetch data with retries and timeouts
                int fetchResult = dataFetcher.fetchStockData(stock, benchmark, N);
                if (fetchResult == 0) {

                    std::lock_guard<std::mutex> lock(validStocksMutex);
                    validStocks.push_back(stock);
                }

            } 
            catch (const std::exception& e) {
                cerr << "Error processing stock: " << e.what() << endl;
            }
        }
    };

    // Launch threads
    for (size_t i = 0; i < numThreads; ++i) {
        size_t startIdx = i * chunkSize;
        size_t endIdx = std::min(startIdx + chunkSize, totalStocks);
        threads.emplace_back(worker, startIdx, endIdx);
    }

    // Wait for threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    // Check if validStocks is empty
    if (validStocks.empty()) {
        cerr << "No valid stocks found after fetching. Exiting." << endl;
        return groups;
    }

    cout << "\nValid stocks after filtering: " << validStocks.size() << endl;

    // Sort stocks based on surprise percentage
    sort(validStocks.begin(), validStocks.end(),
         [](const Stock& a, const Stock& b) {
             return a.getSurprisePercent() < b.getSurprisePercent();
         });

    size_t groupSize = validStocks.size() / 3;
    vector<Stock> missGroup(validStocks.begin(), validStocks.begin() + groupSize);
    vector<Stock> meetGroup(validStocks.begin() + groupSize, validStocks.begin() + 2 * groupSize);
    vector<Stock> beatGroup(validStocks.begin() + 2 * groupSize, validStocks.end());

    groups = {missGroup, meetGroup, beatGroup};

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Elapsed time for getting data: " << elapsed.count() << " seconds." << endl;

    return groups;
}






}
        