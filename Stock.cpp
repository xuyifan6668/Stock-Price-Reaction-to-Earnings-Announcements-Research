#include "Stock.h"
#include <cmath>

using namespace std;

namespace fre {
    double Stock::getPrice(const string& date) const {
        auto it = prices.find(date);
        if (it != prices.end()) {
            return it->second;
        } else {
            throw logic_error("Date not found in prices");
        }
    }

    vector<string> Stock::getTradingDays() const {
        vector<string> trading_days;
        for (const auto& pair : prices) {
            trading_days.push_back(pair.first);
        }
        return trading_days;
    }

    map<string, double> Stock::calculateLogReturns() const {
        map<string, double> log_returns;

        //Set prev iterator
        auto prev = prices.begin();
        for (auto it = next(prev); it != prices.end(); ++it) {
            double value = log(it -> second / prev -> second);  // Dereference current and next iterators
            log_returns[it->first] = value;
            prev = it;
        }
        return log_returns;
    }

    double Stock::calculateDailyReturn(const string& date) const {
        auto it = prices.find(date);
        return (it != prices.begin() && it != prices.end()) ? 
            log(it->second / prev(it)->second) : 0;
    }

    Vector Stock::calculateReturnsForRange(const string& start_date, 
                                                const string& end_date) const {
        vector<double> returns;
        auto start = prices.lower_bound(start_date);
        auto end = prices.upper_bound(end_date);
        
        for (auto it = start; it != end && next(it) != end; ++it) {
            returns.push_back(log(next(it)->second / it->second));
        }
        return returns;
    }
    
    void Stock::calculateAbnormalReturns(const Stock& benchmark) {

    // Get log_returns for the stock and benchmark
    const map<string, double>& stock_log_returns = calculateLogReturns();
    const map<string, double>& benchmark_log_returns = benchmark.calculateLogReturns();

    // Iterate through stock_log_returns and calculate abnormal returns
    for (const auto& [date, stock_return] : stock_log_returns) {
        auto benchmark_it = benchmark_log_returns.find(date);
        // Check if benchmark data exists for the date
        if (benchmark_it != benchmark_log_returns.end()) {
            abnormal_returns.push_back(stock_return - benchmark_it->second);
        } else {
            // If no benchmark data exists, skip the date
            cerr << "Warning: No benchmark data for date " << date << endl;
            continue;
        }
    }
}


    void Stock::calculateCumulativeReturns() {
        cumulative_returns.clear();
        double cum_return = 0;
        for (double ar : abnormal_returns) {
            cum_return += ar;
            cumulative_returns.push_back(cum_return);
        }
    }
}