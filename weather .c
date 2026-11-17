/*
 * weather.c - Current weather for any city.
 * APIs: Open-Meteo Geocoding + Forecast (free, no API key). Parser: cJSON. HTTP: libcurl.
 *
 * Install (Debian/Ubuntu/WSL): sudo apt install build-essential libcurl4-openssl-dev libcjson-dev
 * Install (Windows MSYS2):     In the MSYS2 UCRT64 terminal, run:
 *                              pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-curl mingw-w64-ucrt-x86_64-cjson
 * Build: gcc -Wall -Wextra -O2 "weather .c" -o weather.exe -lcurl -lcjson
 * Run:   ./weather Bilaspur      (or run without arguments and type a city)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

#define MAX_RESPONSE (1024 * 1024)
#define MAX_CITY 100

typedef struct
{
    char *data;
    size_t size;
} Buffer;

static size_t write_cb(void *ptr, size_t sz, size_t nm, void *userdata)
{
    Buffer *b = userdata;
    size_t n = sz * nm;
    if (b->size + n > MAX_RESPONSE)
        return 0;
    char *p = realloc(b->data, b->size + n + 1);
    if (!p)
        return 0;
    b->data = p;
    memcpy(b->data + b->size, ptr, n);
    b->size += n;
    b->data[b->size] = '\0';
    return n;
}

static char *http_get(CURL *curl, const char *url)
{
    Buffer b = {NULL, 0};
    long status = 0;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &b);
    CURLcode rc = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    if (rc != CURLE_OK)
    {
        fprintf(stderr, "Network error: %s\n", curl_easy_strerror(rc));
        free(b.data);
        return NULL;
    }
    if (status != 200 || !b.data)
    {
        fprintf(stderr, "Server returned HTTP %ld\n", status);
        free(b.data);
        return NULL;
    }
    return b.data;
}

static const char *describe(int code)
{
    switch (code)
    {
    case 0:
        return "Clear sky";
    case 1:
        return "Mainly clear";
    case 2:
        return "Partly cloudy";
    case 3:
        return "Overcast";
    case 45:
    case 48:
        return "Fog";
    case 51:
    case 53:
    case 55:
        return "Drizzle";
    case 56:
    case 57:
        return "Freezing drizzle";
    case 61:
        return "Slight rain";
    case 63:
        return "Moderate rain";
    case 65:
        return "Heavy rain";
    case 66:
    case 67:
        return "Freezing rain";
    case 71:
    case 73:
    case 75:
        return "Snowfall";
    case 77:
        return "Snow grains";
    case 80:
    case 81:
    case 82:
        return "Rain showers";
    case 85:
    case 86:
        return "Snow showers";
    case 95:
        return "Thunderstorm";
    case 96:
    case 99:
        return "Thunderstorm with hail";
    default:
        return "Unknown";
    }
}

static const char *unit(const cJSON *units, const char *key)
{
    const cJSON *u = cJSON_GetObjectItemCaseSensitive(units, key);
    return cJSON_IsString(u) ? u->valuestring : "";
}

int main(int argc, char **argv)
{
    char city[MAX_CITY + 2] = {0};
    int exit_code = EXIT_FAILURE;
    CURL *curl = NULL;
    char *geo_body = NULL, *wx_body = NULL, *escaped = NULL;
    cJSON *geo = NULL, *wx = NULL;

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strlen(city) + strlen(argv[i]) + 2 > MAX_CITY)
            {
                fprintf(stderr, "City name too long (max %d characters).\n", MAX_CITY);
                return EXIT_FAILURE;
            }
            if (i > 1)
                strcat(city, " ");
            strcat(city, argv[i]);
        }
    }
    else
    {
        printf("Enter city name: ");
        if (!fgets(city, sizeof city, stdin))
        {
            fprintf(stderr, "No input.\n");
            return EXIT_FAILURE;
        }
        city[strcspn(city, "\r\n")] = '\0';
        if (strlen(city) > MAX_CITY)
        {
            fprintf(stderr, "City name too long.\n");
            return EXIT_FAILURE;
        }
    }
    if (city[0] == '\0')
    {
        fprintf(stderr, "City name cannot be empty.\n");
        return EXIT_FAILURE;
    }

    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
        return EXIT_FAILURE;
    curl = curl_easy_init();
    if (!curl)
    {
        fprintf(stderr, "Failed to initialise libcurl.\n");
        goto done;
    }

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "c-weather-cli/1.0");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
#if LIBCURL_VERSION_NUM >= 0x075500
    curl_easy_setopt(curl, CURLOPT_PROTOCOLS_STR, "https");
#else
    curl_easy_setopt(curl, CURLOPT_PROTOCOLS, (long)CURLPROTO_HTTPS);
#endif

    escaped = curl_easy_escape(curl, city, 0);
    if (!escaped)
    {
        fprintf(stderr, "Could not encode city name.\n");
        goto done;
    }

    char url[512];
    int n = snprintf(url, sizeof url,
                     "https://geocoding-api.open-meteo.com/v1/search?name=%s&count=1&language=en&format=json", escaped);
    if (n < 0 || (size_t)n >= sizeof url)
    {
        fprintf(stderr, "Request too long.\n");
        goto done;
    }

    geo_body = http_get(curl, url);
    if (!geo_body)
        goto done;
    geo = cJSON_Parse(geo_body);
    if (!geo)
    {
        fprintf(stderr, "Invalid response from geocoding service.\n");
        goto done;
    }

    const cJSON *results = cJSON_GetObjectItemCaseSensitive(geo, "results");
    const cJSON *place = cJSON_IsArray(results) ? cJSON_GetArrayItem(results, 0) : NULL;
    if (!place)
    {
        fprintf(stderr, "City \"%s\" not found.\n", city);
        goto done;
    }

    const cJSON *lat = cJSON_GetObjectItemCaseSensitive(place, "latitude");
    const cJSON *lon = cJSON_GetObjectItemCaseSensitive(place, "longitude");
    const cJSON *name = cJSON_GetObjectItemCaseSensitive(place, "name");
    const cJSON *admin = cJSON_GetObjectItemCaseSensitive(place, "admin1");
    const cJSON *country = cJSON_GetObjectItemCaseSensitive(place, "country");
    if (!cJSON_IsNumber(lat) || !cJSON_IsNumber(lon))
    {
        fprintf(stderr, "Geocoding response missing coordinates.\n");
        goto done;
    }

    n = snprintf(url, sizeof url,
                 "https://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f"
                 "&current=temperature_2m,apparent_temperature,relative_humidity_2m,wind_speed_10m,weather_code"
                 "&timezone=auto",
                 lat->valuedouble, lon->valuedouble);
    if (n < 0 || (size_t)n >= sizeof url)
    {
        fprintf(stderr, "Request too long.\n");
        goto done;
    }

    wx_body = http_get(curl, url);
    if (!wx_body)
        goto done;
    wx = cJSON_Parse(wx_body);
    if (!wx)
    {
        fprintf(stderr, "Invalid response from weather service.\n");
        goto done;
    }

    const cJSON *cur = cJSON_GetObjectItemCaseSensitive(wx, "current");
    const cJSON *units = cJSON_GetObjectItemCaseSensitive(wx, "current_units");
    const cJSON *temp = cJSON_GetObjectItemCaseSensitive(cur, "temperature_2m");
    const cJSON *feels = cJSON_GetObjectItemCaseSensitive(cur, "apparent_temperature");
    const cJSON *hum = cJSON_GetObjectItemCaseSensitive(cur, "relative_humidity_2m");
    const cJSON *wind = cJSON_GetObjectItemCaseSensitive(cur, "wind_speed_10m");
    const cJSON *wcode = cJSON_GetObjectItemCaseSensitive(cur, "weather_code");
    const cJSON *time_s = cJSON_GetObjectItemCaseSensitive(cur, "time");
    if (!cJSON_IsNumber(temp) || !cJSON_IsNumber(feels) || !cJSON_IsNumber(hum) ||
        !cJSON_IsNumber(wind) || !cJSON_IsNumber(wcode))
    {
        fprintf(stderr, "Weather response missing expected fields.\n");
        goto done;
    }

    printf("\nWeather for %s%s%s%s%s\n",
           cJSON_IsString(name) ? name->valuestring : city,
           cJSON_IsString(admin) ? ", " : "", cJSON_IsString(admin) ? admin->valuestring : "",
           cJSON_IsString(country) ? ", " : "", cJSON_IsString(country) ? country->valuestring : "");
    if (cJSON_IsString(time_s))
        printf("Local time : %s\n", time_s->valuestring);
    printf("Condition  : %s\n", describe(wcode->valueint));
    printf("Temperature: %.1f %s (feels like %.1f %s)\n",
           temp->valuedouble, unit(units, "temperature_2m"),
           feels->valuedouble, unit(units, "apparent_temperature"));
    printf("Humidity   : %.0f %s\n", hum->valuedouble, unit(units, "relative_humidity_2m"));
    printf("Wind speed : %.1f %s\n", wind->valuedouble, unit(units, "wind_speed_10m"));
    exit_code = EXIT_SUCCESS;

done:
    cJSON_Delete(wx);
    cJSON_Delete(geo);
    free(wx_body);
    free(geo_body);
    curl_free(escaped);
    if (curl)
        curl_easy_cleanup(curl);
    curl_global_cleanup();
    return exit_code;
}