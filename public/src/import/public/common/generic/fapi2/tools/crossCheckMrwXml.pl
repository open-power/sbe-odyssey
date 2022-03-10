#!/usr/bin/env perl
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/generic/fapi2/tools/crossCheckMrwXml.pl $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2021,2022
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
use File::Basename;

#
# This script verifies that all MRW attributes (1) identified in attribute_ids.H
# have a value defined (2)
#
# (1) MRW attribute = <platInit>mrw</platInit>
#
# (2) value defined = <initTo*> tag OR
#                     <default> tag OR
#                     definition in every <system>.mrw.xml file
#
# The script runs during EKB build process and will fail the build if there's a
# missing MRW value assignment.
#
#   Output error file:
#      - mrw_crosscheck_errors.txt         Lists all MRW attribute which do not
#                                          have a value defined in system MRW XML
#
#   Output debug files:
#      - attrs_platinit_mrw.txt            Lists all platInit attributes which
#                                          are expected to be provided by MRW
#
#      - attrs_platinit_mrw_required.txt   Lists all attributes required to
#                                          have a value defined in system MRW XML
#
#      - attrs_platinit_non_mrw.txt        Lists all platInit attributes which
#                                          ARE NOT expected to be provided by MRW
#
#      - attrs_non_platinit.txt            Lists all NON platInit attributes
#

#--------------------------------------------------------------------------------
# P11 system specific properties
#--------------------------------------------------------------------------------
# used to check/ensure MRW required attribute values are specified for each
# expected target instance
my %nps = (
    'VBU_P11'      => 1,
    'WAFER_TAP'    => 1,
    'WAFER_SPINAL' => 1,
);

my %cpn = (
    'VBU_P11'      => 1,
    'WAFER_TAP'    => 1,
    'WAFER_SPINAL' => 1,
);

#--------------------------------------------------------------------------------
# P11 chip specific properties
#--------------------------------------------------------------------------------

