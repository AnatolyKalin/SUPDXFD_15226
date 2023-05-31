#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

typedef struct response_t {
    char *data;
    size_t size;
} response_t;

/*
 * https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
 */
static size_t callback(void *data, size_t size, size_t nmemb, void *user_data) {
    size_t real_size = size * nmemb;
    response_t *response = (response_t *) user_data;

    char *ptr = realloc(response->data, response->size + real_size + 1);

    if (ptr == NULL) {
        return 0;  /* out of memory! */
    }

    response->data = ptr;
    memcpy(&(response->data[response->size]), data, real_size);
    response->size += real_size;
    response->data[response->size] = 0;

    return real_size;
}

void try_set_user_and_password(CURL *curl_handle, const char *user, const char *password) {
    if (curl_handle == NULL || user == NULL || password == NULL) {
        return;
    }

    size_t size = strlen(user) + 1 + strlen(password) + 1;
    char *user_and_password = malloc(size);

    snprintf(user_and_password, size, "%s:%s", user, password);
    curl_easy_setopt(curl_handle, CURLOPT_USERPWD, user_and_password);

    free(user_and_password);
}

void perform_get(const char *url, const char *user, const char *password, response_t *response) {
    CURL *curl_handle = curl_easy_init();

    if (curl_handle == NULL || url == NULL || response == NULL) {
        return;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, callback);

    /* we pass our 'response' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) response);

    try_set_user_and_password(curl_handle, user, password);

    CURLcode res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl_handle);
}

/*
 * https://kb.dxfeed.com/en/data-model/instrument-profile-format.html#list-of-types
 */
typedef enum symbol_type_t {
    ST_BOND,
    ST_ETF,
    ST_FOREX,
    ST_FUTURE,
    ST_INDEX,
    ST_MONEY_MARKET_FUND,
    ST_MUTUAL_FUND,
    ST_OPTION,
    ST_PRODUCT,
    ST_STOCK,
    ST_SPREAD,
    ST_WARRANT,
    ST_CFD,
    ST_CATEGORY_FRED,
    ST_OTHER,
    ST_REMOVED,
} symbol_type_t;

const char *symbol_type_to_string(symbol_type_t symbol_type) {
    switch (symbol_type) {
        case ST_BOND:
            return "BOND";
        case ST_ETF:
            return "ETF";
        case ST_FOREX:
            return "FOREX";
        case ST_FUTURE:
            return "FUTURE";
        case ST_INDEX:
            return "INDEX";
        case ST_MONEY_MARKET_FUND:
            return "MONEY_MARKET_FUND";
        case ST_MUTUAL_FUND:
            return "MUTUAL_FUND";
        case ST_OPTION:
            return "OPTION";
        case ST_PRODUCT:
            return "PRODUCT";
        case ST_STOCK:
            return "STOCK";
        case ST_SPREAD:
            return "SPREAD";
        case ST_WARRANT:
            return "WARRANT";
        case ST_CFD:
            return "CFD";
        case ST_CATEGORY_FRED:
            return "CATEGORY:FRED";
        case ST_OTHER:
            return "ST_OTHER";
        case ST_REMOVED:
            return "REMOVED";
    }
}

#define CREATE_BUFFER(SIZE) const size_t BUFFER_SIZE = SIZE; char buffer[BUFFER_SIZE]; memset(buffer, 0, BUFFER_SIZE * sizeof(char))

void get_symbols_by_type(const char *base_url, const char *user, const char *password, symbol_type_t symbol_type,
                         response_t *response) {
    CREATE_BUFFER(2048);

    /*
     * https://kb.dxfeed.com/en/data-model/instrument-profile-format.html#list-of-fields-60638
     * https://kb.dxfeed.com/en/data-model/instrument-profile-format.html#list-of-types
     */
    snprintf(buffer, BUFFER_SIZE, "%s?TYPE=%s", base_url, symbol_type_to_string(symbol_type));

    perform_get(buffer, user, password, response);
}

void get_symbols_by_type_starting_with_symbol_name(const char *base_url, const char *user, const char *password,
                                                   symbol_type_t symbol_type, const char *symbol_name_start,
                                                   response_t *response) {
    if (symbol_name_start == NULL) {
        return;
    }

    CREATE_BUFFER(2048);

    /*
     * https://kb.dxfeed.com/en/data-model/instrument-profile-format.html#list-of-fields-60638
     */
    snprintf(buffer, BUFFER_SIZE, "%s?TYPE=%s&SYMBOL=%s*", base_url, symbol_type_to_string(symbol_type),
             symbol_name_start);

    perform_get(buffer, user, password, response);
}

void get_expire_date_of_option(const char *base_url, const char *user, const char *password, const char *symbol,
                               response_t *response) {
    if (symbol == NULL) {
        return;
    }

    CREATE_BUFFER(2048);

    /*
     * https://kb.dxfeed.com/en/data-model/instrument-profile-format.html#list-of-fields-60638
     */
    snprintf(buffer, BUFFER_SIZE, "%s?TYPE=%s&fields=UNDERLYING,PRODUCT,CFI,STRIKE,EXPIRATION&UNDERLYING=%s", base_url,
             symbol_type_to_string(ST_OPTION),
             symbol);

    perform_get(buffer, user, password, response);
}

void get_call_or_put_strike_price_on_specific_expire_date_of_option(const char *base_url, const char *user,
                                                                    const char *password, const char *symbol,
                                                                    const char *expire_date, int is_call,
                                                                    response_t *response) {
    if (symbol == NULL || expire_date == NULL) {
        return;
    }

    CREATE_BUFFER(2048);

    /*
     * https://kb.dxfeed.com/en/data-model/instrument-profile-format.html#list-of-fields-60638
     * https://en.wikipedia.org/wiki/ISO_10962
     *
     * Call's  CFI starts with `OC*`
     */
    char *cfi = is_call ? "OC*" : "OP*";
    snprintf(buffer, BUFFER_SIZE,
             "%s?TYPE=%s&fields=UNDERLYING,PRODUCT,CFI,STRIKE,EXPIRATION&UNDERLYING=%s&CFI=%s&EXPIRATION=%s", base_url,
             symbol_type_to_string(ST_OPTION),
             symbol, cfi, expire_date);

    perform_get(buffer, user, password, response);
}


int main() {
    /* https://tools.dxfeed.com/ipf?help */

    const char *ipf_url = "https://tools.dxfeed.com/ipf";
    const char *user = "demo";
    const char *password = "demo";

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* 1) */
    {
        response_t response = {0};
        get_symbols_by_type(ipf_url, user, password, ST_STOCK, &response);

        if (response.data != NULL) {
            printf("\n\nALL STOCK SYMBOLS\n----------\n%s\n----------\n", response.data);

            free(response.data);
        }
    }

    /* 2) */
    {
        response_t response = {0};
        char *symbol_name_start = "A";

        get_symbols_by_type_starting_with_symbol_name(ipf_url, user, password, ST_STOCK, symbol_name_start,
                                                      &response);

        if (response.data != NULL) {
            printf("\n\nALL STOCK SYMBOLS STARTING WITH \"%s\"\n----------\n%s\n----------\n",
                   symbol_name_start, response.data);

            free(response.data);
        }
    }

    /* 3) */

    {
        response_t response = {0};

        /* Retrieve stocks (https://tools.dxfeed.com/ipf?TYPE=STOCK), parse symbols and put here ---v */
        char *symbol_name = "AAPL,IBM,META";

        get_expire_date_of_option(ipf_url, user, password, symbol_name,
                                  &response);

        if (response.data != NULL) {
            printf("\n\nALL EXPIRE DATES OF STOCK OPTIONS \"%s\"\n----------\n%s\n----------\n",
                   symbol_name, response.data);

            free(response.data);
        }
    }

    /* 4) */

    {
        /* CALL STRIKE PRICES */
        {
            response_t response = {0};

            /* Retrieve stocks (https://tools.dxfeed.com/ipf?TYPE=STOCK), parse symbols and put here ---v */
            char *symbol_name = "AAPL,IBM,META";
            char *expire_date = "2023-06-23";

            get_call_or_put_strike_price_on_specific_expire_date_of_option(
                    ipf_url, user, password, symbol_name,
                    expire_date, 1,
                    &response);

            if (response.data != NULL) {
                printf("\n\nCALL STRIKE PRICES ON %s OF STOCK OPTIONS \"%s\"\n----------\n%s\n----------\n",
                       expire_date, symbol_name, response.data);

                free(response.data);
            }
        }
        /* PUT STRIKE PRICES */
        {
            response_t response = {0};

            /* Retrieve stocks (https://tools.dxfeed.com/ipf?TYPE=STOCK), parse symbols and put here ---v */
            char *symbol_name = "AAPL,IBM,META";
            char *expire_date = "2023-06-23";

            get_call_or_put_strike_price_on_specific_expire_date_of_option(
                    ipf_url, user, password, symbol_name,
                    expire_date, 0,
                    &response);

            if (response.data != NULL) {
                printf("\n\nPUT STRIKE PRICES ON %s OF STOCK OPTIONS \"%s\"\n----------\n%s\n----------\n",
                       expire_date, symbol_name, response.data);

                free(response.data);
            }
        }
    }

    curl_global_cleanup();

    return 0;
}
