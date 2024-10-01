#include "pow_cache.h"
#include <logging.h>

// Static member initialization
std::mutex SQLiteCache::db_mutex;  // Ensures thread safety for SQLite access.
sqlite3* SQLiteCache::db = nullptr;  // Shared database connection pointer.

static void PowCacheErrorLogCallback(void* arg, int code, const char* msg)
{
    assert(arg == nullptr);
    LogPrintf("SQLite Error. Code: %d. Message: %s\n", code, msg);
}

// Constructor
SQLiteCache::SQLiteCache(const std::string& dbName) {
    // Lock the mutex to synchronize database access across threads.
    std::lock_guard<std::mutex> lock(db_mutex);

    // If the database connection is not already open, open it.
    if (db == nullptr) {
        const fs::path path = gArgs.GetDataDirNet() / fs::u8path(dbName.c_str());
        int rc = sqlite3_config(SQLITE_CONFIG_LOG, PowCacheErrorLogCallback, nullptr);
        rc = sqlite3_config(SQLITE_CONFIG_SERIALIZED);
        int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;  // Open with read/write permissions and create if necessary.
        sqlite3_initialize();  // Initialize SQLite if not already done.

        // Open the database using the specified path.
        rc = sqlite3_open_v2(path.utf8string().c_str(), &db, flags, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);  // Close the database if opening fails.
            db = nullptr;
        } else {
            setupPragmas();  // Set PRAGMA settings to optimize performance.
            createTable();   // Create the cache table if it doesn't exist.
        }
    }
}

// Destructor: We don’t close the database here, allowing it to be reused by other threads.
// SQLite will close the connection when the program exits.
SQLiteCache::~SQLiteCache() {
    // Database closing logic can be handled when the program terminates.
    // This prevents closing the connection while another thread might be using it.
}

// Inserts or updates a key-value pair in the database.
bool SQLiteCache::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(db_mutex);  // Lock database access for thread safety.

    // SQL statement to insert or replace the key-value pair.
    const std::string sql = "INSERT OR REPLACE INTO Cache (key, value) VALUES (?, ?);";
    sqlite3_stmt* stmt = nullptr;

    // Prepare the SQL statement.
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Bind the key and value to the SQL statement.
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_STATIC);

    // Execute the statement.
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);  // Finalize the statement to avoid memory leaks.
        return false;
    }

    sqlite3_finalize(stmt);  // Finalize the statement after successful execution.
    return true;
}

// Retrieves the value associated with the given key.
std::string SQLiteCache::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(db_mutex);  // Lock database access for thread safety.

    // SQL statement to retrieve the value based on the key.
    const std::string sql = "SELECT value FROM Cache WHERE key = ?;";
    sqlite3_stmt* stmt = nullptr;

    // Prepare the SQL statement.
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return "";
    }

    // Bind the key to the SQL statement.
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_STATIC);

    // Execute the statement and fetch the result.
    rc = sqlite3_step(stmt);
    std::string result;
    if (rc == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        if (text) {
            result = reinterpret_cast<const char*>(text);  // Convert the result to a std::string.
        }
    } else if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);  // Finalize the statement to avoid memory leaks.
    return result;
}

// Create the cache table if it doesn't exist.
void SQLiteCache::createTable() {
    const char* sql = "CREATE TABLE IF NOT EXISTS Cache (key TEXT PRIMARY KEY, value TEXT);";
    char* errMsg = nullptr;

    // Execute the SQL statement to create the table.
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);  // Free the error message memory allocated by SQLite.
    }
}

// Set performance-related PRAGMA settings for the SQLite database.
void SQLiteCache::setupPragmas() {
    // Array of PRAGMA SQL statements.
    const char* pragmas[] = {
        "PRAGMA synchronous = NORMAL;",
        "PRAGMA journal_mode = DELETE;",
        "PRAGMA temp_store = 1;",
        "PRAGMA cache_size = 10000;",
        "PRAGMA journal_size_limit = 1000000;",
        "PRAGMA locking_mode = NORMAL;",
        "PRAGMA count_changes = OFF;"
        "PRAGMA auto_vacuum = 0;"
        "PRAGMA page_size = 4096;"
        "PRAGMA lock_timeout = 5000;"
    };

    // Loop through each PRAGMA statement and execute it.
    for (const char* pragma : pragmas) {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, pragma, nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "PRAGMA error: " << errMsg << std::endl;
            sqlite3_free(errMsg);  // Free the error message memory allocated by SQLite.
        }
    }
}
