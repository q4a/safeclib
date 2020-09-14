#!/usr/bin/perl
#
# gen-u8gbreaks.pl
#
# September 2020, Reini Urban (WIP)
#
# Copyright (c) 2020 by Reini Urban
# All rights reserved.
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#------------------------------------------------------------------

use File::Basename 'dirname';
use File::Spec;

my $src = "GraphemeBreakProperty.txt";
my $cmd = "wget -q ftp://ftp.unicode.org/Public/UNIDATA/auxiliary/GraphemeBreakProperty.txt";
system $cmd  unless -e $src;
die "$cmd failed" unless -e $src;
open my $in, "<", $src or die "$! $src";

my $this = defined &DB::DB ? dirname $0 : dirname(__FILE__);
my $outfn = File::Spec->catfile($this, "u8gbreaks.h");
if (!-w $outfn && -e _) {
  chmod 0664, $outfn;
}
open my $out, ">", $outfn or die "$! $outfn";
my $hdr = <$in>;
chomp $hdr;
$hdr =~ s/^# //; #
# most %A are NULL (i.e. XX or no property), so we are better of with a hash
my ($prop, %A, %S);
while (<$in>) {
  if (/^[0-9A-F]/) {
    my @F = split;
    my ($from, $to);
    $prop = $F[2]; # the boundclass, i.e. GraphemeBreakProperty
    # Maybe: add Start or Other prop (for start-of-text state, when needed)
    $prop = "RI" if $prop eq "Regional_Indicator";
    $S{$prop} = $prop; # just to store the very same SV value in the big %A
    if ($F[0]=~ /^(\w+)\.\.(\w+)/) {
      ($from, $to) = ($1, $2);
      my $i = hex ("0x$from");
      my $j = hex ("0x$to");
      for ($i..$j) { # fill all ranges
        $A{$_} = $S{$prop};
      }
    } else {
      $from = $F[0];
      my $i = hex ($from);
      $A{$i} = $S{$prop};
    }
  }
}
close $in;

print $out <<EOF;
/* ex: set ro ft=c: -*- mode: c; buffer-read-only: t -*-
 * generated by gen-u8gbreaks.pl from unicode $hdr, do not modify!
 *
 * u8gbreaks.h
 *
 * September 2020, Reini Urban
 *
 * Copyright (c) 2020 by Reini Urban
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *------------------------------------------------------------------
 */

/* Sorted Unicode Grapheme_Cluster_Break properties.
*/
typedef enum {
  _U8_GBREAK_NONE                 = 0,
EOF

my $last = 0;
while (($_) = each %A) {
  $last = $_ if $_ > $last;
}
my $i = 1;
# keep the breaks sorted
my (@breaks, %breaks);
for (0 .. $last) {
  if (exists $A{$_}) {
    my $p = $A{$_};
    if (!exists $breaks{$p}) {
      push @breaks, $p;
      $breaks{$p} = !0; # SV_Yes is predefined
    }
  }
}
my $last_break = $breaks{$#breaks};
undef %breaks;
# warn $last;
for (@breaks) {
  printf $out "  _U8_GBREAK_%-20s = %d,", uc($_), $i++;
  printf $out " /* Regional_Indicator */" if $_ eq 'RI';
  printf $out "\n";
}
print $out "} _u8_gbreaks_t;\n\n";

my $maxfbits = length sprintf("%b", $last);
my $lasthex = sprintf "%x", $last;
my $maxgbits = length sprintf("%b", $i);
if ($maxgbits > 8 or $maxfbits > 24) {
  # we could compute 24 and 8 from $i, but we leave that to later.
  # most likely we would have to switch to a better datastructure then anyway.
  # max needed from: 20 bits (0xe0fff), gbreak: 4 bits
  die "too many GBREAK properties, extend the bitfield size (:$maxfbits, :$maxgbits)"
}

print $out <<EOF;
/* TODO: Use a roaring bitmap for better compression? Check gperf htable size.
   Or use from-to ranges skipping the NONE entries.
   Currently needed: from:$maxfbits ($lasthex), gbreak:$maxgbits bits
 */
struct _u8_gbreak_t {
  const unsigned from:24;
  const unsigned gbreak:8;
};
EOF

print $out <<EOF;

static const struct _u8_gbreak_t _u8_gbreaks\[\] = \{
EOF

my ($prevname, $name) = ("NULL", "NULL");
for (0 .. $last) {
  if (exists $A{$_}) {
    $prop = $A{$_};
    $name = "_U8_GBREAK_" . uc($prop);
  } else {
    $name = "_U8_GBREAK_NONE";
  }
  if ($prevname ne $name) {
    printf $out "  { 0x%x, %s },\n", $_, $name;
    $prevname = $name;
  }
}

=pod

As of Unicode 13.0

comparable utf8proc, from julia:
/** Boundclass property. (TR29) */
typedef enum {
  UTF8PROC_BOUNDCLASS_START              =  0, /**< Start */
  UTF8PROC_BOUNDCLASS_OTHER              =  1, /**< Other */
  UTF8PROC_BOUNDCLASS_CR                 =  2, /**< Cr */
  UTF8PROC_BOUNDCLASS_LF                 =  3, /**< Lf */
  UTF8PROC_BOUNDCLASS_CONTROL            =  4, /**< Control */
  UTF8PROC_BOUNDCLASS_EXTEND             =  5, /**< Extend */
  UTF8PROC_BOUNDCLASS_L                  =  6, /**< L */
  UTF8PROC_BOUNDCLASS_V                  =  7, /**< V */
  UTF8PROC_BOUNDCLASS_T                  =  8, /**< T */
  UTF8PROC_BOUNDCLASS_LV                 =  9, /**< Lv */
  UTF8PROC_BOUNDCLASS_LVT                = 10, /**< Lvt */
  UTF8PROC_BOUNDCLASS_REGIONAL_INDICATOR = 11, /**< Regional indicator */
  UTF8PROC_BOUNDCLASS_SPACINGMARK        = 12, /**< Spacingmark */
  UTF8PROC_BOUNDCLASS_PREPEND            = 13, /**< Prepend */
  UTF8PROC_BOUNDCLASS_ZWJ                = 14, /**< Zero Width Joiner */

  /* the following are no longer used in Unicode 11, but we keep
     the constants here for backward compatibility */
  UTF8PROC_BOUNDCLASS_E_BASE             = 15, /**< Emoji Base */
  UTF8PROC_BOUNDCLASS_E_MODIFIER         = 16, /**< Emoji Modifier */
  UTF8PROC_BOUNDCLASS_GLUE_AFTER_ZWJ     = 17, /**< Glue_After_ZWJ */
  UTF8PROC_BOUNDCLASS_E_BASE_GAZ         = 18, /**< E_BASE + GLUE_AFTER_ZJW */

  /* the Extended_Pictographic property is used in the Unicode 11
     grapheme-boundary rules, so we store it in the boundclass field */
  UTF8PROC_BOUNDCLASS_EXTENDED_PICTOGRAPHIC = 19,
  UTF8PROC_BOUNDCLASS_E_ZWG = 20, /* UTF8PROC_BOUNDCLASS_EXTENDED_PICTOGRAPHIC + ZWJ */
} utf8proc_boundclass_t;

=cut

print $out "};\n";
close $out;
chmod 0444, $outfn;
