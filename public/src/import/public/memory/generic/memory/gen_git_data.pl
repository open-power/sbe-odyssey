#!/usr/bin/perl
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/generic/memory/gen_git_data.pl $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2024
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG

use strict;
use warnings;
use Getopt::Long;

#------------------------------------------------------------------------------
# Print Command Line Help
#------------------------------------------------------------------------------
my $arg_output_dir = undef;

# Get the options from the command line - the rest of @ARGV will
# be filenames
GetOptions( "output-dir=s" => \$arg_output_dir );

if ( !defined($arg_output_dir) )
{
    print("Usage: gen_git_data.pl --output-dir=<output dir>\n");
    print("  This perl script will grab the git information create the following file:\n");
    print("  - mss_git_data.H.       Contains the git commit ID so it can be utilized by the current build\n");
    exit(1);
}

# Uncomment to enable debug output
use Data::Dumper;

#------------------------------------------------------------------------------
# Open output files for writing (maf - memory attribute file)
#------------------------------------------------------------------------------
my $git_data_file = $arg_output_dir;
$git_data_file .= "/";
$git_data_file .= "mss_git_data.H";

# Gets the git commit ID
my $git_commit_id = `git log -1 --pretty=\%H`;
chomp($git_commit_id);

# If git status is not clean, we want to append -dirty to the commit
# ID to reflect that the executable does not necessarily match the
# commit ID. We're not going to try to determine what files are
# modified, just that some files were.
#
# We get the names of modified files by diffing with HEAD. This gets
# both modified files and staged files. Then we get the number of files changed,
# chomp off the new line, and if non-zero we append -dirty.
my $modified_count = `git diff --name-only HEAD | wc -l`;
chomp($modified_count);
if ( $modified_count ne "0" )
{
    $git_commit_id .= "-dirty";
}

open( my $file_fh, ">", $git_data_file );

print {$file_fh} "#ifndef MSS_GIT_DATA_H\n";
print {$file_fh} "#define MSS_GIT_DATA_H\n";
print {$file_fh} "#include <string>\n";
print {$file_fh} "const std::string GIT_COMMIT_ID = \"$git_commit_id\";\n";
print {$file_fh} "#endif\n";

#------------------------------------------------------------------------------
# Close output files
#------------------------------------------------------------------------------
close($file_fh);
