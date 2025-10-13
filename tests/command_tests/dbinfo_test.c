#include "dbinfo_test.h"

#include <stdio.h>
#include <stdlib.h>

#include "cc_sqlite/commands/dbinfo.h"
#include "cc_sqlite/database.h"

MunitResult commands_dbinfo_test_page_size(const MunitParameter params[], void *user_data)
{
    FILE *file = fopen(EXAMPLES_DIR "/sample.db", "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", EXAMPLES_DIR "/sample.db");
        return MUNIT_FAIL;
    }

    Database db;
    int err = db_open(file, &db);
    if (err != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to open database:\n");
        return MUNIT_FAIL;
    }

    DbInfo info;
    err = get_dbinfo(&db, &info);
    if (err != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to get database info\n");
        return MUNIT_FAIL;
    }

    munit_assert_uint16(info.page_size, ==, 4096);

    return MUNIT_OK;
}

MunitResult commands_dbinfo_test_num_tables(const MunitParameter params[], void *user_data)
{
    FILE *file = fopen(EXAMPLES_DIR "/sample.db", "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", EXAMPLES_DIR "/sample.db");
        return MUNIT_FAIL;
    }

    Database db;
    int err = db_open(file, &db);
    if (err != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to open database:\n");
        return MUNIT_FAIL;
    }

    DbInfo info;
    err = get_dbinfo(&db, &info);
    if (err != EXIT_SUCCESS)
    {
        fprintf(stderr, "Failed to get database info\n");
        return MUNIT_FAIL;
    }

    munit_assert_uint16(info.num_tables, ==, 3);

    return MUNIT_OK;
}
