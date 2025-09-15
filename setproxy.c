#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>


/*

Both the following config files have the same format
---
# Comment
proxy="proxy1:port1"
proxy="proxy2:port2"
proxy="proxy3:port3"
no_proxy="ip1,ip2,ip3,ip4"
---
*/

char *default_config_file = "/etc/setproxy/proxy.conf";
char *user_config_file = "~/.config/setproxy/proxy.conf";

char *_env_var_proxy[3] = {"all", "http", "https"};
char *_default_proxy[10] = {NULL}; // Increased array size
int _default_proxy_count = 0; // Track the number of entries
// Initialize no_proxy as a dynamically allocated string to avoid free() issues
char *no_proxy = NULL;
char *default_no_proxy = "";
const char *error_message = "error: setting proxy server is disabled by system administrator";
char *program = "setproxy";
char *program_version = "0.1.0";
time_t default_timeout = 5;

/*
 * Expand the tilde in a path to the user's home directory
 */
char *expand_tilde(const char *path) {
    if (path == NULL || path[0] != '~') {
        return strdup(path);
    }

    const char *home = getenv("HOME");
    if (home == NULL) {
        return strdup(path); // Failed to get HOME directory
    }

    size_t home_len = strlen(home);
    // size_t path_len = strlen(path);
    
    // Skip the tilde
    const char *rest = path + 1;
    
    // If the path is just ~, return home
    if (*rest == '\0') {
        return strdup(home);
    }
    
    // If the next character is /, skip it too
    if (*rest == '/') {
        rest++;
    }
    
    size_t rest_len = strlen(rest);
    char *result = malloc(home_len + rest_len + 2); // +2 for '/' and null terminator
    
    if (result == NULL) {
        return NULL; // Memory allocation failed
    }
    
    strcpy(result, home);
    strcat(result, "/");
    strcat(result, rest);
    
    return result;
}

/*
 * Read proxy settings from configuration file
 */
void read_config_file(const char *config_path) {
    char *expanded_path = expand_tilde(config_path);
    if (expanded_path == NULL) {
        fprintf(stderr, "error: Failed to expand path: %s\n", config_path);
        return;
    }
    
    FILE *file = fopen(expanded_path, "r");
    free(expanded_path);
    
    if (file == NULL) {
        // Silently ignore non-existent config files
        return;
    }
    
    char line[1024];
    char *value;
    
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\0') {
            continue;
        }
        
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        
        if (strncmp(line, "proxy=", 6) == 0) {
            // Extract the proxy value between double quotes
            value = strchr(line + 6, '"');
            if (value != NULL) {
                value++; // Skip the opening double quote
                char *end = strchr(value, '"');
                if (end != NULL) {
                    *end = '\0';
                    
                    // Add to _default_proxy if we have space
                    if (_default_proxy_count < 10) {
                        _default_proxy[_default_proxy_count] = strdup(value);
                        _default_proxy_count++;
                    }
                }
            }
        } else if (strncmp(line, "no_proxy=", 9) == 0) {
            // Extract the no_proxy value between double quotes
            value = strchr(line + 9, '"');
            if (value != NULL) {
                value++; // Skip the opening double quote
                char *end = strchr(value, '"');
                if (end != NULL) {
                    *end = '\0';
                    
                    // Replace the existing no_proxy with the new value
                    if (no_proxy != NULL) {
                        free(no_proxy);
                    }
                    no_proxy = strdup(value);
                }
            }
        }
    }
    
    fclose(file);
}

/*
 * Initialize configuration from files
 */
void init_config() {
    // Initialize no_proxy with default value
    no_proxy = strdup(default_no_proxy);
    
    // First read the system config file
    read_config_file(default_config_file);
    
    // Then read the user config file to override system settings
    read_config_file(user_config_file);
}

/*
 * Convert string to uppercase
 */
void strtoupper(char *s) {
    while (*s) {
        *s = toupper(*s);
        s++;
    }
}

/*
 * Print version & exit
 */
int version()
{
    printf("%s, version %s\n", program, program_version);
    return 0;
}

/*
 * Print help & exit
 */
int help()
{
    printf("%s, version %s\n\nusage: ./%s [--unset|-u] [--version|-v] [--help|-h] [proxy1] [proxy2] [proxy3]\n\n", program, program_version, program);
    return 0;
}


/*
 * Check TCP connectivity
 *
 * @param host
 * @param port
 * @return 0 on success, -1 on failure
 *
 */
