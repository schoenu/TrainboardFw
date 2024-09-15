#!/usr/bin/env python3

# Trainboard Firmware
# Copyright (C) 2024 Emile Décosterd
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

print("#######################");
print("Rename firmware file")
print("-----------------------");

import shutil
import sys
import re
import os

cwd = os.getcwd()
print(f"Current working directory: {cwd}")

output_filename = f"{cwd}/.pio/build/trainboard/firmware.bin"
build_info_filename = f"{cwd}/build/BuildInfo.h"
release_folder = f"{cwd}/output/"

def get_version_string():
    try:
        with open(build_info_filename, "r") as f:
            contents = f.read()

        match = re.search(r'#define\s+FW_VERSION_FULL\s+"(\d{1,3}\.\d{1,3}\.\d{1,3}.*)"', contents)

        if match:
            print(f"Found firmware version : {match.group(1)}")
            return match.group(1)
        else:
            print(f"Could not find FW_VERSION_FULL in {build_info_filename}")
            return None
    except FileNotFoundError:
        print(f"Could not find {build_info_filename}")
        return None

def copy_and_rename_file(src_file, dest_dir, new_name):
    # Check if the source file exists
    if not os.path.isfile(src_file):
        print(f"Error: The source file '{src_file}' does not exist.")
        return False

    # Check if the destination directory exists, create it if it doesn't
    if not os.path.isdir(dest_dir):
        os.makedirs(dest_dir)
        print(f"The directory '{dest_dir}' did not exist, it has been created.")

    # Construct the full path of the destination file
    dest_file = os.path.join(dest_dir, new_name)

    # Copy the file
    shutil.copy2(src_file, dest_file)
    print(f"The file '{src_file}' has been copied and renamed to '{dest_file}'.")
    return True


success = True
version_string = get_version_string()
if version_string:
    success = copy_and_rename_file(output_filename, release_folder, f"Trainboard_{version_string}.bin")
else:
    print("Failed to get the version string or to copy output file.")  
    success = False

print("-----------------------");
print("Done")
print("#######################");

if not success:
    sys.exit(-1)

