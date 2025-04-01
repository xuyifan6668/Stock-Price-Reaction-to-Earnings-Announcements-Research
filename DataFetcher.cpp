#include "DataFetcher.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>  // for std::find
#include <utility>    // for std::pair

using namespace std;

namespace fre {
    DataFetcher::DataFetcher()  {
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        if(!curl) {
            cerr << "Failed to initialize CURL" << endl;
        }

        string url_common = "https://eodhistoricaldata.com/api/eod/";
        string api_token = "674cf641987039.16762367";
        
    }


    DataFetcher::~DataFetcher() {
        if(curl) {
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
    }

    size_t DataFetcher::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }


    // //looks like this works, still need to check and do some boundary checks before passing data to the url string
    // pair<string, string> DataFetcher::calculateDateRange(const string& announcement_date, int N) {
    //     struct tm tm;
    //     strptime(announcement_date.c_str(), "%Y-%m-%d", &tm);

    //     // Calculate the start date (N days before the announcement date)
    //     time_t announcement_time = mktime(&tm);
    //     time_t start_time = announcement_time - (N * 24 * 60 * 60);  // N days before

    //     // Calculate the end date (N days after the announcement date)
    //     time_t end_time = announcement_time + (N * 24 * 60 * 60);  // N days after

    //     // Convert start_time and end_time to string in "YYYY-MM-DD" format
    //     char start_date[11], end_date[11];
    //     strftime(start_date, sizeof(start_date), "%Y-%m-%d", localtime(&start_time));
    //     strftime(end_date, sizeof(end_date), "%Y-%m-%d", localtime(&end_time));

    //     return make_pair(string(start_date), string(end_date));
    // }

    int DataFetcher::fetchStocks(vector<Stock>& stocks, Stock& benchmark, int N) {
        if(!curl) {
            cerr << "Curl not initialized!" << endl;
            return -1;
        }

        for (Stock& stock : stocks) {
            DataFetcher::fetchStockData(stock, benchmark, N);
            // cout<<"Fetch "<<stock.getSymbol()<<endl;
        }
        return 0;
    }

    // Need to add the start and end date since we want to collect data for all possible trading days within N-window
    // for all stocks for our benchmark, but for each stock we only need the (2N+1) prices around the earnings 
    // annoucement day
    int DataFetcher::fetchBenchmarkData(Stock& benchmark, string start_date, string end_date)
    {
        if(!curl) {
            cerr << "Curl not initialized!" << endl;
            return -1;
        }
        size_t count = 0;


        string url_request = url_common + benchmark.getSymbol() + 
                            ".US?from=" + start_date + 
                            "&to=" + end_date + 
                            "&api_token=" + api_token + 
                            "&period=d";


        // cout << "URL: " << url_request << endl;

        // Set up CURL request
        string response_string;
        curl_easy_setopt(curl, CURLOPT_URL, url_request.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

        // Perform the request
        CURLcode result = curl_easy_perform(curl);
        if(result != CURLE_OK) {
            cerr << "Curl failed: " << curl_easy_strerror(result) << endl;
            return -1;
        }

        stringstream ss(response_string);
        string line;
        vector<string> fields;

        // Skip header
        getline(ss, line);
        // cout << "Header: " << line << endl;

        // Process each line
        while(getline(ss, line)) {
            if(line.empty()) continue;

            fields.clear();
            stringstream lineStream(line);
            string field;

            while(getline(lineStream, field, ',')) {
                fields.push_back(field);
            }

            if(fields.size() >= 6) {
                try {
                    benchmark.setPrice(fields[0], stod(fields[5]));
                    // cout << fields[0] << ": " << stod(fields[5]) << endl;                    
                    count++;
                } 
                catch(const exception& e) {
                    continue;
                }
            }
        }
        // cout << "Total prices stored: " << count << endl;
        return 0;
    }

     int DataFetcher::fetchStockData(Stock& stock, Stock& benchmark, int N)
    {
        if(!curl) {
            cerr << "Curl not initialized!" << endl;
            return -1;
        }

        if (!stock.getPrices().empty()) {
            cout << "Data for this stock already fetched!" << endl;
            return -1;
        }
            
        // We calculate the start and end dates 
        auto start_end_dates = start_end_window(benchmark.getTradingDays(), stock.getAnnouncementDate(), N);

        size_t count = 0;


        string url_request = url_common + stock.getSymbol() + 
                            ".US?from=" + start_end_dates.first + 
                            "&to=" + start_end_dates.second + 
                            "&api_token=" + api_token + 
                            "&period=d";


        // cout << "URL: " << url_request << endl;

        // Set up CURL request
        string response_string;
        curl_easy_setopt(curl, CURLOPT_URL, url_request.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

        // Perform the request
        CURLcode result = curl_easy_perform(curl);
        if(result != CURLE_OK) {
            cerr << "Curl failed: " << curl_easy_strerror(result) << endl;
            return -1;
        }

        cout<<response_string<<endl;
        if (response_string.empty()) {
            std::cerr << "Empty response string!" << std::endl;
        }
        stringstream ss(response_string);
        string line;
        vector<string> fields;

        // Skip header
        getline(ss, line);
        // cout << "Header: " << line << endl;

        // Process each line
        while(getline(ss, line)) {
            if(line.empty()) continue;

            fields.clear();
            stringstream lineStream(line);
            string field;

            while(getline(lineStream, field, ',')) {
                fields.push_back(field);
            }

            if(fields.size() >= 6) {
                try {
                    stock.setPrice(fields[0], stod(fields[5]));
                    // cout << fields[0] << ": " << stod(fields[5]) << endl;                    
                    count++;
                } 
                catch(const exception& e) {
                    continue;
                }
            }
        }
        // cout<<stock.getPrices().size()<<endl;
        if(stock.getPrices().size()!= static_cast<size_t> (2*N+1)){
            cout<<stock.getSymbol()<<" was filtered"<<endl;
            return -1;
        }
        return 0;
    }

    pair<string, string> start_end_window(const vector<string>& trading_days, string earnings_day, int N) {
    // Use lower_bound to find the first trading day >= earnings_day
    auto itr = lower_bound(trading_days.begin(), trading_days.end(), earnings_day);

    // Check if there is no valid trading day after earnings_day
    if (itr == trading_days.end()) {
        cerr << "Earnings day not found and no subsequent trading day available: " << earnings_day << endl;
        return {trading_days[-1], trading_days[-1]};
    }

    // If the exact earnings_day is not found, log the adjustment
    if (*itr != earnings_day) {
        cout << "Earnings day adjusted to the next trading day: " << *itr << " (was: " << earnings_day << ")" << endl;
    }

    // Determine the full 2N-day window around the earnings_day
    auto earnings_idx = itr - trading_days.begin();
    int total_days = trading_days.size();

    // Calculate the indices for the 2N-day window
    int start_idx = max(0, static_cast<int>(earnings_idx - N)); // Start index within bounds
    int end_idx = min(total_days - 1, static_cast<int>(earnings_idx + N)); // End index within bounds


    // Get the start and end trading days for the overlapping range
    string startDate = trading_days[start_idx];
    string endDate = trading_days[end_idx];

    return {startDate, endDate};
}


}