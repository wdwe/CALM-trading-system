#include <iostream>
#include <ctime>
#include <chrono>
#include "date/tz.h"
#include "date/date.h"
#include <sstream>

using namespace std::chrono_literals;

void check_ctime() {
    char start_time[] = "20240118 06:50:00";
    std::tm tm{};
    strptime(start_time, "%Y%m%d %H:%M:%S", &tm);
    // mktime convert to LOCAL time 20240118 06:50:00 to start (epoch time)
    auto start = mktime(&tm);
    std::cout << start << std::endl;
    time_t time = start;
//    time_t time = std::time(nullptr);
    char time_str[std::size("yyyymmdd-hh:mm:ss")];
    std::cout << time << std::endl;
    // std::localtime will convert time back to local time 20240118 06:50:00
    // std::gmtime will convert time to UTC+0 time
    std::tm *ptm = std::gmtime(&time);
    std::strftime(time_str, std::size(time_str), "%Y%m%d-%H:%M:%S", ptm);
    std::cout << time_str << std::endl;
}

void check_time_zone() {
    auto utc_time = std::chrono::system_clock::now();
    auto berlin = date::make_zoned("Europe/Berlin", utc_time);
    auto eastern = date::make_zoned("America/New_York", utc_time);
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp;
    std::istringstream in{"Thu, 9 Jan 2014 12:35:34.032 +0000"};
    in >> date::parse("%a, %d %b %Y %T %z", tp);
    std::cout << date::format("%F %T %Z", utc_time) << std::endl;
    std::cout << date::format("%F %T %Z", berlin) << std::endl;
    std::cout << date::format("%F %T %Z", eastern) << std::endl;
    std::cout << date::format("%F %T %Z", tp) << std::endl;
    auto t = std::chrono::system_clock::from_time_t(1705992493);
    std::cout << date::format("%F %T %Z", t) << std::endl;
}


int main() {
/*
 * parse timestamp with timezone
 */
    // note the date::local_t
    std::chrono::time_point<date::local_t, std::chrono::microseconds> tp;
    std::istringstream ss{"2025-03-22 04:35:34.032123"}; //  America/New_York

    ss >> date::parse("%Y-%m-%d %T", tp);
    auto zt = date::make_zoned("America/New_York", tp);

    std::cout << zt << std::endl;
    // convert to sys time
    std::cout << zt.get_sys_time() << std::endl;
    auto zt2 = date::make_zoned("America/New_York", date::local_days{date::year(2025)/3/8} + 4h + 35min + 34s + 32123us);
    std::cout << zt2 << std::endl;
    std::cout << zt2.get_sys_time() << std::endl;

    return 0;
}