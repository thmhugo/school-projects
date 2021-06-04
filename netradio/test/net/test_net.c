#include "../cester.h"
#include "../../src/net/net.h"

CESTER_BEFORE_ALL(_,
    log_set_quiet(true);
)

CESTER_TEST(getaddr_localhost, _,
    in_addr_t addr;

    cester_assert_int_eq(OK, net_get_inaddr("localhost", NULL, &addr));
    cester_assert_str_equal("127.0.0.1", inet_ntoa(*(struct in_addr *) &addr));
)

CESTER_TEST(getaddr_unknown_hostname, _,
    in_addr_t addr;

    cester_assert_int_eq(ADDR_ERR, net_get_inaddr("unknown", NULL, &addr));
)

CESTER_TEST(init_sockaddrin, _,
    struct sockaddr_in addr;

    net_init_sockaddrin(4242, "", &addr);
    cester_assert_int_eq(AF_INET, addr.sin_family);
    cester_assert_int_eq(htons(4242), addr.sin_port);
    cester_assert_int_eq(htonl(INADDR_ANY), addr.sin_addr.s_addr);
)

CESTER_TEST(init_sockaddrin_with_internet_address, _,
    struct sockaddr_in addr;

    net_init_sockaddrin(4242, "127.0.0.5", &addr);
    cester_assert_int_eq(AF_INET, addr.sin_family);
    cester_assert_int_eq(htons(4242), addr.sin_port);
    cester_assert_int_eq(inet_addr("127.0.0.5"), addr.sin_addr.s_addr);
)

CESTER_TEST(explode_a_valid_addressport, _,
    const char *addrport = "localhost:4242";
    in_addr_t res_addr;
    int res_port;

    cester_assert_int_eq(OK, net_explode_inaddr(addrport, &res_addr, &res_port));
    cester_assert_int_eq(htons(4242), res_port);
    cester_assert_str_equal("127.0.0.1", inet_ntoa(*(struct in_addr *) &res_addr));
)

CESTER_TEST(explode_a_non_valid_addressport, _,
    const char *addrport = "localhost4242";
    in_addr_t res_addr;
    int res_port;

    cester_assert_int_eq(ADDR_ERR, net_explode_inaddr(addrport, &res_addr, &res_port));
)

CESTER_TEST(explode_a_non_valid_addressport_2, _,
    const char *addrport = "loca:lhost:4242";
    in_addr_t res_addr;
    int res_port;

    cester_assert_int_eq(ADDR_ERR, net_explode_inaddr(addrport, &res_addr, &res_port));
)

CESTER_TEST(explode_a_non_valid_address_from_an_addressport, _,
    const char *addrport = "unknown:4242";
    in_addr_t res_addr;
    int res_port;

    cester_assert_int_eq(ADDR_ERR, net_explode_inaddr(addrport, &res_addr, &res_port));
)

CESTER_TEST(explode_a_non_valid_port_from_an_addressport, _,
    const char *addrport = "localhost:422";
    in_addr_t res_addr;
    int res_port;

    cester_assert_int_eq(PORT_ERR, net_explode_inaddr(addrport, &res_addr, &res_port));
)

CESTER_TEST(explode_a_non_valid_port_from_an_addressport_2, _,
    const char *addrport = "localhost:42a2";
    in_addr_t res_addr;
    int res_port;

    cester_assert_int_eq(PORT_ERR, net_explode_inaddr(addrport, &res_addr, &res_port));
)

CESTER_TEST(is_ip_address_correct, _, 
    cester_assert_int_eq(TRUE, net_is_address_correct("127.000.000.001"));
)

CESTER_TEST(not_correct_ip_address, _, 
    cester_assert_int_eq(FALSE, net_is_address_correct("127.00.000.001"));
)

CESTER_TEST(not_correct_ip_address_2, _, 
    cester_assert_int_eq(FALSE, net_is_address_correct("127.000.000"));
)

CESTER_TEST(not_correct_ip_address_3, _, 
    cester_assert_int_eq(FALSE, net_is_address_correct("127.000.000."));
)