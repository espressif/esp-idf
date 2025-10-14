# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import csv
import os
import sys


def update_csv_suffix(suffix: str) -> None:
    # Hardcoded CSV file path
    CSV_FILE = os.path.join(os.path.dirname(__file__), 'nvs_data.csv')

    # Output CSV file path with suffix
    NEW_CSV_FILE = os.path.join(os.path.dirname(__file__), 'nvs_data_suffix.csv')

    with open(NEW_CSV_FILE, 'w', newline='') as outfile:
        with open(CSV_FILE, 'r', newline='') as infile:
            reader = csv.DictReader(infile)
            fieldnames = reader.fieldnames if reader.fieldnames else []
            writer = csv.DictWriter(outfile, fieldnames=fieldnames)
            writer.writeheader()

            for row in reader:
                if row['value'] == 'TESTSSID':
                    row['value'] = f'TESTSSID_{suffix}'
                elif row['value'] == 'TESTPASSWORD':
                    row['value'] = f'TESTPASSWORD_{suffix}'
                writer.writerow(row)

    print(f'[update_csv.py] Patched CSV with suffix: {suffix}, saved as {NEW_CSV_FILE}')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: python update_csv_suffix.py <suffix>')
        sys.exit(1)

    suffix = sys.argv[1]
    update_csv_suffix(suffix)
