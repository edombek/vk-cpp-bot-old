#ifdef __linux__
#include <curl/curl.h>
#elif _WIN32
#pragma comment(lib, "libcurl.lib")
#include "win32deps/include/curl/curl.h"
#endif
#include "common.h"
#include <fcntl.h>
#include <iostream>
#include <mutex>
#include <stdio.h>
#include <sys/stat.h>

#define net_agent "EVGESHAd vk-cpp-bot"
//#define printOut

mutex infoLock;

long long unsigned int send_dl(0);
long long unsigned int send_ul(0);
long long unsigned int upload_dl(0);
long long unsigned int upload_ul(0);
long long unsigned int download_dl(0);
long long unsigned int download_ul(0);

/* Init network settings */
void net::init()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

string net::urlEncode(string str)
{
    char* esc_text = curl_easy_escape(NULL, str.c_str(), str.length());
    string result = esc_text;

    curl_free(esc_text);
    return result;
}

string net::send(string url, table param, bool post)
{
    string paramline = "";
    table* params = &param;
    for (auto iter = params->begin(); iter != params->end(); iter++) {
        paramline += iter->first + "=" + urlEncode(iter->second) + "&";
    }
    if (post)
        return net::send(url, paramline);
    return net::send(url + "?" + paramline);
}

size_t writer(char* data, size_t size, size_t nmemb, string* buffer)
{
    int result = 0;
    if (buffer != NULL) {
        buffer->append(data, size * nmemb);
        result = size * nmemb;
    }
    return result;
}

string net::send(string url, string params)
{
    string buffer = "";
    CURL* curl;
    CURLcode result;
    char errorBuffer[CURL_ERROR_SIZE];
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, net_agent);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 600L);
        if (params != "") {
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
        }
        result = curl_easy_perform(curl);
        if (result != CURLE_OK)
            cout << other::getRealTime() << ": CURL ERROR: " << errorBuffer << endl;
        double ul, dl;
        curl_easy_getinfo(curl, CURLINFO_SIZE_UPLOAD, &ul);
        curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &dl);
        infoLock.lock();
        send_dl += dl;
        send_ul += ul;
        infoLock.unlock();
    }
    curl_easy_cleanup(curl);
#ifdef printOut
    cout << endl
         << other::getRealTime() << ": " << url << "-" << params << endl
         << "	" << buffer << endl;
#endif
    return buffer;
}

size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}

string net::upload(string url, string filename, string params)
{
    string buffer = "";
    CURL* curl;
    CURLcode result;
    char errorBuffer[CURL_ERROR_SIZE];
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, net_agent);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 600L);
        if (params != "") {
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
        }
        struct curl_httppost* formpost = NULL;
        struct curl_httppost* lastptr = NULL;
        curl_formadd(&formpost,
            &lastptr,
            CURLFORM_COPYNAME, "file",
            CURLFORM_FILENAME, filename.c_str(),
            CURLFORM_FILE, filename.c_str(),
            CURLFORM_CONTENTTYPE, "multipart/form-data",
            CURLFORM_END);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        result = curl_easy_perform(curl);
        if (result != CURLE_OK)
            cout << other::getRealTime() << ": CURL ERROR: " << errorBuffer << endl;
        double ul, dl;
        curl_easy_getinfo(curl, CURLINFO_SIZE_UPLOAD, &ul);
        curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &dl);
        infoLock.lock();
        upload_dl += dl;
        upload_ul += ul;
        infoLock.unlock();
        curl_formfree(formpost);
    }
    curl_easy_cleanup(curl);
#ifdef printOut
    cout << endl
         << other::getRealTime() << ": " << url << "-" << params << endl
         << "	" << buffer << endl;
#endif
    return buffer;
}

void net::download(string url, string filename, string params)
{
    CURL* curl;
    CURLcode result;
    FILE* file = NULL;
    char errorBuffer[CURL_ERROR_SIZE];
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, net_agent);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 600L);
        if (params != "") {
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
        }
        file = fopen(filename.c_str(), "wb");
        if (file) {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            result = curl_easy_perform(curl);
            if (result != CURLE_OK)
                cout << other::getRealTime() << ": CURL ERROR: " << errorBuffer << endl;
            fclose(file);
        }
        double ul, dl;
        curl_easy_getinfo(curl, CURLINFO_SIZE_UPLOAD, &ul);
        curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &dl);
        infoLock.lock();
        download_dl += dl;
        download_ul += ul;
        infoLock.unlock();
    }
    curl_easy_cleanup(curl);
}

string net::getInfo()
{
    return to_string(send_dl) + " " + to_string(send_ul) + " " + to_string(upload_dl) + " " + to_string(upload_ul) + " " + to_string(download_dl) + " " + to_string(download_ul);
}
