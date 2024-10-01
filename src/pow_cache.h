#ifndef SQLITE_CACHE_H
#define SQLITE_CACHE_H

#include <mutex>
#include <string>
#include <sqlite3.h>
#include <iostream>
#include <util/fs.h>
#include <common/args.h>
#include <utility>

class SQLiteCache {
public:
    // Constructor: Opens the SQLite database with the provided database name.
    // If the database connection is already established, it won't re-open.
    SQLiteCache(const std::string& dbName);

    // Destructor: Does not close the database connection here, allowing shared use across threads.
    ~SQLiteCache();

    // Inserts or updates a key-value pair in the cache.
    // Returns true on success, false on failure.
    bool set(const std::string& key, const std::string& value);

    // Retrieves the value associated with the given key.
    // Returns an empty string if the key does not exist or on error.
    std::string get(const std::string& key);

private:
    // Mutex to protect shared access to the SQLite database across threads.
    static std::mutex db_mutex;

    // Shared database connection across instances.
    static sqlite3* db;

    // Creates the cache table if it does not already exist.
    void createTable();

    // Set PRAGMA settings for database performance optimizations.
    void setupPragmas();
};

#endif // SQLITE_CACHE_H