int check_tcp_connectivity(char *host, int port) {
    int sock;
    struct sockaddr_in addr;
    int result = -1;
    struct timeval timeout;
    struct hostent *he;
    
    // Create socket with error handling
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr, "error: Failed to create socket\n");
        return -1;
    }
    
    // Set connection timeout (e.g., 5 seconds)
    timeout.tv_sec = default_timeout;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        fprintf(stderr, "warning: Failed to set receive timeout\n");
    }
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        fprintf(stderr, "warning: Failed to set send timeout\n");
    }
    
    // Prepare address structure
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    // First try to parse as an IP address
    addr.sin_addr.s_addr = inet_addr(host);
    
    // If not a valid IP address, try to resolve as a hostname
    if (addr.sin_addr.s_addr == INADDR_NONE) {
        he = gethostbyname(host);
        if (he == NULL) {
            fprintf(stderr, "error: Could not resolve hostname: %s\n", host);
            close(sock);
            return -1;
        }
        // Copy the first address in the list of addresses
        memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
    }
    
    // Try to connect
    result = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    
    // Always close the socket to avoid resource leaks
    close(sock);
    
    return result;
}

/*
 * Check proxy connectivity
 *
 * @param proxy_server (host:port pair)
 * @return 0 on success, -1 on failure
 *
 */
int check_proxy_connectivity(char *proxy_server) {
    int rc = -1;
    
    if (!proxy_server || *proxy_server == '\0') {
        fprintf(stderr, "error: check_proxy_connectivity: empty proxy server\n");
        return -1;
    }
    
    char *ps = strdup(proxy_server);
    if (!ps) {
        fprintf(stderr, "error: check_proxy_connectivity: memory allocation failed\n");
        return -1;
    }
    
    char *host = strtok(ps, ":");
    if (!host || *host == '\0') {
        fprintf(stderr, "error: check_proxy_connectivity: invalid host\n");
        free(ps);
        return -1;
    }
    
    char *port_str = strtok(NULL, ":");
    if (!port_str || *port_str == '\0') {
        fprintf(stderr, "error: check_proxy_connectivity: invalid port\n");
        free(ps);
        return -1;
    }
    
    char *endptr;
    long port = strtol(port_str, &endptr, 10);
    if (*endptr != '\0' || port <= 0 || port > 65535) {
        fprintf(stderr, "error: check_proxy_connectivity: invalid port number\n");
        free(ps);
        return -1;
    }
    
    // Check TCP connectivity
    rc = check_tcp_connectivity(host, (int)port);
    
    free(ps);
    return rc;
}

/*
 * Unset proxy
*/
int unset_proxy() {
    if (isatty(fileno(stdout))) {
        fprintf(stderr, "%s\n", error_message);
        return 1;
    } else {
        char *var;
        for (int i = 0; i < 3; i++) {
            var = _env_var_proxy[i];
            printf("unset %s_proxy\n", var);
        }
        printf("unset no_proxy\n");
        for (int i = 0; i < 3; i++) {
            var = strdup(_env_var_proxy[i]);
            strtoupper(var);
            printf("unset %s_PROXY\n", var);
            free(var);
        }
        printf("unset NO_PROXY\n");
        return 0;
    }
    return 0;
}

/*
 * Checks connectivity to proxy (host:port pair), if successful, sets the proxy; else, returns -1
 *
 * @param proxy_server (host:port pair)
 * @return 0 on success, -1 on failure
 *
 */
int set_proxy(char *proxy_server) {
    if (check_proxy_connectivity(proxy_server) == 0) {
        char *var;
        for (int i = 0; i < 3; i++) {
            var = _env_var_proxy[i];
            printf("export %s_proxy=\"%s\"\n", var, proxy_server);
        }
        printf("export no_proxy=\"%s\"\n", no_proxy);
        for (int i = 0; i < 3; i++) {
            var = strdup(_env_var_proxy[i]);
            strtoupper(var);
            printf("export %s_PROXY=\"%s\"\n", var, proxy_server);
            free(var);
        }
        printf("export NO_PROXY=\"%s\"\n", no_proxy);
        return 0;
    }
    return -1;
}

/*
 * Free allocated memory
 */
void cleanup() {
    // Free any dynamically allocated memory in _default_proxy
    for (int i = 3; i < _default_proxy_count; i++) {
        if (_default_proxy[i] != NULL) {
            free(_default_proxy[i]);
            _default_proxy[i] = NULL;
        }
    }
    
    // Free the dynamically allocated no_proxy
    if (no_proxy != NULL) {
        free(no_proxy);
        no_proxy = NULL;
    }
}

/*
 * Main function
 */
int main(int argc, char *argv[])
{
    int opt;
    char *try_proxy;

    // Load configuration from files
    init_config();
    
    while ((opt = getopt(argc, argv, "uvh")) != -1) {
        switch (opt) {
            case 'u':
                return unset_proxy();
            case 'v':
                return version();
            case 'h':
                return help();
            default:
                cleanup();
                return !help();
        }
    }

    if (isatty(fileno(stdout))) {
        fprintf(stderr, "%s\n", error_message);
        cleanup();
        return 1;
    } else {
        while (optind < argc) {
            try_proxy = argv[optind++];
            if (set_proxy(try_proxy) == 0) {
                cleanup();
                return 0;
            }
        }
        for (int i = 0; i < _default_proxy_count; i++) {
            try_proxy = _default_proxy[i];
            if (set_proxy(try_proxy) == 0) {
                cleanup();
                return 0;
            }
        }
    }
    
    cleanup();
    return 0;
}
