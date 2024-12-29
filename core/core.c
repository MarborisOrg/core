#include <stdio.h>
#include <stdlib.h>
#include <string.h> // برای strdup
#include <mongoc/mongoc.h>

typedef struct {
    char *id; // Simulating ObjectId
    // Add other dynamic fields as needed
} DynamicData;

typedef struct {
    mongoc_client_t *client;
    mongoc_collection_t *collection;
} DbManager;

void db_connect(DbManager *dbManager, const char *dbName) {
    mongoc_init();
    char uri_string[256];
    snprintf(uri_string, sizeof(uri_string), "mongodb://localhost:27017/%s", dbName);
    mongoc_uri_t *uri = mongoc_uri_new(uri_string);
    dbManager->client = mongoc_client_new_from_uri(uri);
    dbManager->collection = mongoc_client_get_collection(dbManager->client, dbName, "DynamicCollection");
    mongoc_uri_destroy(uri);
}

void db_close(DbManager *dbManager) {
    mongoc_collection_destroy(dbManager->collection);
    mongoc_client_destroy(dbManager->client);
    mongoc_cleanup();
}

char *my_strdup(const char *s) {
    size_t len = strlen(s) + 1; // +1 برای '\0'
    char *copy = malloc(len);
    if (copy) {
        memcpy(copy, s, len);
    }
    return copy;
}

void saveData(DbManager *dbManager, DynamicData *data) {
    bson_t *document = bson_new();
    bson_append_utf8(document, "_id", -1, data->id, -1); // Simulating ObjectId
    bson_error_t error;
    if (!mongoc_collection_insert_one(dbManager->collection, document, NULL, NULL, &error)) {
        fprintf(stderr, "Error inserting document: %s\n", error.message);
    }
    bson_destroy(document);
}

DynamicData *fetchDataById(DbManager *dbManager, const char *id) {
    bson_t *query = bson_new();
    bson_append_utf8(query, "_id", -1, id, -1); // Simulating ObjectId
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(dbManager->collection, query, NULL, NULL);
    
    const bson_t *doc;
    if (mongoc_cursor_next(cursor, &doc)) {
        DynamicData *data = malloc(sizeof(DynamicData));
        data->id = my_strdup(id); // Simulating ObjectId
        mongoc_cursor_destroy(cursor);
        bson_destroy(query);
        return data;
    }
    
    mongoc_cursor_destroy(cursor);
    bson_destroy(query);
    return NULL;
}

int main() {
    DbManager dbManager;
    db_connect(&dbManager, "testdb");

    DynamicData data = { .id = "some_id" };
    saveData(&dbManager, &data);

    DynamicData *fetchedData = fetchDataById(&dbManager, "some_id");
    if (fetchedData) {
        printf("Fetched data with ID: %s\n", fetchedData->id);
        free(fetchedData->id);
        free(fetchedData);
    }

    db_close(&dbManager);
    return 0;
}
