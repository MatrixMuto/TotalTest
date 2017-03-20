#include <sys/types.h>
#include <sys/socket.h>

#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

typedef struct {
    int fd;
    int write;
    int read;
}connection_t;


int tcp_open(connection_t *c, struct sockaddr* addr)
{
    int err;
    c->fd = socket(AF_INET, SOCK_STREAM, SOCK_NONBLOCK | SOCK_CLOEXEC);

    err = connect(c->fd, addr, sizeof(*addr));


    return err;
}

connection_t conn_arr[100];


void foo(const char* hostname)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, j;
    size_t len;
    ssize_t nread;
    char buf[500];

    /* Obtain address(es) matching host/port */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    s = getaddrinfo(hostname, NULL, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully connect(2).
       If socket(2) (or connect(2)) fails, we (close the socket
       and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                  /* Success */

        close(sfd);
    }

    if (rp == NULL) {               /* No address surcceeded */
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */
}

int main(int argc, char *argv[])
{
    char *host = "devimages.apple.com.edgekey.net";
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();

    SSL_CTX * ctx = SSL_CTX_new(SSLv23_client_method());
    SSL * ssl;

    X509_STORE *store;
    /* Attempt to use the system's trusted root certificates. */
    store = SSL_CTX_get_cert_store(ctx);
    if (X509_STORE_set_default_paths(store) != 1) {
//        err_openssl("X509_STORE_set_default_paths");
//        goto error;
        fprintf(stderr, "X509_STORE_set_default_paths");
    }

    BIO *bio;
    bio = BIO_new_ssl_connect(ctx);
    BIO_get_ssl(bio, & ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    /* Attempt to connect */
    BIO_set_conn_hostname(bio, "devimages.apple.com.edgekey.net:443");
    /* Verify the connection opened and perform the handshake */
    if(BIO_do_connect(bio) <= 0)
    {
        /* Handle failed connection */
    }

    if(SSL_get_verify_result(ssl) != X509_V_OK)
    {
        /* Handle the failed verification */
    }

    char buf[4096];
    int len = 4096;

    snprintf(buf, len, "GET /streaming/examples/bipbop_16x9/bipbop_16x9_variant.m3u8 HTTP/1.1\r\nHost: devimages.apple.com.edgekey.net\r\n\r\n");
    if(BIO_write(bio, buf, len) <= 0) {}

    int x = BIO_read(bio, buf, len);

    fprintf(stdout, "%s\n", buf);
    /* To reuse the connection, use this line */
    BIO_reset(bio);
    /* To free it from memory, use this line */

    char *p = strstr(buf, "\r\n\r\n");
    p+= 4;
    memset(buf, 0, 4096);
    snprintf(buf, len, "GET /streaming/examples/bipbop_16x9/gear1/prog_index.m3u8 HTTP/1.1\r\nHost: devimages.apple.com.edgekey.net\r\n\r\n");
    BIO_write(bio, buf, len);

    BIO_read(bio, buf, len);
    fprintf(stdout, "%s\n", buf);


    BIO_reset(bio);
    memset(buf, 0, 4096);
    snprintf(buf, len, "GET /streaming/examples/bipbop_16x9/gear1/main.ts HTTP/1.1\r\nHost: devimages.apple.com.edgekey.net\r\n\r\n");
    BIO_write(bio, buf, len);

    BIO_read(bio, buf, len);
    p = strstr(buf, "\r\n\r\n");
    p += 4;
    fprintf(stdout, "%s\n", p);

    BIO_reset(bio);

    //foo(host);
    BIO_free_all(bio);
    return 0;
}