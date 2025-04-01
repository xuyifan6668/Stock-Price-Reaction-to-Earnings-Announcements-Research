// DataFetcher.h
#pragma once
#include <string>
#include <curl/curl.h>
#include "Stock.h"



namespace fre {
    class DataFetcher {
    private:
        CURL* curl;
        string url_common = "https://eodhistoricaldata.com/api/eod/";
        string api_token = "674cf641987039.16762367";
        
        // Helper function for curl write callback
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
        
        // Calculate date range based on announcement date and N
        // std::pair<std::string, std::string> calculateDateRange(const std::string& announcement_date, int N);

    public:
        DataFetcher();
        ~DataFetcher();
        
        //Function to fetch data for benchmark
        int fetchBenchmarkData(Stock& benchmark, string start_date, string end_date);
        // function to fetch data for a vector of stocks
        int fetchStocks(vector<Stock>& stocks, Stock& benchmark, int N);
        // function to fetch data for single stock
        int fetchStockData(Stock& stock, Stock& benchmark, int N);
    };

    pair<string, string> start_end_window(const vector<string>& trading_days, string earnings_day, int N);
}