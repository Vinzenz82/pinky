#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#include <lgpio.h> // lg

// openweather
#include <curl.h>
#include <json.hpp>  

#include <epaper_ifc.h>

#define PIN_LED_RED 21
#define PIN_LED_YELLOW 20
#define PIN_LED_GREEN 16
#define LFLAGS 0
#define LED_ON 1
#define LED_OFF 0


std::mutex g_pages_mutex;

int handle_lg;

void display(std::string msg)
{
    if(DEV_Module_Init()!=0){
        std::cerr << "DEV_Module_Init fails..." << std::endl;
        return;
    }

    std::cout << "e-Paper Init and Clear... " << std::endl;
    EPD_4IN2_V2_Init();
    EPD_4IN2_V2_Clear();
    DEV_Delay_ms(500);

    //Create a new image cache
    UBYTE *BlackImage;
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UWORD Imagesize = ((EPD_4IN2_V2_WIDTH % 8 == 0)? (EPD_4IN2_V2_WIDTH / 8 ): (EPD_4IN2_V2_WIDTH / 8 + 1)) * EPD_4IN2_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        std::cerr << "Failed to apply for black memory..." << std::endl;
        return ;
    }

    std::cout << "Paint_NewImage..." << std::endl;
    Paint_NewImage(BlackImage, EPD_4IN2_V2_WIDTH, EPD_4IN2_V2_HEIGHT, 0, WHITE);

    std::cout << "show window BMP-----------------" << std::endl;
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    GUI_ReadBmp("./pic/cat_mouse.bmp", 10, 10);
    EPD_4IN2_V2_Display(BlackImage);
    DEV_Delay_ms(2000);

    while(true) {
        {
            std::lock_guard<std::mutex> guard(g_pages_mutex);
            lgGpioWrite(handle_lg, PIN_LED_RED, LED_ON);
            std::cout << "Task1 says: " << std::endl;
            for (std::size_t x = 0, length = msg.length(); x != length; ++x) {
                std::cout << msg[x] << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(288));
            }
            lgGpioWrite(handle_lg, PIN_LED_RED, LED_OFF);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
}

size_t writeCallback(char* ptr, int size, int nmemb, void* userdata) {
    std::string* stream = (std::string*)userdata;
    int realsize = size * nmemb;
    stream->append(ptr, realsize);

    std::cout << "writeCallback: " << stream->c_str() << std::endl; 

    return realsize;
}

void openweather(std::string msg)
{
    CURL* curl;
    CURLcode res;
    curl = curl_easy_init();
    std::string stream;

    time_t sttime;
    char ctime[80];

    if(curl == NULL){
        std::cerr << "curl_easy_init fails..." << std::endl;
        return;
    }

    while(true) {
        {
            std::lock_guard<std::mutex> guard(g_pages_mutex);

            curl_easy_setopt(curl, CURLOPT_URL, "http://api.openweathermap.org/data/2.5/weather?q=Wilsdruff,de&appid=01bfc1473b89420ac08c560a25c1b535");
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            res = curl_easy_perform(curl);

            if (res != CURLE_OK)
            {
                std::cout << "CURL error: " << curl_easy_strerror(res) << std::endl;
            }
            else {
                nlohmann::json weatherData;
                try
                {
                    weatherData = nlohmann::json::parse(stream);
                }
                catch (nlohmann::json::parse_error& e)
                {
                    std::cout << "Error parsing JSON: " << e.what() << std::endl;
                    return;
                }
    
                try
                {
                    time(&sttime);
                    struct tm* data = localtime(&sttime);

                    double temperature = weatherData["main"]["temp"].get<double>() - 273.15;
                    double humidity = weatherData["main"]["humidity"];
                    double pressure = weatherData["main"]["pressure"];
                    double windSpeed = weatherData["wind"]["speed"];
                    std::string weatherDescription = weatherData["weather"][0]["description"];
                    std::string stationName = weatherData["name"];
    
                    std::cout << "Letzte Aktualisiserung: " << std::endl;
                    strftime(ctime, 80, "Uhrzeit: %H:%M", data);
                    std::cout << ctime;
    
                    std::cout << "\nTemperature: " << temperature << " °C" << std::endl;
                    std::cout << "Humidity: " << humidity << "%" << std::endl;
                    std::cout << "Pressure: " << pressure << "hPa" << std::endl;
                    std::cout << "Descrription: " << weatherDescription << std::endl;
                    std::cout << "Wind Speed: " << windSpeed << " m/s" << std::endl;
                    std::cout << "Station Name: " << stationName << std::endl;
                }
                catch (nlohmann::json::exception& e)
                {
                    std::cout << "Error: " << e.what() << std::endl;
                }                
            }
            

            lgGpioWrite(handle_lg, PIN_LED_GREEN, LED_ON);
            std::cout << "openweather says: " << std::endl;
            for (std::size_t x = 0, length = msg.length(); x != length; ++x) {
                std::cout << msg[x] << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(344));
            }
            lgGpioWrite(handle_lg, PIN_LED_GREEN, LED_OFF);
        }
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
    curl_easy_cleanup(curl);
}

void task3(std::string msg)
{
    while(true) 
    {
        {
            std::lock_guard<std::mutex> guard(g_pages_mutex);
            lgGpioWrite(handle_lg, PIN_LED_YELLOW, LED_ON);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            lgGpioWrite(handle_lg, PIN_LED_YELLOW, LED_OFF);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


int main() {
    std::cout << "Hello Poky World!" << std::endl;

    handle_lg = lgGpiochipOpen(0);
    if(handle_lg < 0) {
        throw std::runtime_error("lg error");
    }

    if(     ( lgGpioClaimOutput(handle_lg, LFLAGS, PIN_LED_RED, 0)      != LG_OKAY )
        ||  ( lgGpioClaimOutput(handle_lg, LFLAGS, PIN_LED_YELLOW, 0)   != LG_OKAY )
        ||  ( lgGpioClaimOutput(handle_lg, LFLAGS, PIN_LED_GREEN, 0)    != LG_OKAY ) ) 
    {
        throw std::runtime_error("lg claim error");
    }

    std::thread t1(display, "PING PINg PIng Ping ping");
    std::thread t2(openweather, "Pong");
    std::thread t3(task3, "");
    t1.join();
    t2.join();
    t3.join();

    return 0;
}