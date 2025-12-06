#ifndef CCSQLITE_DATABASE_H
#define CCSQLITE_DATABASE_H

#include <stdint.h>

#include "cc_sqlite/file_header.h"
#include "cc_sqlite/page_header.h"
#include "cc_sqlite/record.h"
#include "cc_sqlite/schema/row.h"

typedef struct
{
    FILE *file;
    FileHeader file_header;
} Database;

/**
 * Open database
 *
 * @param file File handle
 * @param db [out] Database handle
 * @return Status code
 */
int db_open(FILE *file, Database *db);

/**
 * Get table records
 *
 * @param db Database
 * @param rows [out] Schema table rows
 * @param count [out] Number of table rows
 * @return Status code
 */
int db_get_table_records(const Database *db, const PageHeader *page_header, Record *records[], size_t *count, Arena *const arena);

/**
 * Get schema table rows
 *
 * @param db Database
 * @param rows [out] Schema table rows
 * @param count [out] Number of table rows
 * @return Status code
 */
int db_get_schema_table_rows(const Database *db, SchemaRow *rows[], size_t *count, Arena *const arena);

/**
 * Get table names
 *
 * @param db Database
 * @param names [out] Table names
 * @param count [out] Number of table rows
 * @return Status code
 */
int db_get_table_names(const Database *db, char **names[], size_t *count, Arena *const arena);

/**
 * Get the schema row for the provided table name
 *
 * @param db Database
 * @param table_name Table name to match
 * @param row [out] Schema row
 * @return Status code
 */
int db_get_table_by_name(const Database *db, const char *table_name, SchemaRow *row, Arena *const arena);

/**
 * Get page header
 *
 * @param db Database
 * @param page Database page number (zero-indexed)
 * @param header [out] Page header
 * @return Status code
 */
int db_get_page_header(const Database *db, const size_t page, PageHeader *header);

#endif
