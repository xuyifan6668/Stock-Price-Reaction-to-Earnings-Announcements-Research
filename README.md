# Financial Computing Final Project - Fall 2024
## Impact of Quarterly Earnings Report on Stock Price Movement

### Project Overview
This project analyzes the impact of quarterly earnings reports on stock price movements for Russell 3000 stocks. The analysis focuses on the second quarter of 2024 earnings announcements and investigates how different levels of earnings surprises affect stock performance.

### Features
- Historical price data retrieval from eodhistoricaldata.com
- Analysis of Russell 3000 stocks and IWV (Russell 3000 ETF benchmark)
- Bootstrapping analysis with 40 iterations
- Calculation of Average Abnormal Returns (AAR) and Cumulative Average Abnormal Returns (CAAR)
- Visualization using gnuplot
- Interactive menu-driven interface

### Technical Requirements
- C++ programming language
- libcurl for data retrieval
- STL containers (map, vector)
- gnuplot for visualization
- Object-oriented design with operator overloading

### Project Structure
The project is organized into the following components:
1. Data Retrieval Module
   - Historical price data fetching
   - Earnings announcement data processing

2. Analysis Module
   - Stock grouping based on earnings surprises
   - Daily returns calculation
   - AAR and CAAR computation
   - Bootstrapping implementation

3. Visualization Module
   - gnuplot integration
   - CAAR plotting for all three groups

### Menu Options
1. Retrieve historical price data (2N+1 days)
   - N must be between 40 and 80
2. Display individual stock information
   - Daily prices
   - Cumulative daily returns
   - Group classification
   - Earnings details
3. Show AAR statistics for a specific group
4. Display CAAR visualization
5. Exit program

### Stock Groups
Stocks are categorized into three groups based on earnings surprise percentage:
1. Beat Estimate Group (Highest surprise)
2. Meet Estimate Group (Middle range)
3. Miss Estimate Group (Lowest surprise)

### Calculation Methodology
1. Daily Returns: Rit = log(Pricet/ Pricet-1)
2. Abnormal Returns: ARit = Rit – Rmt
3. Average Abnormal Returns: AARt = (1/M) ∑ ARit
4. Cumulative Average Abnormal Returns: CAAR = ∑ AARt

### Dependencies
- libcurl
- gnuplot
- C++ Standard Library

### Building and Running

#### Prerequisites
1. Install required dependencies:
   ```bash
   # For macOS (using Homebrew)
   brew install libcurl
   brew install gnuplot
   
   # For Ubuntu/Debian
   sudo apt-get install libcurl4-openssl-dev
   sudo apt-get install gnuplot
   ```

2. Ensure you have a C++ compiler (g++) installed:
   ```bash
   # For macOS
   brew install gcc
   
   # For Ubuntu/Debian
   sudo apt-get install build-essential
   ```

#### Building the Project
1. Clone the repository:
   ```bash
   git clone [repository-url]
   cd [project-directory]
   ```

2. Compile the project:
   ```bash
   make
   ```
   This will create an executable named `main` in the project directory.

#### Running the Program
1. Execute the program:
   ```bash
   ./main
   ```

2. The program will present a menu with the following options:
   - Enter N to retrieve historical price data (N must be between 40 and 80)
   - Pull information for a specific stock
   - Show AAR statistics for a group
   - Display CAAR visualization
   - Exit

3. Follow the on-screen prompts to interact with the program.

#### Cleaning the Build
To clean the build files and start fresh:
```bash
make clean
```

#### Troubleshooting
- If you encounter libcurl errors, ensure your API key is properly configured
- For gnuplot visualization issues, verify that gnuplot is installed and accessible from the command line
- Make sure all required data files (Russell3000EarningsAnnouncements.csv) are present in the project directory

### Submission Deadline
- Saturday Session: December 11, 2024, 11:55 PM
- Tuesday Session: December 14, 2024, 11:55 PM

### Project Deliverables
1. Source code
2. Executable files
3. PowerPoint presentation
4. Project documentation
5. Research findings on Russell 3000 stocks earnings impact