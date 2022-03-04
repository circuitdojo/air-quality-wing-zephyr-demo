/**
 * @author Jared Wolff (hello@jaredwolff.com)
 * @copyright Copyright Circuit Dojo LLC 2022
 */

#ifndef SCAN_H
#define SCAN_H

#include <bluetooth/scan.h>

int init_scan(void);
void start_scan(void);

#endif