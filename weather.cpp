#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <array>

using json = nlohmann::json;

size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

class Curler{
    public:
        std::string url = "https://app-prod-ws.warnwetter.de/v30/stationOverviewExtended?stationIds=10865,G005";
        std::string to_curl(){
            url = this->url;
            auto curl = curl_easy_init();
            if(!curl) {
                printf("Initialisierung von curl fehlgeschlagen!");
                return "";
            };
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
        
            std::string response_string;
            std::string header_string;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
        
            curl_easy_perform(curl);
            std::cout << "Received weather information\n";
        
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            curl = NULL;  
            return response_string;
        }
};

int write_to_file(std::string log_data){
    std::ofstream myfile;
    myfile.open ("log.json");
    myfile << log_data;
    myfile.close();
    return 0;
}

std::array<std::array<float,2>,4> get_temperature(json weather_information){
    std::array<float,2> temp_today  = {float(weather_information["G005"]["days"][0]["temperatureMin"]) / 10, float(weather_information["G005"]["days"][0]["temperatureMax"]) / 10};
    std::array<float,2> temp_tomorrow  = {float(weather_information["G005"]["days"][1]["temperatureMin"]) / 10, float(weather_information["G005"]["days"][1]["temperatureMax"]) / 10};
    std::array<float,2> temp_twodays  = {float(weather_information["G005"]["days"][2]["temperatureMin"]) / 10, float(weather_information["G005"]["days"][2]["temperatureMax"]) / 10};
    std::array<float,2> temp_threedays  = {float(weather_information["G005"]["days"][3]["temperatureMin"]) / 10, float(weather_information["G005"]["days"][3]["temperatureMax"]) / 10};
    std::array<std::array<float,2>,4> weather_forecast = {temp_today, temp_tomorrow, temp_twodays, temp_threedays};
    return weather_forecast;
}

std::array<int, 4> get_icons(json weather_information){
    int icon_today = weather_information["G005"]["days"][0]["icon"];
    int icon_tomorrow = weather_information["G005"]["days"][1]["icon"];
    int icon_twodays = weather_information["G005"]["days"][2]["icon"];
    int icon_threedays = weather_information["G005"]["days"][3]["icon"];
    std::array<int, 4> icons = {icon_today, icon_tomorrow, icon_twodays, icon_threedays};
    return icons;
}

std::array<float, 24> get_hourly_temperature_for_day(json weather_information, int day_offset) {
    std::array<float, 24> hourly{};

    auto temps = weather_information["G005"]["forecast1"]["temperature"];

    int start_index = day_offset * 24;

    for (int hour = 0; hour < 24; hour++) {
        hourly[hour] = float(temps[start_index + hour]) / 10.0f;
    }

    return hourly;
}

void print_days(json weather_information) {
    auto days = weather_information["G005"]["days"];

    for (int i = 2; i < 5; i++) {
        std::string date = days[i]["dayDate"];
        float minTemp = float(days[i]["temperatureMin"]) / 10.0f;
        float maxTemp = float(days[i]["temperatureMax"]) / 10.0f;
        int icon = days[i]["icon"];

        std::cout << date
                  << ": min " << std::fixed << std::setprecision(1) << minTemp
                  << " °C, max " << maxTemp
                  << " °C, icon " << icon
                  << "\n";
    }
}

std::array<int, 4> get_day_icons_8_12_16_20(json weather_information, int day_offset) {
    std::array<int, 4> icons{};

    auto icon_array = weather_information["G005"]["forecast1"]["icon1h"];

    std::array<int, 4> hours = {8, 12, 16, 20};

    for (int i = 0; i < 4; i++) {
        int index = day_offset * 24 + hours[i];
        icons[i] = icon_array[index];
    }

    return icons;
}

int get_weather(){
    curl_global_init(CURL_GLOBAL_DEFAULT);
    Curler curler;
    json weather_information;
    std::array<int, 4> hours = {8, 12, 16, 20};
    curler.url = "https://app-prod-ws.warnwetter.de/v30/stationOverviewExtended?stationIds=01975,G005";
    std::string weather_info = curler.to_curl();
    weather_information = json::parse(weather_info);
    std::array<std::array<float,2>,4> temperature_forecast = get_temperature(weather_information);
    std::array<int, 4> icons = get_icons(weather_information);

    auto hourly_today = get_hourly_temperature_for_day(weather_information, 0);
    auto hourly_tomorrow = get_hourly_temperature_for_day(weather_information, 1);

    auto today_icons = get_day_icons_8_12_16_20(weather_information, 0);
    auto tomorrow_icons = get_day_icons_8_12_16_20(weather_information, 1); 

    std::cout<<"Hourly forecast for today:\n";
    for (int hour = 0; hour < 24; hour++) {
        std::cout << std::setw(2) << std::setfill('0') << hour
          << ":00 -> " << std::fixed << std::setprecision(1)
          << hourly_today[hour] << " °C\n";
    }

    std::cout<< "Icons for today:\n";
    for (int i = 0; i < 4; i++) {
        std::cout << hours[i] << ":00 -> icon " << today_icons[i] << "\n";
    }

    std::cout << "Hourly forecast for tomorrow:\n";
    for (int hour = 0; hour < 24; hour++) {
        std::cout << std::setw(2) << std::setfill('0') << hour
                << ":00 -> "
                << std::fixed << std::setprecision(1)
                << hourly_tomorrow[hour]
                << " °C\n";
    }

    std::cout<< "Icons for tomorrow:\n";
    for (int i = 0; i < 4; i++) {
        std::cout << hours[i] << ":00 -> icon " << tomorrow_icons[i] << "\n";
    }

    print_days(weather_information);

    write_to_file(weather_info);
    return 0;
}

int main(){
    int weather_status = get_weather();
    return 0;
}