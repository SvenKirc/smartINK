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

int get_weather(){
    curl_global_init(CURL_GLOBAL_DEFAULT);
    Curler curler;
    json weather_information;
    curler.url = "https://app-prod-ws.warnwetter.de/v30/stationOverviewExtended?stationIds=1977,G005";
    std::string weather_info = curler.to_curl();
    weather_information = json::parse(weather_info);
    std::array<std::array<float,2>,4> temperature_forecast = get_temperature(weather_information);
    std::array<int, 4> icons = get_icons(weather_information);
    std::cout << std::to_string(temperature_forecast[0][1]) << std::to_string(icons[0]);
    write_to_file(weather_info);
    return 0;
}

int main(){
    int weather_status = get_weather();
    return 0;
}