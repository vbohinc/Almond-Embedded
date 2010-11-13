use GD;
use Data::Dumper;
use POSIX;

my @fonts = (
	{width => 6, height => 8, font_face => GD::gdTinyFont},
	{width => 7, height => 13, font_face => GD::gdSmallFont},
	{width => 9, height => 15, font_face => GD::gdGiantFont}
);

my $ascii_index_from = 32;
my $ascii_index_to = 126;

my $font_size = 0;
for my $font(@fonts){
	#print "Rendering font size ".$font->{"width"}."x".$font->{"height"}."...\n";
	
	my @size = ($font->{"width"} * ($ascii_index_to - $ascii_index_from), $font->{"height"});
	my $preview_image = GD::Image->new(@size);
	$preview_image->rectangle(0, 0, $preview_image->width(), $preview_image->height(), $preview_image->colorAllocate(255,255,255));
	
	my $count = 0;
	print "\n// FONT SIZE: ".$font_size." (".$font->{"width"}."x".$font->{"height"}.")\n";
	my $array_length = POSIX::ceil(($font->{"width"} * $font->{"height"}) / 8);
	my $font_array = "const uint8_t font_".$font_size."[][".$array_length."] PROGMEM = { {".$font->{"width"}.", ".$font->{"height"};
	for(my $i = 2; $i < $array_length; $i++){
		$font_array .= ", 0";
	}
	$font_array .= "},\n";
	for($ascii_index = $ascii_index_from; $ascii_index <= $ascii_index_to; $ascii_index++){
		# Allocate new image and paint it white
		my @size = ($font->{"width"}, $font->{"height"});
		my $image = GD::Image->new(@size);
		$image->rectangle(0, 0, $font->{"width"}, $font->{"height"}, $image->colorAllocate(255,255,255));
		
		# Draw character
		$image->string($font->{"font_face"}, 0, 0, chr($ascii_index), $image->colorAllocate(0,0,0));
		$preview_image->string($font->{"font_face"}, $count * $font->{"width"}, 0, chr($ascii_index), $preview_image->colorAllocate(0,0,0));

		#Generate bytes
		$font_array .= "    {";
		my @bits = ();
		for($y = 0; $y < $font->{"height"}; $y++){
			for($x = 0; $x < $font->{"width"}; $x++){
				my ($r,$g,$b) = $image->rgb($image->getPixel($x, $y));
				if($r == 0 and $g == 0 and $b == 0){
					push(@bits, 1);
				}else{
					push(@bits, 0);
				}
				if($#bits == 7){
					$font_array .= bits_to_byte(@bits).",";
					@bits = ();
				}
			}	
		}
		if($#bits > -1){
			$font_array .= bits_to_byte(@bits);
		}else{
			$font_array = substr($font_array, 0, length($font_array) - 1);
		}
		
		$font_array .= "}";
		$font_array .= "," if($ascii_index < $ascii_index_to);
		$font_array .= " " if($ascii_index == $ascii_index_to);
		$font_array .= " // ".chr($ascii_index)." (ASCII ".$ascii_index.")\n";
		undef $image;
		$count++;
	}
	$font_array .= "};";
	print $font_array."\n";
	open FILE, ">", "font_size_".$font_size.".png" or die $!;
	print FILE $preview_image->png();
	close FILE;
	
	$font_size++;
}

sub bits_to_byte{
	my @bits = @_;
	my $byte = 0;
	$byte += 1 * @bits[7];
	$byte += 2 * @bits[6];
	$byte += 4 * @bits[5];
	$byte += 8 * @bits[4];
	$byte += 16 * @bits[3];
	$byte += 32 * @bits[2];
	$byte += 64 * @bits[1];
	$byte += 128 * @bits[0];
	return sprintf("%3i", $byte);
}