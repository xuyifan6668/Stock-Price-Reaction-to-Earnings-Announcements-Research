#pragma once

#include <string>
#include <map>
#include <vector>
#include "Matrix.h"

using namespace std;

namespace fre {
    class Stock {
    private:
        string symbol;
        map<string, double> prices;     // date -> adjusted closing price; key -> value
        string announcement_date;
        string period_ending;
        double eps_estimate;
        double eps_reported;
        double surprise;
        double surprise_percent;
        int group;                      // 1: Beat, 2: Meet, 3: Miss

        
        Vector abnormal_returns;
        Vector cumulative_returns;

    public:
        // Constructors
        Stock() : eps_estimate(0), eps_reported(0), surprise(0), 
                surprise_percent(0), group(0) {}
                
        Stock(const string& sym) : Stock() { symbol = sym; }
        
        // Core functionality
        void setSymbol(const string& sym) { symbol = sym; }
        void setPrice(const string& date, double price) { prices[date] = price; }
        
        void setEarningsInfo(const string& announce_date_, const string& period_end_,
                            double estimate_, double reported_, double surprise_, double surprise_percent_) {
            announcement_date = announce_date_;
            period_ending = period_end_;
            eps_estimate = estimate_;
            eps_reported = reported_;
            surprise = surprise_;
            surprise_percent = surprise_percent_;
            // surprise = reported - estimate;
            // surprise_percent = (surprise / abs(estimate)) * 100;
        }
        
        void setGroup(int g) { group = g; }

        // Getters
        string getSymbol() const { return symbol; }
        string getAnnouncementDate() const { return announcement_date; }
        string getPeriodEnding() const { return period_ending; }
        double getEPSEstimate() const { return eps_estimate; }
        double getEPSReported() const { return eps_reported; }
        double getSurprise() const { return surprise; }
        double getSurprisePercent() const { return surprise_percent; }
        int getGroup() const { return group; }
        map<string, double> getPrices() const { return prices; }
        vector<string> getTradingDays() const;
        Vector getAbnormalReturns() const { return abnormal_returns; }
        Vector getCumulativeReturns() const { return cumulative_returns; }
        
        // Price and return calculations
        double getPrice(const string& date) const;
        double calculateDailyReturn(const string& date) const;
        map<string, double> calculateLogReturns() const;
        Vector calculateReturnsForRange(const string& start_date, 
                                            const string& end_date) const;
        void calculateAbnormalReturns(const Stock& benchmark);
        void calculateCumulativeReturns();

    };
}