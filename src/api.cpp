#include <string>
#include <string_view>
#include "../include/crow_all.h"
#include "../include/hash_table.h"
#include "../include/avl_tree.h"

using namespace std;
int main()
{
    crow::SimpleApp app;
    HashTable db;
    CROW_ROUTE(app, "/api/create_database").methods(crow::HTTPMethod::POST)([&db](const crow::request &req)
                                                                            {

        auto body = crow::json::load(req.body);
        if(!body || !body.has("owner_key")) {
             crow::json::wvalue res;
            res["status"] = "error";
            res["message"] = "owner_key is required";

            return crow::response(400, res);
        }
        string ownerKey=body["owner_key"].s();
        bool created=db.createKey(ownerKey);

        if(!created){
            crow::json::wvalue res;
            res["status"]="error";
            res["message"]="owner_key already exists";
            return crow::response(409,res);
        }
        crow::json::wvalue res;
        res["status"]="ok";
        res["message"]="database created successfully";
        return crow::response(200,res); });

    CROW_ROUTE(app, "/api/update_record").methods(crow::HTTPMethod::PATCH)([&db](const crow::request &req)
                                                                           {
        auto body= crow::json::load(req.body);
        if(!body || !body.has("owner_key") || !body.has("name") || !body.has("age") || !body.has("weight") || !body.has("cgpa")) {
            crow::json::wvalue res;
            res["status"] = "error";
            res["message"] = "Missing required fields";
            return crow::response(400, res);
        }
        string ownerKey=body["owner_key"].s();
        AVLTree* tree=db.getTree(ownerKey);
        if(tree==nullptr){
            crow::json::wvalue res;
            res["status"]="error";
            res["message"]="database not found";
            return crow::response(404,res);
        }
        string name=body["name"].s();
        int age= body["age"].i();
        double weight=body["weight"].d();
        double cgpa=body["cgpa"].d();
        Record record(name,age,weight,cgpa);
        tree->insert(record);
        crow::json::wvalue res;
        res["status"]="ok";
        res["message"]="record updated successfully";
        return crow:: response(200,res); });

    CROW_ROUTE(app, "/api/display_record")
        .methods(crow::HTTPMethod::GET)([&db](const crow::request &req)
                                        {
    const char *ownerKeyParam = req.url_params.get("owner_key");
    const char *nameParam = req.url_params.get("name");

    if (ownerKeyParam == nullptr || nameParam == nullptr)
    {
        crow::json::wvalue res;
        res["status"] = "error";
        res["message"] = "owner_key and name are required";

        return crow::response(400, res);
    }

    string ownerKey = ownerKeyParam;
    string name = nameParam;

    AVLTree *tree = db.getTree(ownerKey);

    if (tree == nullptr)
    {
        crow::json::wvalue res;
        res["status"] = "error";
        res["message"] = "database not found";

        return crow::response(404, res);
    }

    Record *record = tree->search(name);

    if (record == nullptr)
    {
        crow::json::wvalue res;
        res["status"] = "error";
        res["message"] = "record not found";

        return crow::response(404, res);
    }

    crow::json::wvalue res;

    res["status"] = "ok";
    res["name"] = record->name;
    res["age"] = record->age;
    res["weight"] = record->weight;
    res["cgpa"] = record->cgpa;

    return crow::response(200, res); });
    CROW_ROUTE(app, "/api/delete_record")
        .methods(crow::HTTPMethod::DELETE)([&db](const crow::request &req)
                                           {
    auto body = crow::json::load(req.body);

    if (!body ||
        !body.has("owner_key") ||
        !body.has("name"))
    {
        crow::json::wvalue res;
        res["status"] = "error";
        res["message"] = "owner_key and name are required";

        return crow::response(400, res);
    }

    string ownerKey = body["owner_key"].s();
    string name = body["name"].s();

    AVLTree *tree = db.getTree(ownerKey);

    if (tree == nullptr)
    {
        crow::json::wvalue res;
        res["status"] = "error";
        res["message"] = "database not found";

        return crow::response(404, res);
    }

    Record *record = tree->search(name);

    if (record == nullptr)
    {
        crow::json::wvalue res;
        res["status"] = "error";
        res["message"] = "record not found";

        return crow::response(404, res);
    }

    tree->remove(name);
    crow::json::wvalue res;
    res["status"] = "ok";
    res["message"] = "record deleted successfully";

    return crow::response(200, res); });

    app.port(3000).multithreaded().run();
}
