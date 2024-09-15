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
print("Update firmware version")
print("-----------------------");

import subprocess
import sys
import re
import os

cwd = os.getcwd()
print(f"Current working directory: {cwd}")

build_info_filename = f"{cwd}/build/BuildInfo.h"
config_filename = f"{cwd}/src/FwConfig.h"

def get_git_hash():
    try:
        output = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"], universal_newlines=True)
        return output.strip()
    except subprocess.CalledProcessError as e:
        print(f"An error occurred while trying to get the git hash: {str(e)}")
        return None

def get_version_string():
    try:
        with open(config_filename, "r") as f:
            contents = f.read()

        match = re.search(r'#define\s+FW_VERSION\s+"(\d{1,3}\.\d{1,3}\.\d{1,3}).*"', contents)

        if match:
            print(f"Found firmware version : {match.group(1)}")
            return match.group(1)
        else:
            print(f"Could not find FW_VERSION in {config_filename}")
            return None
    except FileNotFoundError:
        print(f"Could not find {config_filename}")
        return None

def write_new_version_string(new_version_string):
    try:

        # Write the new contents back to the file
        with open(build_info_filename, "w") as f:
            code = """
// Trainboard.ch
// Copyright (C) 2024 Emile Decosterd
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""
            code+= "#ifndef BUILD_INFO_H_\n"
            code+= "#define BUILD_INFO_H_\n"
            code+= "\n"
            code+= f"#define FW_VERSION_FULL \"{new_version_string}\"\n"
            code+= "\n"
            code+= "#endif  // BUILD_INFO_H_"
            f.write(code);

        print(f"Successfully wrote the new version string to {build_info_filename}: {new_version_string}")
        return True
    except FileNotFoundError:
        print(f"Could not find {build_info_filename}")
        return False
        
def has_uncommitted_changes():
    try:
        # Run the git diff-index command, which compares the HEAD commit with the index and working tree
        subprocess.call(["git", "update-index", "-q", "--refresh"]);
        _ = subprocess.check_output(["git", "diff-index", "--quiet", "HEAD", "--"], stderr=subprocess.STDOUT, universal_newlines=True)
        return False  # No output means there are no uncommitted changes
    except subprocess.CalledProcessError:
        return True 
        
# Get the version string and the git hash
version_string = get_version_string()
git_hash = get_git_hash()
git_is_dirty = has_uncommitted_changes()

success = True
if version_string and git_hash:
    new_version_string = f"{version_string}-{git_hash}"
    if (git_is_dirty):
        new_version_string = f"{new_version_string}-dirty"
    success = write_new_version_string(new_version_string)
else:
    print("Failed to get the version string or the git hash.")  
    success = False

print("-----------------------");
print("Done")
print("#######################");

if not success:
    sys.exit(-1)

