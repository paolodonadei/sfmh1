#!/usr/bin/perl -w


use strict;

use warnings;
use FileHandle;

 my @outLines;
 my $line;
 my $linenew;
my $fname;
my $zconfirm;
my $numArgs = $#ARGV + 1;
my $switch = "PERL";
my $flag=0;
if($numArgs<1)
{
print "You did not pass a file name,\n run the program as such: perl regexp.pl filename \n aborting\n";
exit;
}
$fname=$ARGV[0];
print "using file: $fname\n";
print "Is this ok?(y/n)\n";
chomp( $zconfirm = <STDIN> );
unless($zconfirm eq "y")
{
exit;
}



open ( FILE, $fname) or die "Cannot open file: $!";
while (  <FILE> ) {
	$line= $_;
	$linenew=$line;
	if(m/$switch/ && $flag==0)
	{
		$flag=1;
	}
	elsif(m/$switch/ && $flag==1)
	{
		$flag=0;
	}
	
	if($flag==1)
	{
    $linenew =~ s/cg\s*\[(\d+)\]\[(\d+)\]\s*/cvmSet\(cg\,$1\,$2\)/g;
	
	
    $linenew =~ s/\s*(\w+)\s*\[(\d+)\]\[(\d+)\]\s*/ cvmGet\($1\,$2\,$3\)/g;
	
	
	$linenew =~ s/0\.2e1/2/g;
	
	
	print "convert line:\n\n$line to:\n$linenew"
	
	
	
	
	
	}
	
	push(@outLines, $linenew);
}

close FILE;

print "Is this ok?(y/n)\n";
chomp( $zconfirm = <STDIN> );
unless($zconfirm eq "y")
{
exit;
}
open ( OUTFILE, ">$fname" );
print ( OUTFILE @outLines );
close ( OUTFILE );
