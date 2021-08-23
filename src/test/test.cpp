#include "test.h"
#include "../level.h"
#include "../physics/collision.h"

#ifdef TESTS_ENABLED

#include <debug.h>

#define RUN_TEST(name) if(!run_test(test_##name, #name)) return 1

void test_print_line(const struct line_seg *line) {
    dbg_printf("(%i, %i), (%i, %i)\n", line->x1, line->y1, line->x2, line->y2);
}

bool test_raycast() {
    const static tile_t tiles[LEVEL_SIZE_Y][LEVEL_SIZE_X] = {
            {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
            {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
            {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
            {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
            {0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
            {0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
            {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
            {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
            {0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
            {0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
            {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
            {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
            {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
            {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
            {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
            {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
            {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
            {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
            {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}
    };
    const static struct line_seg expected_lines[256] = {{1792, 1792, 2304, 1792}, {1792, 1792, 2304, 1804}, {1792, 1792, 2304, 1817}, {1792, 1792, 2304, 1829}, {1792, 1792, 2304, 1842}, {1792, 1792, 2304, 1854}, {1792, 1792, 2304, 1867}, {1792, 1792, 2304, 1880}, {1792, 1792, 2304, 1893}, {1792, 1792, 2304, 1906}, {1792, 1792, 2304, 1919}, {1792, 1792, 2304, 1933}, {1792, 1792, 2304, 1947}, {1792, 1792, 2304, 1960}, {1792, 1792, 2304, 1974}, {1792, 1792, 2304, 1989}, {1792, 1792, 2304, 2004}, {1792, 1792, 2304, 2019}, {1792, 1792, 2304, 2034}, {1792, 1792, 2304, 2049}, {1792, 1792, 2304, 2065}, {1792, 1792, 2304, 2082}, {1792, 1792, 2304, 2098}, {1792, 1792, 2304, 2116}, {1792, 1792, 2304, 2133}, {1792, 1792, 2304, 2152}, {1792, 1792, 2304, 2171}, {1792, 1792, 2304, 2190}, {1792, 1792, 2304, 2212}, {1792, 1792, 2304, 2234}, {1792, 1792, 2304, 2255}, {1792, 1792, 2304, 2279}, {1792, 1792, 2304, 2304}, {1792, 1792, 2304, 2330}, {1792, 1792, 2304, 2357}, {1792, 1792, 2304, 2384}, {1792, 1792, 2304, 2415}, {1792, 1792, 2304, 2449}, {1792, 1792, 2304, 2482}, {1792, 1792, 2304, 2518}, {1792, 1792, 2304, 2559}, {1792, 1792, 2603, 3072}, {1792, 1792, 2559, 3072}, {1792, 1792, 2517, 3072}, {1792, 1792, 2475, 3072}, {1792, 1792, 2435, 3072}, {1792, 1792, 2397, 3072}, {1792, 1792, 2359, 3072}, {1792, 1792, 2322, 3072}, {1792, 1792, 2285, 3072}, {1792, 1792, 2249, 3072}, {1792, 1792, 2213, 3072}, {1792, 1792, 2179, 3072}, {1792, 1792, 2145, 3072}, {1792, 1792, 2111, 3072}, {1792, 1792, 2079, 3072}, {1792, 1792, 2046, 3072}, {1792, 1792, 2013, 3072}, {1792, 1792, 1981, 3072}, {1792, 1792, 1949, 3072}, {1792, 1792, 1917, 3072}, {1792, 1792, 1886, 3072}, {1792, 1792, 1854, 3072}, {1792, 1792, 1823, 3072}, {1792, 1792, 1792, 3072}, {1792, 1792, 1761, 3072}, {1792, 1792, 1730, 3072}, {1792, 1792, 1698, 3072}, {1792, 1792, 1667, 3072}, {1792, 1792, 1635, 3072}, {1792, 1792, 1603, 3072}, {1792, 1792, 1571, 3072}, {1792, 1792, 1538, 3072}, {1792, 1792, 1505, 3072}, {1792, 1792, 1473, 3072}, {1792, 1792, 1439, 3072}, {1792, 1792, 1405, 3072}, {1792, 1792, 1371, 3072}, {1792, 1792, 1536, 2508}, {1792, 1792, 1536, 2456}, {1792, 1792, 1536, 2409}, {1792, 1792, 1536, 2368}, {1792, 1792, 1536, 2332}, {1792, 1792, 1536, 2301}, {1792, 1792, 1536, 2271}, {1792, 1792, 1536, 2243}, {1792, 1792, 1536, 2218}, {1792, 1792, 1536, 2195}, {1792, 1792, 1536, 2175}, {1792, 1792, 1536, 2155}, {1792, 1792, 1536, 2137}, {1792, 1792, 1536, 2120}, {1792, 1792, 1536, 2103}, {1792, 1792, 1536, 2088}, {1792, 1792, 1536, 2074}, {1792, 1792, 1536, 2061}, {1792, 1792, 1536, 2048}, {1792, 1792, 1523, 2048}, {1792, 1792, 1510, 2048}, {1792, 1792, 1496, 2048}, {1792, 1792, 1481, 2048}, {1792, 1792, 1464, 2048}, {1792, 1792, 1447, 2048}, {1792, 1792, 1429, 2048}, {1792, 1792, 1409, 2048}, {1792, 1792, 1389, 2048}, {1792, 1792, 1366, 2048}, {1792, 1792, 1341, 2048}, {1792, 1792, 1313, 2048}, {1792, 1792, 1283, 2048}, {1792, 1792, 256, 2518}, {1792, 1792, 256, 2473}, {1792, 1792, 256, 2428}, {1792, 1792, 256, 2383}, {1792, 1792, 256, 2340}, {1792, 1792, 256, 2298}, {1792, 1792, 256, 2257}, {1792, 1792, 256, 2216}, {1792, 1792, 256, 2175}, {1792, 1792, 256, 2136}, {1792, 1792, 256, 2097}, {1792, 1792, 256, 2057}, {1792, 1792, 256, 2019}, {1792, 1792, 256, 1980}, {1792, 1792, 256, 1943}, {1792, 1792, 256, 1905}, {1792, 1792, 256, 1867}, {1792, 1792, 256, 1829}, {1792, 1792, 256, 1792}, {1792, 1792, 256, 1755}, {1792, 1792, 256, 1717}, {1792, 1792, 256, 1679}, {1792, 1792, 256, 1641}, {1792, 1792, 256, 1604}, {1792, 1792, 256, 1565}, {1792, 1792, 256, 1527}, {1792, 1792, 256, 1487}, {1792, 1792, 256, 1448}, {1792, 1792, 256, 1409}, {1792, 1792, 256, 1368}, {1792, 1792, 256, 1327}, {1792, 1792, 256, 1286}, {1792, 1792, 256, 1244}, {1792, 1792, 256, 1201}, {1792, 1792, 256, 1156}, {1792, 1792, 256, 1111}, {1792, 1792, 256, 1066}, {1792, 1792, 1283, 1536}, {1792, 1792, 1313, 1536}, {1792, 1792, 1341, 1536}, {1792, 1792, 1366, 1536}, {1792, 1792, 1389, 1536}, {1792, 1792, 1409, 1536}, {1792, 1792, 1429, 1536}, {1792, 1792, 1447, 1536}, {1792, 1792, 1464, 1536}, {1792, 1792, 1481, 1536}, {1792, 1792, 1496, 1536}, {1792, 1792, 1510, 1536}, {1792, 1792, 1523, 1536}, {1792, 1792, 1536, 1536}, {1792, 1792, 1536, 1523}, {1792, 1792, 1536, 1510}, {1792, 1792, 1536, 1496}, {1792, 1792, 1536, 1481}, {1792, 1792, 1536, 1464}, {1792, 1792, 1536, 1447}, {1792, 1792, 1536, 1429}, {1792, 1792, 1536, 1409}, {1792, 1792, 1536, 1389}, {1792, 1792, 1536, 1366}, {1792, 1792, 1536, 1341}, {1792, 1792, 1536, 1313}, {1792, 1792, 1536, 1283}, {1792, 1792, 1536, 1252}, {1792, 1792, 1536, 1216}, {1792, 1792, 1536, 1175}, {1792, 1792, 1536, 1128}, {1792, 1792, 1536, 1076}, {1792, 1792, 1286, 256}, {1792, 1792, 1327, 256}, {1792, 1792, 1368, 256}, {1792, 1792, 1409, 256}, {1792, 1792, 1448, 256}, {1792, 1792, 1487, 256}, {1792, 1792, 1527, 256}, {1792, 1792, 1565, 256}, {1792, 1792, 1604, 256}, {1792, 1792, 1641, 256}, {1792, 1792, 1679, 256}, {1792, 1792, 1717, 256}, {1792, 1792, 1755, 256}, {1792, 1792, 1792, 256}, {1792, 1792, 1829, 256}, {1792, 1792, 1867, 256}, {1792, 1792, 1905, 256}, {1792, 1792, 1943, 256}, {1792, 1792, 1980, 256}, {1792, 1792, 2019, 256}, {1792, 1792, 2057, 256}, {1792, 1792, 2097, 256}, {1792, 1792, 2136, 256}, {1792, 1792, 2175, 256}, {1792, 1792, 2216, 256}, {1792, 1792, 2257, 256}, {1792, 1792, 2298, 256}, {1792, 1792, 2340, 256}, {1792, 1792, 2383, 256}, {1792, 1792, 2428, 256}, {1792, 1792, 2473, 256}, {1792, 1792, 2518, 256}, {1792, 1792, 2564, 256}, {1792, 1792, 2612, 256}, {1792, 1792, 2662, 256}, {1792, 1792, 2712, 256}, {1792, 1792, 2765, 256}, {1792, 1792, 2304, 1025}, {1792, 1792, 2304, 1066}, {1792, 1792, 2304, 1102}, {1792, 1792, 2304, 1135}, {1792, 1792, 2304, 1169}, {1792, 1792, 2304, 1200}, {1792, 1792, 2304, 1227}, {1792, 1792, 2304, 1254}, {1792, 1792, 2304, 1280}, {1792, 1792, 2304, 1305}, {1792, 1792, 2304, 1329}, {1792, 1792, 2304, 1350}, {1792, 1792, 2304, 1372}, {1792, 1792, 2304, 1394}, {1792, 1792, 2304, 1413}, {1792, 1792, 2304, 1432}, {1792, 1792, 2304, 1451}, {1792, 1792, 2304, 1468}, {1792, 1792, 2304, 1486}, {1792, 1792, 2304, 1502}, {1792, 1792, 2304, 1519}, {1792, 1792, 2304, 1535}, {1792, 1792, 2304, 1550}, {1792, 1792, 2304, 1565}, {1792, 1792, 2304, 1580}, {1792, 1792, 2304, 1595}, {1792, 1792, 2304, 1610}, {1792, 1792, 2304, 1624}, {1792, 1792, 2304, 1637}, {1792, 1792, 2304, 1651}, {1792, 1792, 2304, 1665}, {1792, 1792, 2304, 1678}, {1792, 1792, 2304, 1691}, {1792, 1792, 2304, 1704}, {1792, 1792, 2304, 1717}, {1792, 1792, 2304, 1730}, {1792, 1792, 2304, 1742}, {1792, 1792, 2304, 1755}, {1792, 1792, 2304, 1767}, {1792, 1792, 2304, 1780}};
    const static uint8_t expected_axes[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for(int angle = 0; angle < 256; angle++) {
        struct line_seg line;
        timer_Enable(1, TIMER_CPU, TIMER_NOINT, TIMER_UP);
        bool axis = raycast(TILE_TO_X_COORD(7), TILE_TO_X_COORD(7), angle << 16, tiles, &line);
        timer_Disable(1);
        const struct line_seg *expected_line = &expected_lines[angle];
        bool lines_match = line.x1 == expected_line->x1 &&
                line.y1 == expected_line->y1 &&
                abs(line.x2 - expected_line->x2) < 16 &&
                abs(line.y2 - expected_line->y2) < 16;
        if(axis != expected_axes[angle] || !lines_match) {
            dbg_printf("Failed for angle %x\n", angle << 16);
            dbg_printf("Expected axis %u, actual %u\n", expected_axes[angle], axis);
            dbg_printf("Expected ");
            test_print_line(expected_line);
            dbg_printf("Actual   ");
            test_print_line(&line);
            return false;
        }
    }
    return true;
}

bool run_test(bool func(), const char *name) {
    timer_Set(1, 0);
    if(func()) {
        dbg_printf("Test `%s` passed, total %f ms\n", name, timer_Get(1) / 8388.608);
        return true;
    } else {
        dbg_printf("Test `%s` failed.\n", name);
        return false;
    }
}

int main() {
    gen_lookups();
    RUN_TEST(raycast);
    return 0;
}
#endif
