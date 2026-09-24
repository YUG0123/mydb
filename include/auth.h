#ifndef AUTH_H
#define AUTH_H

#include <string>
#include "crow_all.h"

using namespace std;

const string API_KEY = "yug_mydb_2026";

inline bool isAuthorized(const crow::request &req)
{
    string key = req.get_header_value("X-API-Key");

    return key == API_KEY;
}

#endif