sub instances_to_check($$)
{
    my $mrw_name = $_[0];
    my $type     = $_[1];

    my @ret;

    if ( $type eq "TARGET_TYPE_SYSTEM" )
    {
        push( @ret, "sys0" );
    }
    else
    {
        for ( my $n = 0; $n < $nps{$mrw_name}; $n++ )
        {
            for ( my $p = 0; $p < $cpn{$mrw_name}; $p++ )
            {
                my $chipstr = "sys0node${n}proc${p}";
                if ( $type eq "TARGET_TYPE_COMPUTE_CHIP" )
                {
                    push( @ret, $chipstr );
                }
                elsif ( $type eq "TARGET_TYPE_HUB_CHIP" )
                {
                    push( @ret, $chipstr );
                }
                elsif ( $type eq "TARGET_TYPE_CORE" )
                {
                    for ( my $eq = 0; $eq < 2; $eq++ )
                    {
                        for ( my $c = 0; $c < 4; $c++ )
                        {
                            push( @ret, "${chipstr}eq${eq}core${c}" );
                        }
                    }
                }
                elsif ( $type eq "TARGET_TYPE_L3CACHE" )
                {
                    for ( my $eq = 0; $eq < 2; $eq++ )
                    {
                        for ( my $c = 0; $c < 4; $c++ )
                        {
                            push( @ret, "${chipstr}eq${eq}l3${c}" );
                        }
                    }
                }
                elsif ( $type eq "TARGET_TYPE_EQ" )
                {
                    for ( my $eq = 0; $eq < 2; $eq++ )
                    {
                        push( @ret, "${chipstr}eq${eq}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_PAU" )
                {
                    for ( my $pau = 0; $pau < 8; $pau++ )
                    {
                        push( @ret, "${chipstr}pau${pau}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_INT" )
                {
                    for ( my $int = 0; $int < 2; $int++ )
                    {
                        push( @ret, "${chipstr}int${int}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_NX" )
                {
                    for ( my $nx = 0; $nx < 2; $nx++ )
                    {
                        push( @ret, "${chipstr}nx${nx}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_MC" )
                {
                    for ( my $mc = 0; $mc < 4; $mc++ )
                    {
                        push( @ret, "${chipstr}mc${mc}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_OMI" )
                {
                    for ( my $mc = 0; $mc < 4; $mc++ )
                    {
                        for ( my $mcc = 0; $mcc < 4; $mcc++ )
                        {
                            for ( my $omi = 0; $omi < 2; $omi++ )
                            {
                                push( @ret, "${chipstr}mc${mc}mi0mcc${mcc}omi${omi}" );
                            }
                        }
                    }
                }
                elsif ( $type eq "TARGET_TYPE_IOHS" )
                {
                    for ( my $iohs = 0; $iohs < 12; $iohs++ )
                    {
                        push( @ret, "${chipstr}iohs${iohs}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_PAX" )
                {
                    for ( my $pax = 0; $pax < 8; $pax++ )
                    {
                        push( @ret, "${chipstr}pax${pax}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_PAXO" )
                {
                    for ( my $paxo = 0; $paxo < 4; $paxo++ )
                    {
                        push( @ret, "${chipstr}paxo${paxo}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_PEC2P" )
                {
                    for ( my $pec = 0; $pec < 2; $pec++ )
                    {
                        push( @ret, "${chipstr}pec2p${pec}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_PEC6P" )
                {
                    for ( my $pec = 0; $pec < 4; $pec++ )
                    {
                        push( @ret, "${chipstr}pec6p${pec}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_PHB248X" )
                {
                    for ( my $phb = 0; $phb < 24; $phb++ )
                    {
                        push( @ret, "${chipstr}phb248x${phb}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_PHB16X" )
                {
                    for ( my $phb = 0; $phb < 6; $phb++ )
                    {
                        push( @ret, "${chipstr}phb16x${phb}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_TBUSC" )
                {
                    for ( my $tbusc = 0; $tbusc < 4; $tbusc++ )
                    {
                        push( @ret, "${chipstr}tbusc${tbusc}" );
                    }
                }
                elsif ( $type eq "TARGET_TYPE_TBUSL" )
                {
                    for ( my $tbusl = 0; $tbusl < 8; $tbusl++ )
                    {
                        push( @ret, "${chipstr}tbusl${tbusl}" );
                    }
                }
                else
                {
                    die "Unsupported target: $type!";
                }
            }
        }
    }
    return @ret;
}

my %CSV_ATTR_SKIP = (
    "ATTR_FAPI_POS" => 1,
    "ATTR_REL_POS"  => 1,
);

#--------------------------------------------------------------------------------
# FAPI vs MRW target type arrays, they must be in sync.
# Used to look up a matched target type in MRW xml file given a FAPI target type
#--------------------------------------------------------------------------------
my %fapi_to_sys_xml_type_map = (
    'TARGET_TYPE_SYSTEM' => 'sys-sys-power11',
    'TARGET_TYPE_DIMM'   => 'lcard-dimm-ddimm',
    'TARGET_TYPE_PERV'   => 'unit-perv-power11',

    'TARGET_TYPE_COMPUTE_CHIP' => 'chip-processor-power11-compute',
    'TARGET_TYPE_CORE'         => 'unit-core-power11',
    'TARGET_TYPE_L3CACHE'      => 'unit-l3cache-power11',
    'TARGET_TYPE_EQ'           => 'unit-eq-power11',
    'TARGET_TYPE_TBUSL'        => 'unit-tbusl-power11',

    'TARGET_TYPE_HUB_CHIP' => 'chip-processor-power11-hub',
    'TARGET_TYPE_PAU'      => 'unit-pau-power11',
    'TARGET_TYPE_INT'      => 'unit-int-power11',
    'TARGET_TYPE_NX'       => 'unit-nx-power11',
    'TARGET_TYPE_MC'       => 'unit-mc-power11',
    'TARGET_TYPE_MI'       => 'unit-mi-power11',
    'TARGET_TYPE_MCC'      => 'unit-mcc-power11',
    'TARGET_TYPE_OMIC'     => 'unit-omic-power11',
    'TARGET_TYPE_OMI'      => 'unit-omi-power11',
    'TARGET_TYPE_IOHS'     => 'unit-iohs-power11',
    'TARGET_TYPE_PAX'      => 'unit-pax-power11',
    'TARGET_TYPE_PAXO'     => 'unit-paxo-power11',
    'TARGET_TYPE_PEC6P'    => 'unit-pec6p-power11',
    'TARGET_TYPE_PEC2P'    => 'unit-pec2p-power11',
    'TARGET_TYPE_PHB248X'  => 'unit-phb248x-power11',
    'TARGET_TYPE_PHB16X'   => 'unit-phb16x-power11',
    'TARGET_TYPE_TBUS'     => 'unit-tbus-power11',
    'TARGET_TYPE_TBUSL'    => 'unit-tbusl-power11',

    'TARGET_TYPE_OCMB_CHIP' => 'chip-ocmb',
    'TARGET_TYPE_MEM_PORT'  => 'unit-mem_port',
    'TARGET_TYPE_PMIC'      => 'chip-vreg-generic',
);

my %sys_xml_to_fapi_type_map;
foreach my $key ( keys(%fapi_to_sys_xml_type_map) )
{
    $sys_xml_to_fapi_type_map{ $fapi_to_sys_xml_type_map{$key} } = $key;
    if ( $key eq "TARGET_TYPE_DIMM" )
    {
        $sys_xml_to_fapi_type_map{ $fapi_to_sys_xml_type_map{$key} . "4u" } = $key;
    }
}

my %sys_xml_type_exclusions = (
    'unit-fc-power11'      => 1,
    'unit-smplink-power11' => 1,
    'unit-occ-power11'     => 1,
    'unit-spi-master'      => 1,
    'unit-abus-power11'    => 1,
    'chip-adc'             => 1,
    'chip-PCA9554'         => 1,
    'chip-bmc-ast2600'     => 1,
    'chip-vreg-generic'    => 1,
    'enc-node-power11'     => 1,
);

# MRW data source value
# Must be the same as @platInitVals in hwpf/fapi2/tools/parseAttributeInfo.pl
my $plat_init_src_mrw = 3;

#------------------------------------------------------------------------------
# Debug setting
#------------------------------------------------------------------------------
# debug = 0  - No debug print
#         1  - Process files
#         2  - #1 + processed data from files
my $debug = 0;

#------------------------------------------------------------------------------
# Print Command Line Help
#------------------------------------------------------------------------------
# Default input/output directories to be current directory
# Specify $attr_id_file_dir for debug purposes.
my $attr_id_file_dir = ".";
my $out_files_dir    = ".";
my $help             = 0;
GetOptions(
    "attribute-ids-dir=s" => \$attr_id_file_dir,
    "output-dir=s"        => \$out_files_dir,
    "debug:i"             => \$debug,
    "help"                => \$help
);

my $numArgs = $#ARGV + 1;
if ( ($help) || ( $numArgs < 1 ) )
{
    print(
        "Usage: crossCheckMrwXml.pl --attribute-ids-dir=<attribute info dir> --output-dir=<output dir> [<mrw-xml-file1>] [<mrw-xml-file2> ...]\n"
    );
    print("  This perl script will verify that all Plat Init MRW attributes exist in input MRW files\n");
    print("  --attribute-ids-dir  Directory where attribute_ids.H file is located.\n");
    print("  --output-dir         Directory to output cross check error file.\n");
    exit(1);
}

# Debug
if ( $debug > 0 )
{
    print "attribute_ids.H dir = $attr_id_file_dir\n";
    print "Output files dir = $out_files_dir\n";
    print "Input MRW XML files: \n";
    foreach my $argnum ( 0 .. $#ARGV )
    {
        my $mrw_file = $ARGV[$argnum];
        print "  $mrw_file\n";
    }
}

#------------------------------------------------------------------------------
# Specify perl modules to use
#------------------------------------------------------------------------------
use Digest::MD5 qw(md5_hex);
use XML::Simple;
my $xml = new XML::Simple( KeyAttr => [] );

# Uncomment to enable debug output
# use Data::Dumper;

#------------------------------------------------------------------------------
# Set PREFERRED_PARSER to XML::Parser. Otherwise it uses XML::SAX which contains
# bugs that result in XML parse errors that can be fixed by adjusting white-
# space (i.e. parse errors that do not make sense).
#------------------------------------------------------------------------------
$XML::Simple::PREFERRED_PARSER = 'XML::Parser';

#------------------------------------------------------------------------------
# attribute arrays/output files
#------------------------------------------------------------------------------

my %platInitMrwAttrs    = ();
my $platInitMrwAttrFile = $out_files_dir;
$platInitMrwAttrFile .= "/";
$platInitMrwAttrFile .= "attrs_platinit_mrw.txt";
open( ATTRS_PLATINIT_MRW_FILE, ">", $platInitMrwAttrFile );
print ATTRS_PLATINIT_MRW_FILE "// This file is generated by perl script crossCheckMrwXml.pl.\n";
print ATTRS_PLATINIT_MRW_FILE "// It contains the list of MRW PlatInit attributes in the attribute_ids.H file.\n\n";

my %platInitMrwRequiredAttrs    = ();
my $platInitMrwRequiredAttrFile = $out_files_dir;
$platInitMrwRequiredAttrFile .= "/";
$platInitMrwRequiredAttrFile .= "attrs_platinit_mrw_required.txt";
open( ATTRS_PLATINIT_MRW_REQUIRED_FILE, ">", $platInitMrwRequiredAttrFile );
print ATTRS_PLATINIT_MRW_REQUIRED_FILE "// This file is generated by perl script crossCheckMrwXml.pl.\n";
print ATTRS_PLATINIT_MRW_REQUIRED_FILE
    "// It contains the list of MRW attributes that don't have InitToZero nor Default value\n";
print ATTRS_PLATINIT_MRW_REQUIRED_FILE "// specified.\n\n";

my %platInitNonMrwAttrs    = ();
my $platInitNonMrwAttrFile = $out_files_dir;
$platInitNonMrwAttrFile .= "/";
$platInitNonMrwAttrFile .= "attrs_platinit_non_mrw.txt";
open( ATTRS_PLATINIT_NON_MRW_FILE, ">", $platInitNonMrwAttrFile );
print ATTRS_PLATINIT_NON_MRW_FILE "// This file is generated by perl script crossCheckMrwXml.pl.\n";
print ATTRS_PLATINIT_NON_MRW_FILE
    "// It contains the list of non-MRW PlatInit attributes in the attribute_ids.H file.\n\n";

my %nonPlatInitAttrs    = ();
my $nonPlatInitAttrFile = $out_files_dir;
$nonPlatInitAttrFile .= "/";
$nonPlatInitAttrFile .= "attrs_non_platinit.txt";
open( ATTRS_NON_PLATINIT_FILE, ">", $nonPlatInitAttrFile );
print ATTRS_NON_PLATINIT_FILE "// This file is generated by perl script crossCheckMrwXml.pl.\n";
print ATTRS_NON_PLATINIT_FILE "// It contains the list of non-PlatInit attributes in the attribute_ids.H file.\n\n";

my %mrwSpecifiedAttrs    = ();
my $mrwSpecifiedAttrFile = $out_files_dir;
$mrwSpecifiedAttrFile .= "/";
$mrwSpecifiedAttrFile .= "mrw_specified_attrs.txt";
open( MRW_SPECIFIED_ATTR_FILE, ">", $mrwSpecifiedAttrFile );
print MRW_SPECIFIED_ATTR_FILE "// This file is generated by perl script crossCheckMrwXml.pl.\n";
print MRW_SPECIFIED_ATTR_FILE "// It contains the list of attribute references across all System MRW XML files.\n\n";

# Error output file
my $errFile = $out_files_dir;
$errFile .= "/";
$errFile .= "mrw_crosscheck_errors.txt";
open( ERRFILE, ">", $errFile );
print ERRFILE "// mrw_crosscheck_errors.txt\n";
print ERRFILE "// This file is generated by perl script crossCheckMrwXml.pl.\n";
print ERRFILE "// It contains all MRW attributes which do not have values specified by either:\n";
print ERRFILE "// - attribute definition XML (via initTo or default value tags)\n";
print ERRFILE "// - system MRW XML (<system>.mrw.xml files).\n\n";

#------------------------------------------------------------------------------
# Open input file attribute_ids.H
#------------------------------------------------------------------------------
my $inFile = $attr_id_file_dir;
$inFile .= "/";
$inFile .= "attribute_ids.H";
open INFILE, "<", $inFile or die "crossCheckMrwXml> can not open $inFile";

# ----------------------------------------------------------------------------------
# Examine each line in attribute_ids.H file and build hashes
# ----------------------------------------------------------------------------------
# Read file
while ( my $line = <INFILE> )
{
    # Remove leading and trailing whitespace
    $line =~ s/^\s+|\s+$//g;

    # Debug print
    if ( $debug > 1 )
    {
        print "Line scan: $line\n";
    }

    # Look for first line where an attribute is defined
    my @attr_defs;
    if ( $line =~ m/^\/\* ATTR_.* \*\// )    # Look for '/* ATTR_... */'
    {
        # Read remaining lines of this attr definition
        my $last_read_line = 0;
        while ( $line = <INFILE> )
        {
            # Remove leading and trailing whitespace
            $line =~ s/^\s+|\s+$//g;

            # If not hitting next attribute definition yet
            if ( $line !~ m/^\/\* ATTR_.* \*\// )
            {
                push @attr_defs, $line;      # Save all definition lines of this attribute
                $last_read_line = tell(INFILE);    # Save file pos
            }
            else
            {
                # Revert file position to the last line of the attribute just read
                seek( INFILE, $last_read_line, 0 );
                last;                              # Get out of inside while-loop
            }
        }

        # Debug print: print attribute definition just read
        if ( $debug > 1 )
        {
            foreach my $attr_inf (@attr_defs)
            {
                print "  $attr_inf\n";
            }
        }

        # Process the collected attribute definition
        my $init_to_zero_specified = "false";
        my $default_specified      = "false";
        my $plat_init              = "false";
        my $fapi2_target_type      = undef;
        my $mrw_target_type        = undef;
        my $attr_name              = undef;

        foreach my $attr_inf (@attr_defs)
        {
            # Get Name
            if ( $attr_inf =~ m/^typedef\s\S+\s+(\S+)_Type/ )
            {
                $attr_name = $1;
            }

            # Get InitToZero setting
            if ( $attr_inf =~ m/_InitToZero = true/ )    # InitToZero = true
            {
                $init_to_zero_specified = "true";
            }

            # Get default setting
            if ( $attr_inf =~ m/_hasDefault = true/ )    # Default = true
            {
                $default_specified = "true";
            }

            # Get attribute fapi2 target type, find line with '_TargetType = '
            if ( $attr_inf =~ m/_TargetType = / )
            {
                #printf("*** $attr_inf\n");

                # Extract target type from line
                my @l_splitTargetLine1 = split / = fapi2::/, $attr_inf, 2;
                my @l_splitTargetLine2 = split /;/, $l_splitTargetLine1[1], 2;
                $fapi2_target_type = $l_splitTargetLine2[0];
                $fapi2_target_type =~ s/fapi2:://g;
            }

            # PlatInit line
            if ( $attr_inf =~ m/_PlatInit =/ )
            {
                if ( $attr_inf !~ m/_PlatInit = 0/ )
                {
                    $plat_init = "true";

                    # PlatInit = mrw
                    if ( $attr_inf =~ m/_PlatInit = $plat_init_src_mrw/ )
                    {
                        $plat_init = "mrw";
                    }
                }
            }
        }

        # Debug print
        if ( $debug > 1 )
        {
            print
                "Attr info: init_to_zero $init_to_zero_specified, default $default_specified, fapi2_target_type $fapi2_target_type, plat_init $plat_init\n\n";
        }

        # Save attribute to appropriate lists
        if ( ( $plat_init eq "true" ) || ( $plat_init eq "mrw" ) )
        {
            if ( $plat_init eq "true" )
            {
                $platInitNonMrwAttrs{$attr_name} = $fapi2_target_type;
            }
            else
            {
                $platInitMrwAttrs{$attr_name} = $fapi2_target_type;

                if (   ( $init_to_zero_specified ne "true" )
                    && ( $default_specified ne "true" ) )
                {
                    my $sys_xml_type;

                    # Look up for equivalent target instance type use in MRW xml file
                    # ex: <type>unit-eq-power11</type>
                    my @keys = split( /\|/, $fapi2_target_type );
                    foreach my $key (@keys)
                    {
                        $key =~ s/^\s+//;
                        $key =~ s/\s$//;
                        if ( defined( $fapi_to_sys_xml_type_map{$key} ) )
                        {
                            $sys_xml_type = $fapi_to_sys_xml_type_map{$key};
                        }
                        else
                        {
                            print(
                                "ERROR: crossCheckMrwXml.pl -> Unsupported FAPI2 target type: '$key' for attribute name: '$attr_name'.\n"
                            );
                            exit(1);
                        }
                    }
                    $platInitMrwRequiredAttrs{$attr_name} = $sys_xml_type;
                }
            }
        }
        else
        {
            $nonPlatInitAttrs{$attr_name} = $fapi2_target_type;
        }
    }
}

# Print attribute categories to output files
foreach my $attr ( sort keys %platInitMrwAttrs )
{
    print ATTRS_PLATINIT_MRW_FILE "$attr, $platInitMrwAttrs{$attr}\n";
}

foreach my $attr ( sort keys %platInitMrwRequiredAttrs )
{
    print ATTRS_PLATINIT_MRW_REQUIRED_FILE "$attr, $platInitMrwRequiredAttrs{$attr}\n";
}

foreach my $attr ( sort keys %platInitNonMrwAttrs )
{
    print ATTRS_PLATINIT_NON_MRW_FILE "$attr, $platInitNonMrwAttrs{$attr}\n";
}

foreach my $attr ( sort keys %nonPlatInitAttrs )
{
    print ATTRS_NON_PLATINIT_FILE "$attr, $nonPlatInitAttrs{$attr}\n";
}

# ----------------------------------------------------------------------------------
# Extract attributes from  <system>.mrw.xml files
# ----------------------------------------------------------------------------------

my %mrw_attrs_flattened;
my %mrw_attrs;
foreach my $argnum ( 0 .. $#ARGV )
{
    my $infile = $ARGV[$argnum];

    if ( $debug > 1 )
    {
        print "Processing MRW attributes in: $infile\n";
    }

    # Read XML file. The ForceArray option ensures that there is an array of
    # elements even if there is only one such element in the file
    my $target_instances = $xml->XMLin( $infile, ForceArray => [ 'targetInstance', 'attribute' ] );

    my ( $mrw_name, $bleh1, $bleh2 ) = fileparse( $infile, ("_hb.mrw.xml") );

    foreach my $target_instance ( @{ $target_instances->{targetInstance} } )
    {
        my $instance_id;
        if ( !exists $target_instance->{type} )
        {
            if ( !exists $target_instance->{id} )
            {
                print("ERROR: Missing target instance ID for $target_instance, file $infile. \n");
                exit(1);

            }

            $instance_id = $target_instance->{id};

            if ( $instance_id =~ /oscrefclk/ )
            {
                next;
            }
            print("ERROR: Missing target type entry for $instance_id, file $infile. \n");
            exit(1);
        }

        $instance_id = $target_instance->{id};

        my $sys_xml_key = $target_instance->{type};
        if ( defined( $sys_xml_type_exclusions{$sys_xml_key} ) )
        {
            next;
        }

        my $fapi_target_type;
        if ( defined( $sys_xml_to_fapi_type_map{$sys_xml_key} ) )
        {
            $fapi_target_type = $sys_xml_to_fapi_type_map{$sys_xml_key};
        }
        else
        {
            print("ERROR: crossCheckMrwXml.pl -> Unsupported MRW target instance type: '$sys_xml_key'.\n");
            exit(1);
        }

        #       printf("*** $sys_xml_key $fapi_target_type $instance_id\n");

        foreach my $attr ( @{ $target_instance->{attribute} } )
        {
            my $attr_name  = $attr->{id};
            my $attr_value = $attr->{default};
            $attr_name =~ s/^\s+//;
            $attr_name =~ s/\s+$//;
            $attr_name = "ATTR_" . $attr_name;

            $mrw_attrs_flattened{$attr_name}{$fapi_target_type}{$instance_id} = ${attr_value};
            $mrw_attrs{$mrw_name}{$attr_name}{$fapi_target_type}{$instance_id} = ${attr_value};

            #           printf("*** $sys_xml_key $attr->{id}\n");
        }
    }
}

foreach my $attr ( sort { $a cmp $b } keys %mrw_attrs_flattened )
{
    my $target_str = "";
    my @target_list;
    foreach my $target_type ( keys %{ $mrw_attrs_flattened{$attr} } )
    {
        $target_str .= $target_type . " | ";
        push( @target_list, $target_type );
    }
    $target_str =~ s/ \| $//;

    my $prefix = "x";

    if ( defined( $platInitMrwAttrs{$attr} ) )    { $prefix = "m"; }
    if ( defined( $platInitNonMrwAttrs{$attr} ) ) { $prefix = "p"; }
    if ( defined( $nonPlatInitAttrs{$attr} ) )    { $prefix = "n"; }

    print MRW_SPECIFIED_ATTR_FILE "${prefix} ${attr},$target_str\n";

    if (   ( $prefix ne "x" )
        && ( !defined( $CSV_ATTR_SKIP{$attr} ) ) )
    {
        foreach my $mrw_name ( sort keys %mrw_attrs )
        {
            if ( defined( $mrw_attrs{$mrw_name}{$attr} ) )
            {
                foreach my $target_type (@target_list)
                {
                    if ( defined( $mrw_attrs{$mrw_name}{$attr}{$target_type} ) )
                    {
                        my $str = "${attr}|${mrw_name}|${target_type},";
                        foreach my $instance ( sort keys %{ $mrw_attrs{$mrw_name}{$attr}{$target_type} } )
                        {
                            $str .= "$instance=$mrw_attrs{$mrw_name}{$attr}{$target_type}{$instance}|";
                        }
                        chop($str);
                        print MRW_SPECIFIED_ATTR_FILE "$str\n";
                    }
                }
            }
        }
    }
}

#--------------------------------------------------------------------------
# Cross check each MRW file to ensure that EKB XML tagged as <mrw> are
# present
#--------------------------------------------------------------------------
# Global error
my $global_error = 0;

foreach my $mrw_name ( keys %mrw_attrs )
{
    if ($debug)
    {
        printf "Checking $mrw_name...\n";
    }

    foreach my $attr ( sort keys %platInitMrwRequiredAttrs )
    {
        #    print "  $attr\n";

        # check that attribute is defined in this MRW
        if ( !defined( $mrw_attrs{$mrw_name}{$attr} ) )
        {
            print ERRFILE "$mrw_name : Entry for $attr NOT found!\n";
            $global_error = 1;
            next;
        }

        # attribute is present, check that it exists for EKB XML defined target type
        my $ekb_xml_attr_type = $sys_xml_to_fapi_type_map{ $platInitMrwRequiredAttrs{$attr} };
        if ( !defined( $mrw_attrs{$mrw_name}{$attr}{$ekb_xml_attr_type} ) )
        {
            print ERRFILE "$mrw_name : Entry for $attr (type: ${ekb_xml_attr_type}) NOT found!\n";
            $global_error = 1;
            next;
        }

        # ensure that there is a value for each instance expected for that
        # target type
        my @inst_to_check = instances_to_check( $mrw_name, $ekb_xml_attr_type );

        foreach my $instance_id (@inst_to_check)
        {
            if ( !defined( $mrw_attrs{$mrw_name}{$attr}{$ekb_xml_attr_type}{$instance_id} ) )
            {
                print ERRFILE
                    "$mrw_name : Entry for $attr (type: ${ekb_xml_attr_type}, instance: ${instance_id}) NOT found!\n";
                $global_error = 1;
            }
        }
    }
}

#------------------------------------------------------------------------------
# Close opened files
#------------------------------------------------------------------------------

close(ATTRS_PLATINIT_MRW_FILE);
close(ATTRS_PLATINIT_MRW_REQUIRED_FILE);
close(ATTRS_PLATINIT_NON_MRW_FILE);
close(ATTRS_NON_PLATINIT_FILE);
close(MRW_SPECIFIED_ATTR_FILE);
close(ERRFILE);

# Exit with error
if ($global_error)
{
    print "ERROR: Missing MRW attribute settings in XML files.  See $errFile\n";
    exit(1);
}

exit(0);
